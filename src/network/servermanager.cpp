#include "servermanager.h"

ServerManager::ServerManager(QObject *parent) : QObject(parent)
{

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
    if (m_server == NULL)
    {
        m_server = new QTcpServer(this);
        connect(m_server, SIGNAL(newConnection()), this, SLOT(newClientConnection()));
    }
    if (m_server->listen(QHostAddress::Any, m_connectionProfile->getPort()))
    {
        emit sendLog(tr("Rolisteam Server is on!"));
//        connect(this, SIGNAL(linkDeleted(NetworkLink *)), m_playersList, SLOT(delPlayerWithLink(NetworkLink *)));
        return true;
    }
    else
    {
        emit errorOccurs(m_server->errorString());
    }
}

void ServerManager::incomingClientConnection()
{
    QTcpSocket* socketTcp = m_server->nextPendingConnection();
    //NetworkLink* netlink = new NetworkLink(socketTcp,this);

    TcpClient* client = new TcpClient(socketTcp);
    m_networkLinkList.append(client);


    connect(client,SIGNAL(dataReceived(QByteArray)),this,SLOT(sendDataToAll(QByteArray)));
    connect(socketTcp, SIGNAL(error(QAbstractSocket::SocketError)),client, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(socketTcp, SIGNAL(disconnected()), client, SIGNAL(disconnected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnection()));


}
