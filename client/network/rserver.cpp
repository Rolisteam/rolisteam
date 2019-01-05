#include "rserver.h"
#include "servermanager.h"
#include <QDebug>
#include <QMessageLogger>

RServer::RServer(ServerManager* serverMan,int threadCount,QObject *parent)
    : QTcpServer(parent), m_numberOfThread(threadCount),m_serverManager(serverMan)
{
    connect(this,&RServer::finished,m_serverManager,&ServerManager::quit, Qt::QueuedConnection);
}

RServer::~RServer()
{
    for(auto& thread : m_threadPool)
    {
        thread.first->quit();
        thread.first->wait(1000);
    }
}

bool RServer::listen(const QHostAddress &address, quint16 port)
{
    if(!QTcpServer::listen(address,port)) return false;

    for(int i = 0; i < m_numberOfThread ; ++i)
    {
        QThread* thread = new QThread(this);
        QPair<QThread*,int> pair(thread,0);

        m_threadPool.append(pair);
        thread->start();
    }
    connect(this, &RServer::accepting,m_serverManager,&ServerManager::accept, Qt::QueuedConnection);

    return true;
}

void RServer::close()
{
    emit finished();
    QTcpServer::close();
}

qint64 RServer::port()
{
    if(isListening())
    {
        return this->serverPort();
    }
    else
    {
        return 6660;
    }
}

void RServer::incomingConnection(qintptr descriptor)
{
    TcpClient* connection = new TcpClient(nullptr,nullptr);
    accept(descriptor, connection);
}

void RServer::accept(qintptr descriptor, TcpClient* connection)
{
    bool found = false;
    int previous = 0;
    QThread* thread=nullptr;
    if(!m_threadPool.isEmpty())
    {
        for(int i = 0 ; i < m_threadPool.size() && !found ; ++i)
        {
            auto pair = m_threadPool.at(i);
            if((pair.second == 0)||(pair.second < previous))
            {
                thread = pair.first;
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
            thread = pair.first;
            pair.second++;
        }
        connection->moveToThread(thread);
        emit accepting(descriptor, connection, thread);
    }
    else
    {
        qFatal("No thread available - server is stopped") ;
    }
}

void RServer::complete()
{
        return;
}
