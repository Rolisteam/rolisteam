#include "servermanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include "passwordaccepter.h"

#include "ipbanaccepter.h"
#include "iprangeaccepter.h"
#include "network/networkmessagewriter.h"
#include "timeaccepter.h"

ServerManager::ServerManager(QObject* parent) : QObject(parent), m_server(nullptr), m_state(Off)
{
    qRegisterMetaType<NetworkMessage*>("NetworkMessage*");

    m_model.reset(new ChannelModel());

    connect(m_model.get(), &ChannelModel::totalSizeChanged, this, &ServerManager::memoryChannelChanged);
    connect(m_model.get(), &ChannelModel::modelChanged, this, &ServerManager::sendOffModelToAll);

    m_msgDispatcher= new MessageDispatcher(this);
    connect(this, &ServerManager::messageMustBeDispatched, m_msgDispatcher, &MessageDispatcher::dispatchMessage,
            Qt::QueuedConnection);

    connect(m_msgDispatcher, &MessageDispatcher::messageForAdmin, this, &ServerManager::processMessageAdmin);
    m_model->addChannel("default", "");

    PasswordAccepter* tmp2= new PasswordAccepter();

    m_corEndProcess= tmp2;
    tmp2->setNext(nullptr);

    m_corConnection= new IpBanAccepter();

    IpRangeAccepter* tmp= new IpRangeAccepter();
    TimeAccepter* tmp3= new TimeAccepter();
    m_corConnection->setNext(tmp);
    tmp->setNext(tmp3);
    tmp3->setNext(nullptr);

    m_adminAccepter= new PasswordAccepter(PasswordAccepter::Admin);
    m_adminAccepter->setNext(nullptr);
}

ServerManager::~ServerManager()
{
    if(nullptr != m_server)
    {
        delete m_server;
        m_server= nullptr;
    }

    if(nullptr != m_corConnection)
    {
        delete m_corConnection;
        m_corConnection= nullptr;
    }

    if(nullptr != m_corEndProcess)
    {
        delete m_corEndProcess;
        m_corEndProcess= nullptr;
    }

    if(nullptr != m_adminAccepter)
    {
        delete m_adminAccepter;
        m_adminAccepter= nullptr;
    }
}

int ServerManager::getPort() const
{
    return m_port;
}
void ServerManager::startListening()
{
    if(m_server == nullptr)
    {
        m_server= new RServer(this, getValue(QStringLiteral("ThreadCount")).toInt());
    }
    ++m_tryCount;
    if(m_server->listen(QHostAddress::Any, static_cast<quint16>(getValue(QStringLiteral("port")).toInt())))
    {
        setState(Listening);
        emit sendLog(tr("Rolisteam Server is on!"), LogController::Info);
    }
    else
    {
        emit sendLog(m_server->errorString(), LogController::Error);
        if(m_tryCount < getValue(QStringLiteral("TryCount")).toInt()
           || getValue(QStringLiteral("TryCount")).toInt() == 0)
        {
            emit sendLog(tr("Retry start server in %1s!").arg(getValue(QStringLiteral("TimeToRetry")).toInt()),
                         LogController::Info);
            QTimer::singleShot(getValue(QStringLiteral("TimeToRetry")).toInt(), this, SLOT(startListening()));
        }
        else
        {
            emit sendLog(tr("Retry count reached. Server stops trying."), LogController::Info);
            emit finished();
        }
    }
}
void ServerManager::stopListening()
{
    m_server->terminate();
    close();
}

void ServerManager::messageReceived(QByteArray array)
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        Channel* channel= client->getParentChannel();
        {
            emit messageMustBeDispatched(array, channel, client);
        }
    }
}

void ServerManager::initServerManager()
{
    // create channel
    int chCount= getValue("ChannelCount").toInt();
    int count= m_model->rowCount(QModelIndex());
    for(int i= count; i < chCount; ++i)
    {
        m_model->addChannel(QStringLiteral("Channel %1").arg(i), "");
    }
}

void ServerManager::initClient()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        m_connections.insert(client->getSocket(), client);
        sendEventToClient(client, TcpClient::CheckSuccessEvent);
    }
    else
    {
        sendEventToClient(client, TcpClient::CheckFailEvent);
    }
}
void ServerManager::sendEventToClient(TcpClient* client, TcpClient::ConnectionEvent event)
{
    QMetaObject::invokeMethod(client, "sendEvent", Qt::QueuedConnection, Q_ARG(TcpClient::ConnectionEvent, event));
}

