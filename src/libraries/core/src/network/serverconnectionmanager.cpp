#include <network/serverconnectionmanager.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include "network/ipbanaccepter.h"
#include "network/iprangeaccepter.h"
#include "network/messagedispatcher.h"
#include "network/networkmessagewriter.h"
#include "network/passwordaccepter.h"
#include "network/serverconnection.h"
#include "qhostaddress.h"

#include <QReadLocker>
#include <QWriteLocker>

void sendEventToClient(ServerConnection* client, ServerConnection::ConnectionEvent event)
{
    QMetaObject::invokeMethod(client, "sendEvent", Qt::QueuedConnection,
                              Q_ARG(ServerConnection::ConnectionEvent, event));
}

ServerConnectionManager::ServerConnectionManager(const QMap<QString, QVariant>& parameters, QObject* parent)
    : QObject(parent), m_model(new ChannelModel), m_parameters(parameters)
{
    int chCount= parameters.value("ChannelCount", 1).toInt();
    int count= m_model->rowCount(QModelIndex());
    for(int i= count; i < chCount; ++i)
    {
        m_model->addChannel(QStringLiteral("Channel %1").arg(i), {});
    }

    qRegisterMetaType<NetworkMessage*>("NetworkMessage*");

    connect(m_model.get(), &ChannelModel::totalSizeChanged, this, &ServerConnectionManager::memoryChannelChanged);

    m_msgDispatcher= new MessageDispatcher(this);
    connect(this, &ServerConnectionManager::messageMustBeDispatched, m_msgDispatcher,
            &MessageDispatcher::dispatchMessage, Qt::QueuedConnection);

    connect(m_msgDispatcher, &MessageDispatcher::messageForAdmin, this, &ServerConnectionManager::processMessageAdmin);

    m_corEndProcess.reset(new PasswordAccepter());
    m_tcpConnectionAccepter.reset(new IpBanAccepter());
    m_tcpConnectionAccepter->setNext(new IpRangeAccepter());
    m_adminAccepter.reset(new PasswordAccepter(PasswordAccepter::Admin));
}

ServerConnectionManager::~ServerConnectionManager()
{
    // stopListening();
}

int ServerConnectionManager::countConnection() const
{
    QReadLocker locker(&m_lock);
    return m_connections.count();
}

/*bool ServerConnectionManager::startListening()
{
    qDebug() << "Start listening:";
    if(!m_server)
    {
        m_server.reset(new RServer(getValue(QStringLiteral("ThreadCount")).toInt()));
        connect(m_server.get(), &RServer::accepting, this, &ServerManager::accept, Qt::QueuedConnection);
        connect(m_server.get(), &RServer::finished, this, [this]() { setState(Stopped); });
    }

    if(m_server->listen(QHostAddress::Any, static_cast<quint16>(getValue(QStringLiteral("port")).toInt())))
    {
        setState(Listening);
        emit eventOccured(tr("Rolisteam Server is on!"), LogController::Info);
    }
    else
    {
        setState(Error);
        emit eventOccured(m_server->errorString(), LogController::Error);
        /*if(m_tryCount < getValue(QStringLiteral("TryCount")).toInt()
           || getValue(QStringLiteral("TryCount")).toInt() == 0)
        {
            emit eventOccured(tr("Retry start server in %1s!").arg(getValue(QStringLiteral("TimeToRetry")).toInt()),
                              LogController::Info);
            QTimer::singleShot(getValue(QStringLiteral("TimeToRetry")).toInt(), this, SLOT(startListening()));
        }
        else
        {
            emit eventOccured(tr("Retry count reached. Server stops trying."), LogController::Info);
            setState(Stopped); // on error
        }
    }
    return state() == Listening;
}*/

void ServerConnectionManager::messageReceived(QByteArray array)
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr == client)
        return;

    emit messageMustBeDispatched(array, client->getParentChannel(), client);
}

void ServerConnectionManager::initClient()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        qDebug() << "client insert" << client << client->name();
        m_connections.insert(client->getSocket(), client);
        sendEventToClient(client, ServerConnection::CheckSuccessEvent);
    }
    else
    {
        sendEventToClient(client, ServerConnection::CheckFailEvent);
    }
}

