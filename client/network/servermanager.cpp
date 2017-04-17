#include "servermanager.h"

#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>

#include "iprangeaccepter.h"
#include "ipbanaccepter.h"
#include "passwordaccepter.h"
#include "timeaccepter.h"
#include "network/networkmessagewriter.h"

ServerManager::ServerManager(QObject *parent)
    : QObject(parent),  m_state(Off),m_server(nullptr)
{
    m_model = new ChannelModel();
    m_msgDispatcher = new MessageDispatcher(this);
    connect(m_msgDispatcher, SIGNAL(messageForAdmin(NetworkMessageReader*,Channel*,TcpClient*)),this,SLOT(processMessageAdmin(NetworkMessageReader*,Channel*,TcpClient*)));
    m_defaultChannelIndex = m_model->addChannel("default","");


    m_corConnection = new IpBanAccepter();

    IpRangeAccepter* tmp = new IpRangeAccepter();
    TimeAccepter* tmp3 = new TimeAccepter();

    PasswordAccepter* tmp2 = new PasswordAccepter();

    m_corEndProcess = tmp2;

    m_corConnection->setNext(tmp);
    tmp->setNext(tmp3);
    //tmp2->setNext(tmp3);
    tmp3->setNext(nullptr);
    tmp2->setNext(nullptr);

}

int ServerManager::getPort() const
{
    return m_port;
}
void ServerManager::startListening()
{
    if (m_server == nullptr)
    {
        m_server = new QTcpServer(this);
        connect(m_server, SIGNAL(newConnection()), this, SLOT(incomingClientConnection()));

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
    //qInfo() << "Message Received" << array.size();
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        //qInfo() << "Client" ;
        Channel* channel = client->getParentChannel();
        //if(nullptr != channel)
        {
            //qInfo() << "channel" ;
            //channel->sendToAll(,client);
            m_msgDispatcher->dispatchMessage(array,channel,client);
        }
    }
}

void ServerManager::disconnection()
{

    qInfo() << "Disconnction";
}
void ServerManager::initServerManager()
{
   //create thread
   int thCount = getValue("ThreadCount").toInt();
   for(int i = 0; i < thCount ; ++i)
   {
       QPair<QThread*,int> pair(new QThread(),0);
       m_threadPool.append(pair);
       pair.first->start();
   }


   //create channel
   int chCount = getValue("ChannelCount").toInt();
   int count = m_model->rowCount(QModelIndex());


   for(int i = count; i < thCount ; ++i)
   {
       m_model->addChannel(QStringLiteral("Channel %1").arg(chCount),"");
   }
   qDebug() << m_model->rowCount(QModelIndex()) << thCount;

}

void ServerManager::incomingClientConnection()
{
    QTcpSocket* socketTcp = m_server->nextPendingConnection();
    emit sendLog(tr("New Incoming Connection!"));


    QMap<QString,QVariant> data(m_parameters);
    data["currentIp"]=socketTcp->peerAddress().toString();
    qInfo() << "currentIP" << data["currentIp"].toString();
    if(m_corConnection->isValid(data))
    {
        qInfo() << "inside connection valide";
        TcpClient* client = new TcpClient(socketTcp);
        connect(client,SIGNAL(isReady()),this,SLOT(initClient()));
        connect(client,SIGNAL(authSuccess()),this,SLOT(sendOffAuthSuccessed()));
        connect(client,SIGNAL(authFail()),this,SLOT(sendOffAuthFail()));
        m_clients.append(client);

    }
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

        bool found = false;
        int previous = 0;
        for(int i = 0 ; i < m_threadPool.size() && !found ; ++i)
        {
            auto pair = m_threadPool.at(i);
            if((pair.second == 0)||(pair.second < previous))
            {
                client->moveToThread(pair.first);
                found = true;
                pair.second++;
            }
            else
            {
                previous = pair.second;
            }
        }
        if(!found)
        {
            auto pair = m_threadPool.first();
            client->moveToThread(pair.first);
            pair.second++;
        }

        qInfo() << "set connection signal/slots: found:" << found;
        connect(client,SIGNAL(dataReceived(QByteArray)),this,SLOT(messageReceived(QByteArray)));
        connect(client, SIGNAL(disconnected()), this, SLOT(disconnection()));
    }
}
void ServerManager::sendOffAuthSuccessed()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::AuthentificationSucessed);
        client->sendMessage(msg);

        sendOffModel(client);
    }
}
void ServerManager::sendOffAuthFail()
{
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::AuthentificationFail);
        client->sendMessage(msg);
    }
}
void ServerManager::processMessageAdmin(NetworkMessageReader* msg,Channel* chan, TcpClient* tcp)
{
    switch (msg->action())
    {
        case NetMsg::Goodbye:

        break;
        case NetMsg::Kicked:

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
        case NetMsg::SetChannelList:
        {
            QByteArray data = msg->byteArray32();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if(!doc.isEmpty())
            {
                QJsonObject obj = doc.object();

                m_model->readDataJson(obj);
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
        NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::SetChannelList);
        QJsonDocument doc;
        QJsonObject obj;
        m_model->writeDataJson(obj);
        doc.setObject(obj);

        msg.byteArray32(doc.toJson());
        qDebug() << doc.toJson();
        client->sendMessage(msg);
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
}
