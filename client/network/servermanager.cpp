#include "servermanager.h"

#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>



#include "passwordaccepter.h"

#include "network/networkmessagewriter.h"
#include "timeaccepter.h"
#include "iprangeaccepter.h"
#include "ipbanaccepter.h"


ServerManager::ServerManager(QObject *parent)
    : QObject(parent),m_server(nullptr), m_state(Off)
{
    qRegisterMetaType<NetworkMessage*>("NetworkMessage*");

    m_model = new ChannelModel();
    m_msgDispatcher = new MessageDispatcher(this);
    connect(this,SIGNAL(messageMustBeDispatched(QByteArray,Channel*,TcpClient*)),m_msgDispatcher,SLOT(dispatchMessage(QByteArray,Channel*,TcpClient*)),Qt::QueuedConnection);

    connect(m_msgDispatcher, SIGNAL(messageForAdmin(NetworkMessageReader*,Channel*,TcpClient*)),this,SLOT(processMessageAdmin(NetworkMessageReader*,Channel*,TcpClient*)));
    m_defaultChannelIndex = m_model->addChannel("default","");

    PasswordAccepter* tmp2 = new PasswordAccepter();

    m_corEndProcess = tmp2;
    tmp2->setNext(nullptr);

    m_corConnection = new IpBanAccepter();

    IpRangeAccepter* tmp = new IpRangeAccepter();
    TimeAccepter* tmp3 = new TimeAccepter();
    m_corConnection->setNext(tmp);
    tmp->setNext(tmp3);
    tmp3->setNext(nullptr);


    m_adminAccepter = new PasswordAccepter(PasswordAccepter::Admin);
    m_adminAccepter->setNext(nullptr);
}

int ServerManager::getPort() const
{
    return m_port;
}
void ServerManager::startListening()
{
    if (m_server == nullptr)
    {
        m_server = new RServer(this,getValue(QStringLiteral("ThreadCount")).toInt());
    }
    ++m_tryCount;
    if (m_server->listen(QHostAddress::Any,getValue(QStringLiteral("port")).toInt()))
    {
        setState(Listening);
        emit sendLog(tr("Rolisteam Server is on!"), LogController::Info);
    }
    else
    {
        emit sendLog(m_server->errorString(), LogController::Error);
        if(m_tryCount < getValue(QStringLiteral("TryCount")).toInt() || getValue(QStringLiteral("TryCount")).toInt() == 0)
        {
            emit sendLog(tr("Retry start server in %1s!").arg(getValue(QStringLiteral("TimeToRetry")).toInt()), LogController::Info);
            QTimer::singleShot(getValue(QStringLiteral("TimeToRetry")).toInt(),this,SLOT(startListening()));
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
    m_server->close();
}

void ServerManager::messageReceived(QByteArray array)
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        Channel* channel = client->getParentChannel();
        {
            emit messageMustBeDispatched(array,channel,client);
        }
    }
}

void ServerManager::initServerManager()
{
   //create channel
   int chCount = getValue("ChannelCount").toInt();
   int count = m_model->rowCount(QModelIndex());
   for(int i = count; i < chCount ; ++i)
   {
       m_model->addChannel(QStringLiteral("Channel %1").arg(i),"");
   }
}

