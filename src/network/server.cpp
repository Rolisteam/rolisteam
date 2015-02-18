#include "server.h"

Server::Server(int port,QObject *parent) :
    QTcpServer(parent),m_port(port)
{

}
void Server::startConnection()
{
    if(!listen(QHostAddress::Any,m_port))
    {
        emit error(tr("Enable to start the server"));
    }
}
