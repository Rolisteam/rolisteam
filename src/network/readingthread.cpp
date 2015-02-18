#include "readingthread.h"
#include <QTcpSocket>
ReadingThread::ReadingThread(QTcpSocket* l,QObject *parent) :
    QThread(parent),m_socket(l)
{
    m_messageQueue = new QList<Message*>;
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
    Message* msg = new Message;
    QByteArray array = m_socket->readAll();
    msg->append(array);
    m_messageQueue->append(msg);

}