void ServerManager::initClient()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        m_connections.insert(client->getSocket(),client);
        sendEventToClient(client,TcpClient::CheckSuccessEvent);
    }
    else
    {
        sendEventToClient(client,TcpClient::CheckFailEvent);
    }
}
void ServerManager::sendEventToClient(TcpClient* client, TcpClient::ConnectionEvent event)
{
    QMetaObject::invokeMethod(client,"sendEvent",Qt::QueuedConnection,Q_ARG(TcpClient::ConnectionEvent,event));
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
        QMap<QString,QVariant> data(m_parameters);
        data["currentIp"]=client->getIpAddress();
        if(m_corConnection->isValid(data))
        {
            sendEventToClient(client,TcpClient::ControlSuccessEvent);
        }
        else
        {
            sendEventToClient(client,TcpClient::ControlFailEvent);
        }
    }
}
void ServerManager::checkAuthToServer(TcpClient* client)
{
    if(nullptr != client)
    {
        QMap<QString,QVariant> data(m_parameters);
        data["currentIp"]=client->getIpAddress();
        data["userpassword"] = client->getServerPassword();
        if(m_corEndProcess->isValid(data))
        {
            m_model->addConnectionToDefaultChannel(client);
            sendEventToClient(client,TcpClient::ServerAuthSuccessEvent);
            sendOffModel(client);
        }
        else
        {
            sendEventToClient(client,TcpClient::ServerAuthFailEvent);
        }
    }
}
void ServerManager::checkAuthAsAdmin(TcpClient* client)
{
    QString passwd = client->getAdminPassword();
    QMap<QString,QVariant> data(m_parameters);
    data["userpassword"]=passwd;
    if(m_adminAccepter->isValid(data))
    {
        sendEventToClient(client,TcpClient::AdminAuthSuccessEvent);
    }
    else
    {
        sendEventToClient(client,TcpClient::AdminAuthFailEvent);
    }
}
void ServerManager::checkAuthToChannel(TcpClient* client,QString channelId, QByteArray password)
{
    QMap<QString,QVariant> data(m_parameters);
    auto item = m_model->getItemById(channelId);
    auto channel = dynamic_cast<Channel*>(item);

    auto eventToSend = TcpClient::ChannelAuthSuccessEvent;

    if(nullptr == channel)
        eventToSend = TcpClient::ChannelAuthFailEvent;

    if(channel->password() != password)
        eventToSend = TcpClient::ChannelAuthFailEvent;

    if((m_corEndProcess->isValid(data)) && (eventToSend != TcpClient::ChannelAuthFailEvent))
    {
        if(!m_model->addConnectionToChannel(channelId,client))
        {
            m_model->addConnectionToDefaultChannel(client);
        }
    }
    else
    {
        eventToSend = TcpClient::ChannelAuthFailEvent;
    }
    sendEventToClient(client,eventToSend);
}
/////////////////////////////////////////////////////////
///
/// Slot to perform check during connection process.
///
////////////////////////////////////////////////////////
void ServerManager::sendOffAdminAuthSuccessed()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::AdminAuthSucessed);
        QMetaObject::invokeMethod(client,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,static_cast<NetworkMessage*>(msg)),Q_ARG(bool,true));
        sendOffModel(client);
    }
}
void ServerManager::sendOffAdminAuthFail()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::AdminAuthFail);
        QMetaObject::invokeMethod(client,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,static_cast<NetworkMessage*>(msg)),Q_ARG(bool,true));
    }
    emit sendLog(tr("Authentification as Admin fails: %2 - %1, Wrong password.")
                 .arg(client->getName())
                 .arg(client->getIpAddress()), LogController::Info);
}
void ServerManager::sendOffAuthSuccessed()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::AuthentificationSucessed);
        QMetaObject::invokeMethod(client,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,static_cast<NetworkMessage*>(msg)),Q_ARG(bool,true));
        sendOffModel(client);
    }
}
void ServerManager::sendOffAuthFail()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::AuthentificationFail);
        QMetaObject::invokeMethod(client,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,static_cast<NetworkMessage*>(msg)),Q_ARG(bool,true));
    }
    emit sendLog(tr("Authentification fails: %1 try to connect to the server with wrong password.").arg(client->getIpAddress()), LogController::Info);
}
void ServerManager::kickClient(QString id)
{
    m_model->kick(id);
    sendOffModelToAll();

    TcpClient* client = nullptr;
    for(auto key : m_connections.keys())
    {
        auto value = m_connections[key];
        if(value->getId() == id)
        {
            client = value;
        }
    }
    emit sendLog(tr("User has been kick out: %2 - %1.")
                 .arg(client->getName())
                 .arg(client->getIpAddress()), LogController::Info);

    if(nullptr != client)
    {
        removeClient(client);
    }
}

