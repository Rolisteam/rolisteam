#include "servermanager.h"

#include "iprangeaccepter.h"
#include "ipbanaccepter.h"
#include "passwordaccepter.h"
#include "timeaccepter.h"


ServerManager::ServerManager(QObject *parent)
    : QObject(parent),  m_state(Off),m_server(nullptr)
{
    m_model = new ChannelModel();
    m_defaultChannelIndex = m_model->addChannel("default","");

    m_chainOfResponsability = new IpBanAccepter();

    IpRangeAccepter* tmp = new IpRangeAccepter();
    PasswordAccepter* tmp2 = new PasswordAccepter();
    TimeAccepter* tmp3 = new TimeAccepter();

    m_chainOfResponsability->setNext(tmp);
    tmp->setNext(tmp2);
    tmp2->setNext(tmp3);
    tmp3->setNext(nullptr);

}

int ServerManager::getPort() const
{
    return m_port;
}

void ServerManager::setPort(int port)
{
    m_port = port;
}

void ServerManager::startListening()
{
    if (m_server == nullptr)
    {
        m_server = new QTcpServer(this);
        connect(m_server, SIGNAL(newConnection()), this, SLOT(incomingClientConnection()));
    }
    if (m_server->listen(QHostAddress::Any,m_port))
    {
        setState(Listening);
        emit sendLog(tr("Rolisteam Server is on!"));
    }
    else
    {
        emit errorOccurs(m_server->errorString());
    }
}

void ServerManager::incomingClientConnection()
{
    QTcpSocket* socketTcp = m_server->nextPendingConnection();
    emit sendLog(tr("New Incoming Connection!"));

    QMap<QString,QVariant> data(m_parameters);
    data["currentIp"]=socketTcp->peerAddress().toString();

    if(m_chainOfResponsability->isValid(data))
    {
        TcpClient* client = new TcpClient(socketTcp);
        m_model->addConnectionToChannel(m_defaultChannelIndex,client);

        connect(client,SIGNAL(dataReceived(QByteArray)),this,SLOT(sendDataToAll(QByteArray)));
        connect(socketTcp, SIGNAL(error(QAbstractSocket::SocketError)),client, SLOT(connectionError(QAbstractSocket::SocketError)));
        connect(socketTcp, SIGNAL(disconnected()), client, SIGNAL(disconnected()));
        connect(client, SIGNAL(disconnected()), this, SLOT(disconnection()));
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