/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerManager::serverAcceptClient(TcpClient* client)
{
    if(nullptr != client)
    {
        QMap<QString, QVariant> data(m_parameters);
        data["currentIp"]= client->getIpAddress();
        if(m_corConnection->runAccepter(data))
        {
            sendEventToClient(client, TcpClient::ControlSuccessEvent);
        }
        else
        {
            sendEventToClient(client, TcpClient::ControlFailEvent);
        }
    }
}
void ServerManager::checkAuthToServer(TcpClient* client)
{
    if(nullptr != client)
    {
        QMap<QString, QVariant> data(m_parameters);
        data["currentIp"]= client->getIpAddress();
        data["userpassword"]= client->getServerPassword();
        if(m_corEndProcess->runAccepter(data))
        {
            m_model->addConnectionToDefaultChannel(client);
            sendEventToClient(client, TcpClient::ServerAuthSuccessEvent);
            sendOffModel(client);
        }
        else
        {
            sendEventToClient(client, TcpClient::ServerAuthFailEvent);
        }
    }
}
void ServerManager::checkAuthAsAdmin(TcpClient* client)
{
    QMap<QString, QVariant> data(m_parameters);
    data["userpassword"]= client->getAdminPassword();
    if(m_adminAccepter->runAccepter(data))
    {
        sendEventToClient(client, TcpClient::AdminAuthSuccessEvent);
    }
    else
    {
        sendEventToClient(client, TcpClient::AdminAuthFailEvent);
    }
}

void ServerManager::memoryChannelChanged(quint64 size)
{
    if(size > m_parameters["memorySize"].toULongLong())
    {
        m_model->emptyChannelMemory();
    }
}
void ServerManager::checkAuthToChannel(TcpClient* client, QString channelId, QByteArray password)
{
    QMap<QString, QVariant> data(m_parameters);
    auto item= m_model->getItemById(channelId);
    auto channel= dynamic_cast<Channel*>(item);

    auto eventToSend= TcpClient::ChannelAuthSuccessEvent;

    if(nullptr == channel)
        eventToSend= TcpClient::ChannelAuthFailEvent;

    if(channel->password() != password)
        eventToSend= TcpClient::ChannelAuthFailEvent;

    if((m_corEndProcess->runAccepter(data)) && (eventToSend != TcpClient::ChannelAuthFailEvent))
    {
        if(!m_model->addConnectionToChannel(channelId, client))
        {
            m_model->addConnectionToDefaultChannel(client);
        }
    }
    else
    {
        eventToSend= TcpClient::ChannelAuthFailEvent;
    }
    sendEventToClient(client, eventToSend);
}
/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerManager::sendOffAdminAuthSuccessed()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthSucessed);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        sendOffModel(client);
    }
}
void ServerManager::sendOffAdminAuthFail()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AdminAuthFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
    emit sendLog(
        tr("Authentification as Admin fails: %2 - %1, Wrong password.").arg(client->getName(), client->getIpAddress()),
        LogController::Info);
}
void ServerManager::sendOffAuthSuccessed()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationSucessed);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
        sendOffModel(client);
    }
}
void ServerManager::sendOffAuthFail()
{
    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg
            = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::AuthentificationFail);
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
    emit sendLog(
        tr("Authentification fails: %1 try to connect to the server with wrong password.").arg(client->getIpAddress()),
        LogController::Info);
}
void ServerManager::kickClient(QString id, bool isAdmin, QString senderId)
{
    m_model->kick(id, isAdmin, senderId);
    sendOffModelToAll();

    TcpClient* client= nullptr;
    auto keys= m_connections.keys();
    for(auto& key : keys)
    {
        auto value= m_connections[key];
        if(value->getId() == id)
        {
            client= value;
        }
    }
    emit sendLog(tr("User has been kick out: %2 - %1.").arg(client->getName(), client->getIpAddress()),
                 LogController::Info);

    if(nullptr != client)
    {
        removeClient(client);
    }
}

void ServerManager::banClient(QString id, bool isAdmin, QString senderId)
{
    // TODO implement this function
    Q_UNUSED(id)
    Q_UNUSED(isAdmin)
    Q_UNUSED(senderId)
}

void ServerManager::processMessageAdmin(NetworkMessageReader* msg, Channel* chan, TcpClient* tcp)
{
    if(tcp == nullptr)
        return;

    bool isAdmin= tcp->isAdmin();
    bool isGM= tcp->isGM();
    auto sourceId= tcp->getPlayerId();
    switch(msg->action())
    {
    case NetMsg::Kicked:
    {
        QString id= msg->string8();
        kickClient(id, isAdmin, sourceId);
    }
    break;
    case NetMsg::BanUser:
        // TODO
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
            channel->read(*msg);
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
            sendEventToClient(tcp, TcpClient::ChannelChanged);
        }
    }
    break;
    case NetMsg::SetChannelList:
    {
        if(isAdmin)
        {
            QByteArray data= msg->byteArray32();
            QJsonDocument doc= QJsonDocument::fromJson(data);
            if(!doc.isEmpty())
            {
                QJsonObject obj= doc.object();
                m_model->readDataJson(obj);
            }
        }
    }
    break;
    case NetMsg::DeleteChannel:
    {
        if(isAdmin)
        {
            QString id= msg->string8();
            m_model->removeChild(id);
            sendOffModelToAll();
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
                auto chan= dynamic_cast<Channel*>(item);
                if(chan)
                    chan->clearData();
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

void ServerManager::sendOffModel(TcpClient* client)
{
    if((nullptr != client)) //&&(!client->getName().isEmpty())
    {
        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::SetChannelList);
        QJsonDocument doc;
        QJsonObject obj;
        m_model->writeDataJson(obj);
        doc.setObject(obj);

        msg->byteArray32(doc.toJson());
        QMetaObject::invokeMethod(client, "sendMessage", Qt::QueuedConnection,
                                  Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)), Q_ARG(bool, true));
    }
}