void ServerManager::processMessageAdmin(NetworkMessageReader* msg,Channel* chan, TcpClient* tcp)
{
    bool isAdmin = tcp->isAdmin();
    switch (msg->action())
    {
        case NetMsg::Goodbye:

        break;
        case NetMsg::Kicked:
        {
            if(isAdmin)
            {
                QString id = msg->string8();
                kickClient(id);
            }
        }
        break;
        case NetMsg::MoveChannel:

        break;
        case NetMsg::AddChannel:
        {
            if(isAdmin)
            {
                QString idparent = msg->string8();
                TreeItem* parentItem = m_model->getItemById(idparent);
                Channel* dest = static_cast<Channel*>(parentItem);

                auto channel = new Channel();
                channel->read(*msg);
                m_model->addChannelToChannel(channel,dest);
            }
        }
        break;
        case NetMsg::JoinChannel:
        {
            if(isAdmin)
            {
                QString id = msg->string8();
                QString idClient = msg->string8();
                TreeItem* item = m_model->getItemById(id);
                TreeItem* clientItem = m_model->getItemById(idClient);
                TcpClient* client = static_cast<TcpClient*>(clientItem);
                Channel* dest = static_cast<Channel*>(item);
                if(nullptr != dest)
                {
                    chan->removeClient(client);
                    dest->addChild(client);
                    sendEventToClient(tcp,TcpClient::ServerAuthDataReceivedEvent);
                }
            }
            else
            {

            }
        }
        break;
        case NetMsg::SetChannelList:
        {
        if(isAdmin)
        {
            QByteArray data = msg->byteArray32();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if(!doc.isEmpty())
            {
                QJsonObject obj = doc.object();
                m_model->readDataJson(obj);
            }
          }
        }
            break;
    case NetMsg::DeleteChannel:
    {
        if(isAdmin)
        {
            QString id = msg->string8();
            m_model->removeChild(id);
            sendOffModelToAll();
        }
    }
        break;
    case NetMsg::AdminPassword:
    {

    }
        break;
        default:
            break;
    }
}

void ServerManager::sendOffModel(TcpClient* client)
{
    if((nullptr != client))//&&(!client->getName().isEmpty())
    {
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::SetChannelList);
        QJsonDocument doc;
        QJsonObject obj;
        m_model->writeDataJson(obj);
        doc.setObject(obj);

        msg->byteArray32(doc.toJson());
        QMetaObject::invokeMethod(client,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,static_cast<NetworkMessage*>(msg)),Q_ARG(bool,true));

    }
}

void ServerManager::insertField(QString key,QVariant value,bool erase)
{
    if(!m_parameters.contains(key)||erase)
    {
        m_parameters.insert(key,value);
    }
}
QVariant  ServerManager::getValue(QString key) const
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

void ServerManager::setState(const ServerManager::ServerState &state)
{
    if(m_state != state)
    {
        m_state = state;
        emit stateChanged(m_state);
    }

    if(m_state == Listening)
    {
        emit listening();
    }
}

void ServerManager::quit()
{
    if(!sender()) return;
    emit finished();
}