/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerConnectionManager::serverAcceptClient(ServerConnection* client)
{

    if(nullptr == client)
    {
        qDebug() << "client is null";
        return;
    }

    QMap<QString, QVariant> data(m_parameters);
    data["currentIp"]= client->getIpAddress();
    if(m_tcpConnectionAccepter->runAccepter(data))
    {
        sendEventToClient(client, ServerConnection::ControlSuccessEvent);
    }
    else
    {
        sendEventToClient(client, ServerConnection::ControlFailEvent);
    }
}
void ServerConnectionManager::checkAuthToServer(ServerConnection* client)
{
    qDebug() << "check auth to server";
    if(nullptr == client)
        return;
    qDebug() << "cilent is fully defined" << client->isFullyDefined();

    QMap<QString, QVariant> data(m_parameters);
    data["currentIp"]= client->getIpAddress();
    data["userpassword"]= client->getServerPassword();
    qDebug() << "Parameter :" << data;
    if(m_corEndProcess->runAccepter(data))
    {
        m_model->addConnectionToDefaultChannel(client);
        sendEventToClient(client, ServerConnection::ServerAuthSuccessEvent);
        // sendOffModel(client);
        qDebug() << "server auth successed";
    }
    else
    {
        sendEventToClient(client, ServerConnection::ServerAuthFailEvent);
        qDebug() << "server auth failed";
    }
}
void ServerConnectionManager::checkAuthAsAdmin(ServerConnection* client)
{
    QMap<QString, QVariant> data(m_parameters);
    data["userpassword"]= client->getAdminPassword();
    if(m_adminAccepter->runAccepter(data))
    {
        sendEventToClient(client, ServerConnection::AdminAuthSuccessEvent);
    }
    else
    {
        sendEventToClient(client, ServerConnection::AdminAuthFailEvent);
    }
}

void ServerConnectionManager::memoryChannelChanged(quint64 size)
{
    if(size > m_parameters["memorySize"].toULongLong())
    {
        m_model->emptyChannelMemory();
    }
}
void ServerConnectionManager::checkAuthToChannel(ServerConnection* client, QString channelId, QByteArray password)
{
    QMap<QString, QVariant> data(m_parameters);
    auto item= m_model->getItemById(channelId);
    auto channel= dynamic_cast<Channel*>(item);

    auto eventToSend= ServerConnection::ChannelAuthSuccessEvent;

    if(nullptr == channel)
    {
        eventToSend= ServerConnection::ChannelAuthFailEvent;
        sendEventToClient(client, eventToSend);
        return;
    }

    if(channel->password() != password)
        eventToSend= ServerConnection::ChannelAuthFailEvent;

    if((m_corEndProcess->runAccepter(data)) && (eventToSend != ServerConnection::ChannelAuthFailEvent))
    {
        if(!m_model->addConnectionToChannel(channelId, client))
        {
            m_model->addConnectionToDefaultChannel(client);
        }
    }
    else
    {
        eventToSend= ServerConnection::ChannelAuthFailEvent;
    }
    sendEventToClient(client, eventToSend);
}
/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerConnectionManager::sendOffAdminAuthSuccessed()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthSucessed);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        // sendOffModel(client);
    }
}
void ServerConnectionManager::sendOffAdminAuthFail()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
    emit eventOccured(
        tr("Authentification as Admin fails: %2 - %1, Wrong password.").arg(client->name(), client->getIpAddress()),
        LogController::Info);
}
void ServerConnectionManager::sendOffAuthSuccessed()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationSucessed);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        // sendOffModel(client);
    }
}
void ServerConnectionManager::sendOffAuthFail()
{
    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
    emit eventOccured(
        tr("Authentification fails: %1 try to connect to the server with wrong password.").arg(client->getIpAddress()),
        LogController::Info);
}
void ServerConnectionManager::kickClient(QString id, bool isAdmin, QString senderId)
{
    m_model->kick(id, isAdmin, senderId);
    // sendOffModelToAll();

    ServerConnection* client= nullptr;
    auto keys= m_connections.keys();
    for(auto& key : keys)
    {
        auto value= m_connections[key];
        if(value->uuid() == id)
        {
            client= value;
        }
    }
    emit eventOccured(tr("User has been kick out: %2 - %1.").arg(client->name(), client->getIpAddress()),
                      LogController::Info);

    if(nullptr != client)
    {
        // removeClient(client);
    }
}

