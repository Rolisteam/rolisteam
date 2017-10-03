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
        m_server = new RServer(this,getValue("ThreadCount").toInt());
        //connect(m_server, SIGNAL(newConnection()), this, SLOT(incomingClientConnection()));

    }
    if (m_server->listen(QHostAddress::Any,getValue("port").toInt()))
    {
        setState(Listening);
        emit sendLog(tr("Rolisteam Server is on!"));
    }
    else
    {
        emit errorOccurs(m_server->errorString());
        QTimer::singleShot(getValue("TimeToRetry").toInt(),this,SLOT(startListening()));
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

void ServerManager::disconnection()
{

    qInfo() << "Disconnction";
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

void ServerManager::incomingClientConnection()
{
   //empty
}
void ServerManager::initClient()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    QMap<QString,QVariant> data(m_parameters);
    data["currentIp"]=client->getSocket()->peerAddress().toString();
    if(nullptr != client)
    {
        client->sendEvent(TcpClient::HasCheckEvent);

        if(m_corEndProcess->isValid(data))
        {
            m_model->addConnectionToDefaultChannel(client);
            client->sendEvent(TcpClient::AuthSuccessEvent);
            sendOffModel(client);
        }
        else
        {
            client->sendEvent(TcpClient::CheckedEvent);
        }
    }
}
void ServerManager::sendOffAdminAuthSuccessed()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::AdminAuthSucessed);
        qDebug()<< "[sendOffAdminAuthSuccessed] ";
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
        //client->sendMessage(msg);
        qDebug()<< "[sendOffAdminAuthFail] ";
        QMetaObject::invokeMethod(client,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,static_cast<NetworkMessage*>(msg)),Q_ARG(bool,true));
    }
}
void ServerManager::sendOffAuthSuccessed()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::AuthentificationSucessed);
        //qDebug()<< "[sendOffAuthSuccessed] ";
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
        //client->sendMessage(msg);
        //qDebug()<< "[sendOffAuthFail] ";
        QMetaObject::invokeMethod(client,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,static_cast<NetworkMessage*>(msg)),Q_ARG(bool,true));
    }
}
void ServerManager::kickClient(QString id)
{
    m_model->kick(id);
    sendOffModelToAll();

    QTcpSocket* client = nullptr;
    for(auto key : m_connections.keys())
    {
        if(!key->isOpen())
        {
            //qDebug() << "isClose";
        }
        auto value = m_connections[key];
        if(value->getId() == id)
        {
            client = key;
        }
    }
    if(nullptr != client)
    {
        m_connections.remove(client);
    }
}

void ServerManager::processMessageAdmin(NetworkMessageReader* msg,Channel* chan, TcpClient* tcp)
{
    bool idAdmin = tcp->isAdmin();
    switch (msg->action())
    {
        case NetMsg::Goodbye:

        break;
        case NetMsg::Kicked:
        {
            if(idAdmin)
            {
                QString id = msg->string8();
                kickClient(id);
            }
        }
        break;
        case NetMsg::Password:
        {
            QMap<QString,QVariant> data(m_parameters);
            data["userpassword"]=msg->string32();
            bool hasChannelData = static_cast<bool>(msg->uint8());
            if(m_corEndProcess->isValid(data))
            {
                tcp->sendEvent(TcpClient::AuthSuccessEvent);
                if(hasChannelData)
                {
                    QString chanId=msg->string32();
                    m_model->addConnectionToChannel(chanId,tcp);
                }
                else
                {
                    m_model->addConnectionToDefaultChannel(tcp);
                }
            }
            else
            {
                tcp->sendEvent(TcpClient::AuthFailEvent);
            }
        }
        break;
        case NetMsg::MoveChannel:

        break;
        case NetMsg::AddChannel:
        {
            if(idAdmin)
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
            }
        }
        break;
        case NetMsg::SetChannelList:
        {
        if(idAdmin)
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
        if(idAdmin)
        {
            QString id = msg->string8();
            m_model->removeChild(id);
            sendOffModelToAll();
        }
    }
        break;
    case NetMsg::AdminPassword:
    {
        QString passwd = msg->string32();
        QMap<QString,QVariant> data(m_parameters);
        data["userpassword"]=passwd;
        if(m_adminAccepter->isValid(data))
        {
            tcp->sendEvent(TcpClient::AdminAuthSuccess);
        }
        else
        {
            tcp->sendEvent(TcpClient::AdminAuthFailed);
        }
    }
        break;
        default:
            break;
    }
}