void ServerManager::accept(qintptr handle, TcpClient *connection,QThread* thread)
{
    Q_UNUSED(thread);
    emit sendLog(tr("New Incoming Connection!"), LogController::Info);

    connect(connection,SIGNAL(dataReceived(QByteArray)),this,SLOT(messageReceived(QByteArray)),Qt::QueuedConnection);//
    connect(connection,SIGNAL(socketInitiliazed()),this,SLOT(initClient()),Qt::QueuedConnection);

    connect(connection,SIGNAL(serverAuthFail()),this,SLOT(sendOffAuthFail()),Qt::QueuedConnection);
    connect(connection,SIGNAL(serverAuthSuccess()),this,SLOT(sendOffAuthSuccessed()),Qt::QueuedConnection);

    connect(connection,SIGNAL(adminAuthFailed()),this,SLOT(sendOffAdminAuthFail()),Qt::QueuedConnection);
    connect(connection,SIGNAL(adminAuthSucceed()),this,SLOT(sendOffAdminAuthSuccessed()),Qt::QueuedConnection);
    connect(connection,SIGNAL(itemChanged()),this,SLOT(sendOffModelToAll()),Qt::QueuedConnection);


    connect(connection,&TcpClient::checkServerAcceptClient,this,&ServerManager::serverAcceptClient,Qt::QueuedConnection);
    connect(connection,&TcpClient::checkServerPassword,this,&ServerManager::checkAuthToServer,Qt::QueuedConnection);
    connect(connection,&TcpClient::checkAdminPassword,this,&ServerManager::checkAuthAsAdmin,Qt::QueuedConnection);
    connect(connection,&TcpClient::checkChannelPassword,this,&ServerManager::checkAuthToChannel,Qt::QueuedConnection);
    connect(connection,&TcpClient::channelPassword,this,&ServerManager::setChannelPassword,Qt::QueuedConnection);

    connect(connection,&TcpClient::socketDisconnection,this,&ServerManager::disconnected,Qt::QueuedConnection);
    connect(connection,&TcpClient::socketError,this,&ServerManager::error,Qt::QueuedConnection);
    connection->setSocketHandleId(handle);

    //emit clientAccepted();
    QMetaObject::invokeMethod(connection,"startReading",Qt::QueuedConnection);
}

void ServerManager::sendOffModelToAll()
{
    for( auto connection : m_connections.values())
    {
        sendOffModel(connection);
    }
}

void ServerManager::disconnected()
{
    emit sendLog(tr("New Incoming Connection!"), LogController::Info);
    if(!sender()) return;

    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(!client) return;

    removeClient(client);
}
void ServerManager::removeClient(TcpClient* client)
{
    client->isReady();

    auto socket = client->getSocket();

    if(nullptr != socket)
    {
        m_model->removeChild(client->getId());
        if(socket->isOpen())
        {
            socket->disconnect();
            socket->close();
        }
        m_connections.remove(socket);
        socket->deleteLater();

        sendOffModelToAll();
        client->deleteLater();
    }

}
void ServerManager::setChannelPassword(QString chanId, QByteArray passwd)
{
    auto item = m_model->getItemById(chanId);
    if(nullptr == item)
        return;

    auto channel = dynamic_cast<Channel*>(item);
    if(nullptr == channel)
        return;

    channel->setPassword(passwd);
    sendOffModelToAll();
}

void ServerManager::error(QAbstractSocket::SocketError socketError)
{
    if(QAbstractSocket::RemoteHostClosedError == socketError)
        return;
    if(!sender()) return;

    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(!client) return;

    auto socket = client->getSocket();

    if(!socket) return;

    emit sendLog(socket->errorString(), LogController::Error);

   /* switch(socketError)
    {
    case QAbstractSocket::ConnectionRefusedError:
        break;
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        break;
    case QAbstractSocket::SocketAccessError:
        break;
    case QAbstractSocket::SocketResourceError:
        break;
    case QAbstractSocket::SocketTimeoutError:
        break;
    case QAbstractSocket::DatagramTooLargeError:
        break;
    case QAbstractSocket::NetworkError:
        break;
    case QAbstractSocket::AddressInUseError:
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        break;
    case QAbstractSocket::ProxyNotFoundError:
        break;
    case QAbstractSocket::ProxyProtocolError:
        break;
    case QAbstractSocket::OperationError:
        break;
    case QAbstractSocket::SslInternalError:
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        break;
    case QAbstractSocket::TemporaryError:
        break;
    case QAbstractSocket::UnknownSocketError:
        break;

    }*/
}