void ServerConnectionManager::banClient(QString id, bool isAdmin, QString senderId)
{
    // TODO implement this function
    Q_UNUSED(id)
    Q_UNUSED(isAdmin)
    Q_UNUSED(senderId)
}

void ServerConnectionManager::processMessageAdmin(NetworkMessageReader* msg, Channel* chan, ServerConnection* tcp)
{
    if(tcp == nullptr)
        return;

    bool isAdmin= tcp->isAdmin();
    bool isGM= tcp->isGM();
    auto sourceId= tcp->playerId();
    switch(msg->action())
    {
    case NetMsg::Kicked:
    {
        QString id= msg->string8();
        kickClient(id, isAdmin, sourceId);
    }
    break;
    case NetMsg::BanUser:
    {
        QString id= msg->string8();
        banClient(id, isAdmin, sourceId);
    }
    break;
    case NetMsg::RenameChannel:
    {
        if(isAdmin)
        {
            QString idChan= msg->string8();
            QString newName= msg->string32();
            m_model->renameChannel(sourceId, idChan, newName);
        }
    }
    break;
    case NetMsg::AddChannel:
    {
        if(isAdmin)
        {
            QString idparent= msg->string8();
            TreeItem* parentItem= m_model->getItemById(idparent);
            Channel* dest= static_cast<Channel*>(parentItem);

            auto channel= new Channel();
            // channel->read(*msg);
            m_model->addChannelToChannel(channel, dest);
        }
    }
    break;
    case NetMsg::JoinChannel:
    {
        QString id= msg->string8();
        QString idClient= msg->string8();
        TreeItem* item= m_model->getItemById(id);
        Channel* dest= static_cast<Channel*>(item);
        if(nullptr != dest && !dest->locked())
        {
            m_model->moveClient(chan, idClient, dest);
            sendEventToClient(tcp, ServerConnection::ChannelChanged);
        }
    }
    break;
    case NetMsg::SetChannelList:
    {
        qDebug() << "Server received channellist";
        /*if(isAdmin)
        {
            QByteArray data= msg->byteArray32();
            QJsonDocument doc= QJsonDocument::fromJson(data);
            if(!doc.isEmpty())
            {
                QJsonObject obj= doc.object();
               // m_model->readDataJson(obj);
            }
        }*/
    }
    break;
    case NetMsg::DeleteChannel:
    {
        if(isAdmin)
        {
            QString id= msg->string8();
            m_model->removeChild(id);
            // sendOffModelToAll();
        }
    }
    break;
    case NetMsg::MoveChannel:
    case NetMsg::AdminPassword:
        break;
    case NetMsg::ResetChannel:
    {
        if(isGM)
        {
            if(nullptr != chan)
                chan->clearData();
        }
        else if(isAdmin)
        {
            QString id= msg->string8();
            auto item= m_model->getItemById(id);
            if(!item->isLeaf())
            {
                auto channel= dynamic_cast<Channel*>(item);
                if(channel)
                    channel->clearData();
            }
        }
    }
    break;
    case NetMsg::LockChannel:
    case NetMsg::UnlockChannel:
        if(isGM)
        {
            if(nullptr != chan)
                chan->setLocked(msg->action() == NetMsg::LockChannel ? true : false);
        }
        break;
    default:
        break;
    }
}

/*void ServerManager::sendOffModel(ServerConnection* client)
{
    if(nullptr == client)
        return;

    qDebug() << "ServerManager Send off channel model" << sender();
    static QMap<ServerConnection*, QByteArray> model;
    QJsonDocument doc;
    QJsonObject obj;
    m_model->writeDataJson(obj);
    doc.setObject(obj);

    auto b= doc.toJson();

    if(b != model[client])
    {
        model[client]= b;

        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::SetChannelList);
        msg->byteArray32(doc.toJson());
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
}*/

ChannelModel* ServerConnectionManager::channelModel() const
{
    return m_model.get();
}

const QHash<QTcpSocket*, ServerConnection*> ServerConnectionManager::connections() const
{
    return m_connections;
}