void ServerManager::sendOffModel(TcpClient* client)
{
    if(nullptr != client)
    {
        //qDebug() << "[current qthread]" << QThread::currentThread() << " [client thread:]"<<client->thread();
        NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::SetChannelList);
        QJsonDocument doc;
        QJsonObject obj;
        m_model->writeDataJson(obj);
        doc.setObject(obj);

        msg->byteArray32(doc.toJson());
        //qDebug()<< "[sendOffModel] ";
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
void ServerManager::start()
{
    qDebug() << this << "connections started on" << QThread::currentThread();
}

void ServerManager::quit()
{
    if(!sender()) return;
    //qDebug() << this << "connections quitting";

  /*  foreach(QTcpSocket *socket, m_connections.keys())
    {
        qDebug() << this << "closing socket" << socket;
        removeSocket(socket);
    }*/

    //qDebug() << this << "finishing";
    emit finished();
}

void ServerManager::accept(qintptr handle, TcpClient *connection,QThread* thread)
{
    //qDebug() << "*** HEY WATCH THIS";
    QTcpSocket* socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(handle))
    {
        qWarning() << this << "could not accept connection" << handle;
        connection->deleteLater();
        return;
    }

    emit sendLog(tr("New Incoming Connection!"));

    QMap<QString,QVariant> data(m_parameters);
    data["currentIp"]=socket->peerAddress().toString();
    //qInfo() << "currentIP" << data["currentIp"].toString();

    if(m_corConnection->isValid(data))
    {
        connect(connection,SIGNAL(dataReceived(QByteArray)),this,SLOT(messageReceived(QByteArray)));//,Qt::QueuedConnection
        connect(connection,SIGNAL(isReady()),this,SLOT(initClient()),Qt::QueuedConnection);
        connect(connection,SIGNAL(authSuccess()),this,SLOT(sendOffAuthSuccessed()),Qt::QueuedConnection);
        connect(connection,SIGNAL(adminAuthFailed()),this,SLOT(sendOffAdminAuthFail()),Qt::QueuedConnection);
        connect(connection,SIGNAL(adminAuthSucceed()),this,SLOT(sendOffAdminAuthSuccessed()),Qt::QueuedConnection);
        connect(connection,SIGNAL(authFail()),this,SLOT(sendOffAuthFail()),Qt::QueuedConnection);
        connect(connection,SIGNAL(itemChanged()),this,SLOT(sendOffModelToAll()),Qt::QueuedConnection);

        connect(socket,&QTcpSocket::disconnected, this, &ServerManager::disconnected,Qt::QueuedConnection);
        connect(socket,static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),this,&ServerManager::error,Qt::QueuedConnection);

        connection->moveToThread(thread);
        connection->setSocket(socket);

        m_connections.insert(socket,connection);
        emit socket->connected();
    }

}
void ServerManager::sendOffModelToAll()
{
   // qDebug() << "sendoffmodeltoALL";
    int i = 0;
    for( auto connection : m_connections.values())
    {

     //   qDebug() << "for loop connection: loop:" << ++i << connection->getName();
        sendOffModel(connection);
    }
}

void ServerManager::disconnected()
{
    if(!sender()) return;
    //qDebug() << this << "disconnecting socket"<< sender();

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    removeSocket(socket);
}
void ServerManager::removeSocket(QTcpSocket *socket)
{
    if(!socket) return;
    if(!m_connections.contains(socket)) return;

    TcpClient* client = m_connections.value(socket);

    //qDebug() << this << "removing socket = " <<  socket;
    m_model->removeChild(client->getId());
    if(socket->isOpen())
    {
       // qDebug() << this << "socket is open, attempting to close it " << socket;
        socket->disconnect();
        socket->close();
    }

    //qDebug() << this << "deleting socket" << socket;
    m_connections.remove(socket);
    socket->deleteLater();

    //qDebug() << this << "client count = " << m_connections.count();
    sendOffModelToAll();

}
void ServerManager::error(QAbstractSocket::SocketError socketError)
{
    if(!sender()) return;
    //qDebug() << this << "error in socket" << sender() << " error = " << socketError;

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    removeSocket(socket);
}
