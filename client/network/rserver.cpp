#include "rserver.h"
#include "servermanager.h"

RServer::RServer(ServerManager* serverMan,int threadCount,QObject *parent)
    : QTcpServer(parent), m_numberOfThread(threadCount),m_serverManager(serverMan)
{
    qDebug() <<  this << "created on" << QThread::currentThread();
}

RServer::~RServer()
{
    qDebug() <<  this << "destroyed";
}

bool RServer::listen(const QHostAddress &address, quint16 port)
{
    if(!QTcpServer::listen(address,port)) return false;

    for(int i = 0; i < m_numberOfThread ; ++i)
    {
        QThread* thread = new QThread();
        QPair<QThread*,int> pair(thread,0);

        connect(thread,&QThread::started,m_serverManager,&ServerManager::start, Qt::QueuedConnection);
        connect(this,&RServer::finished,m_serverManager,&ServerManager::quit, Qt::QueuedConnection);

        m_threadPool.append(pair);
        thread->start();
    }
    connect(this, &RServer::accepting,m_serverManager,&ServerManager::accept, Qt::QueuedConnection);

    return true;
}

void RServer::close()
{
    qDebug() << this << "closing server";
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
    qDebug() << this << "attempting to accept connection" << descriptor;
    TcpClient* connection = new TcpClient(nullptr);
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
        qDebug() << this << "accepting the connection" << descriptor;
        connection->moveToThread(thread);
        emit accepting(descriptor, connection, thread);
    }
    /// @todo error no thread!

}

void RServer::complete()
{
   // if(!m_thread)
    {
        qWarning() << this << "exiting complete there was no thread!";
        return;
    }

   qDebug() << this << "Complete called, destroying thread";
//    delete m_connections;

    qDebug() << this << "Quitting thread";
   // m_thread->quit();
  //  m_thread->wait();

 //   delete m_thread;

    qDebug() << this << "complete";

}
