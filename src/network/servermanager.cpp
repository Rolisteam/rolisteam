#include "servermanager.h"
#include "server.h"
ServerManager::ServerManager(int port,QObject *parent) :
    QThread(parent),m_port(port)
{
    m_server =new Server(m_port);
    connect(m_server,SIGNAL(error(QString)),this,SLOT(errorOccur(QString)));
    m_server->moveToThread(this);

}


void ServerManager::run()
{
    m_server->startConnection();
    exec();
}
void ServerManager::errorOccur(QString msgerror)
{
    QTextStream out(stdout,QIODevice::WriteOnly);
    out << msgerror << m_server->errorString();
}
