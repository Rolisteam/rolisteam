#include "readingthread.h"
#include <QTcpSocket>
ReadingThread::ReadingThread(QTcpSocket* l,QObject *parent) :
    QThread(parent),m_socket(l)
{

}

void ReadingThread::setConnection(Connection& m)
{
    m_conn = m;
}

void ReadingThread::run()
{
    //m_socket->connectToHost(m_conn.getAddress(),m_conn.getPort());
    exec();
}
void ReadingThread::readDataFromSocket()
{

}