void ServerConnectionManager::quit()
{
    if(!sender())
        return;

    auto sockets= m_connections.keys();
    std::for_each(sockets.begin(), sockets.end(), [this](QTcpSocket* socket) { removeSocket(socket); });

    m_connections.clear();

    emit finished();
}

void ServerConnectionManager::accept(qintptr handle, ServerConnection* connection)
{
    if(nullptr == connection)
        return;

    emit eventOccured(tr("New Incoming Connection!"), LogController::Info);

    connect(connection, &ServerConnection::dataReceived, this, &ServerConnectionManager::messageReceived,
            Qt::QueuedConnection); //
    connect(connection, &ServerConnection::socketInitiliazed, this, &ServerConnectionManager::initClient,
            Qt::QueuedConnection);

    connect(connection, &ServerConnection::serverAuthFail, this, &ServerConnectionManager::sendOffAuthFail,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::serverAuthSuccess, this, &ServerConnectionManager::sendOffAuthSuccessed,
            Qt::QueuedConnection);

    connect(connection, &ServerConnection::adminAuthFailed, this, &ServerConnectionManager::sendOffAdminAuthFail,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::adminAuthSucceed, this, &ServerConnectionManager::sendOffAdminAuthSuccessed,
            Qt::QueuedConnection);

    connect(
        connection, &ServerConnection::itemChanged, this,
        [this]() {
            qDebug() << "connection ItemChanged";
            // sendOffModelToAll();
        },
        Qt::QueuedConnection);

    connect(connection, &ServerConnection::checkServerAcceptClient, this, &ServerConnectionManager::serverAcceptClient,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::checkServerPassword, this, &ServerConnectionManager::checkAuthToServer,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::checkAdminPassword, this, &ServerConnectionManager::checkAuthAsAdmin,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::checkChannelPassword, this, &ServerConnectionManager::checkAuthToChannel,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::channelPassword, this, &ServerConnectionManager::setChannelPassword,
            Qt::QueuedConnection);

    connect(connection, &ServerConnection::socketDisconnection, this, &ServerConnectionManager::disconnectedUser,
            Qt::QueuedConnection);
    connect(connection, &ServerConnection::socketError, this, &ServerConnectionManager::error, Qt::QueuedConnection);
    connection->setSocketHandleId(handle);

    // emit clientAccepted();
    QMetaObject::invokeMethod(connection, &ServerConnection::startReading, Qt::QueuedConnection);
}

void ServerConnectionManager::disconnectedUser()
{
    if(!sender())
        return;

    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(!client)
        return;

    emit eventOccured(tr("User %1 has been disconnected!").arg(client->playerName()), LogController::Info);
    // removeClient(client);
}

void ServerConnectionManager::removeSocket(QTcpSocket* socket)
{
    if(!socket)
        return;
    if(!m_connections.contains(socket))
        return;

    qDebug() << this << "removing socket = " << socket;

    if(socket->isOpen())
    {
        qDebug() << this << "socket is open, attempting to close it " << socket;
        QMetaObject::invokeMethod(socket, &QTcpSocket::close);
    }

    qDebug() << this << "deleting socket" << socket;
    auto client= m_connections.value(socket);
    m_connections.remove(socket);

    QMetaObject::invokeMethod(socket, &QTcpSocket::deleteLater);
    client->deleteLater();

    qDebug() << this << "client count = " << m_connections.count();
}
void ServerConnectionManager::setChannelPassword(QString chanId, QByteArray passwd)
{
    auto item= m_model->getItemById(chanId);
    if(nullptr == item)
        return;

    auto channel= dynamic_cast<Channel*>(item);
    if(nullptr == channel)
        return;

    channel->setPassword(passwd);
    // sendOffModelToAll();
}

void ServerConnectionManager::error(QAbstractSocket::SocketError socketError)
{
    if(QAbstractSocket::RemoteHostClosedError == socketError)
        return;
    if(!sender())
        return;

    ServerConnection* client= qobject_cast<ServerConnection*>(sender());
    if(!client)
        return;

    auto socket= client->getSocket();

    if(!socket)
        return;

    emit eventOccured(socket->errorString(), LogController::Error);
}
