#include "servermanager.h"

#include "iprangeaccepter.h"
#include "ipbanaccepter.h"
#include "passwordaccepter.h"
#include "timeaccepter.h"

#include <QTimer>
ServerManager::ServerManager(QObject *parent)
    : QObject(parent),  m_state(Off),m_server(nullptr)
{
    m_model = new ChannelModel();
    m_msgDispatcher = new MessageDispatcher(this);
    connect(m_msgDispatcher, SIGNAL(messageForAdmin(NetworkMessageReader*)),this,SLOT(processMessageAdmin(NetworkMessageReader*)));
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

void ServerManager::messageReceived(QByteArray array)
{
    qInfo() << "Message Received" << array.size();
    TcpClient* client = qobject_cast<TcpClient*>(sender());
    if(nullptr != client)
    {
        qInfo() << "Client" ;
        Channel* channel = client->getParentChannel();
        if(nullptr != channel)
        {
            qInfo() << "channel" ;
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
   //m_model->
   for(int i = 0; i < thCount ; ++i)
   {
       m_model->addChannel(QStringLiteral("Channel %1").arg(chCount),"");
   }
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
        m_model->addConnectionToChannel(m_defaultChannelIndex,client);
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
        connect(socketTcp, SIGNAL(error(QAbstractSocket::SocketError)),client, SLOT(connectionError(QAbstractSocket::SocketError)));
        connect(socketTcp, SIGNAL(disconnected()), client, SIGNAL(disconnected()));
        connect(client, SIGNAL(disconnected()), this, SLOT(disconnection()));
    }
}
void ServerManager::processMessageAdmin(NetworkMessageReader* msg)
{
    switch (msg->action())
    {
        case NetMsg::Goodbye:

        break;
        case NetMsg::Kicked:

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

void ServerManager::sendOffModel()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::SetChannelList);
    QJsonDocument doc;
    QJsonObject obj;
    m_model->writeDataJson(obj);
    doc.setObject(obj);

    msg.byteArray32(doc.toJson());
    msg.sendAll();
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
