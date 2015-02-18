#include "rclient.h"
#include <QHostAddress>
#include <QDebug>

RClient::RClient(Connection& connectpar,QObject *parent) :
    QObject(parent),m_connection(connectpar)
{
    m_client = new QTcpSocket(this);
    m_currentState =DISCONNECTED;
    connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorOccurs()));
    connect(m_client,SIGNAL(connected()),this,SLOT(isConnected()));
    connect(m_client,SIGNAL(readyRead()),this,SLOT(readData()));
}
RClient::~RClient()
{

}
void RClient::errorOccurs()
{
    m_currentState=ERROR;
    qDebug() << m_client->errorString();
}

void RClient::startConnection()
{
    QHostAddress addr(m_connection.getAddress());
    qDebug() << m_connection.getAddress() << m_connection.getPort();
    m_client->connectToHost(addr,m_connection.getPort());
}
void RClient::isConnected()
{
    m_currentState=CONNECTED;
    qDebug() << "connected established";
}
void RClient::readData()
{
    m_currentState=CONNECTED;
    qDebug() << "ready to read";
}