void ServerManager::insertField(QString key, QVariant value, bool erase)
{
    if(!m_parameters.contains(key) || erase)
    {
        m_parameters.insert(key, value);
    }
}
QVariant ServerManager::getValue(QString key) const
{
    if(m_parameters.contains(key))
    {
        return m_parameters[key];
    }
    return QVariant();
}
ServerManager::ServerState ServerManager::getState() const
{
    return m_state;
}

void ServerManager::setState(const ServerManager::ServerState& state)
{
    if(m_state != state)
    {
        m_state= state;
        emit stateChanged(m_state);
    }

    if(m_state == Listening)
    {
        emit listening();
    }
}

void ServerManager::quit()
{
    if(!sender())
        return;
    emit finished();
}

void ServerManager::accept(qintptr handle, TcpClient* connection, QThread* thread)
{
    Q_UNUSED(thread);
    if(nullptr == connection)
        return;

    emit sendLog(tr("New Incoming Connection!"), LogController::Info);

    connect(connection, &TcpClient::dataReceived, this, &ServerManager::messageReceived, Qt::QueuedConnection); //
    connect(connection, &TcpClient::socketInitiliazed, this, &ServerManager::initClient, Qt::QueuedConnection);

    connect(connection, &TcpClient::serverAuthFail, this, &ServerManager::sendOffAuthFail, Qt::QueuedConnection);
    connect(connection, &TcpClient::serverAuthSuccess, this, &ServerManager::sendOffAuthSuccessed,
            Qt::QueuedConnection);

    connect(connection, &TcpClient::adminAuthFailed, this, &ServerManager::sendOffAdminAuthFail, Qt::QueuedConnection);
    connect(connection, &TcpClient::adminAuthSucceed, this, &ServerManager::sendOffAdminAuthSuccessed,
            Qt::QueuedConnection);
    connect(connection, &TcpClient::itemChanged, this, &ServerManager::sendOffModelToAll, Qt::QueuedConnection);

    connect(connection, &TcpClient::checkServerAcceptClient, this, &ServerManager::serverAcceptClient,
            Qt::QueuedConnection);
    connect(connection, &TcpClient::checkServerPassword, this, &ServerManager::checkAuthToServer, Qt::QueuedConnection);
    connect(connection, &TcpClient::checkAdminPassword, this, &ServerManager::checkAuthAsAdmin, Qt::QueuedConnection);
    connect(connection, &TcpClient::checkChannelPassword, this, &ServerManager::checkAuthToChannel,
            Qt::QueuedConnection);
    connect(connection, &TcpClient::channelPassword, this, &ServerManager::setChannelPassword, Qt::QueuedConnection);

    connect(connection, &TcpClient::socketDisconnection, this, &ServerManager::disconnected, Qt::QueuedConnection);
    connect(connection, &TcpClient::socketError, this, &ServerManager::error, Qt::QueuedConnection);
    connection->setSocketHandleId(handle);

    // emit clientAccepted();
    QMetaObject::invokeMethod(connection, &TcpClient::startReading, Qt::QueuedConnection);
}

void ServerManager::sendOffModelToAll()
{
    auto values= m_connections.values();
    for(auto& connection : values)
    {
        sendOffModel(connection);
    }
}

void ServerManager::disconnected()
{
    emit sendLog(tr("Disconnected!"), LogController::Info);
    if(!sender())
        return;

    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(!client)
        return;

    removeClient(client);
}

void ServerManager::removeClient(TcpClient* client)
{
    emit client->isReady();

    auto socket= client->getSocket();
    m_connections.remove(socket);

    m_model->removeChild(client->getId());
    client->disconnect();

    if(nullptr != socket)
    {
        socket->disconnect();
        if(socket->isOpen())
        {
            socket->close();
        }
        socket->deleteLater();
    }
    sendOffModelToAll();
    client->deleteLater();
}
void ServerManager::setChannelPassword(QString chanId, QByteArray passwd)
{
    auto item= m_model->getItemById(chanId);
    if(nullptr == item)
        return;

    auto channel= dynamic_cast<Channel*>(item);
    if(nullptr == channel)
        return;

    channel->setPassword(passwd);
    sendOffModelToAll();
}

void ServerManager::close()
{
    auto clients= m_connections.values();
    std::for_each(clients.begin(), clients.end(), [](TcpClient* client) {
        // removeClient(client);
        QMetaObject::invokeMethod(client, &TcpClient::closeConnection, Qt::QueuedConnection);
    });
    emit closed();
}

void ServerManager::error(QAbstractSocket::SocketError socketError)
{
    if(QAbstractSocket::RemoteHostClosedError == socketError)
        return;
    if(!sender())
        return;

    TcpClient* client= qobject_cast<TcpClient*>(sender());
    if(!client)
        return;

    auto socket= client->getSocket();

    if(!socket)
        return;

    emit sendLog(socket->errorString(), LogController::Error);
}
