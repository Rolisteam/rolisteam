#include "writingthread.h"
#include <QTcpSocket>
WritingThread::WritingThread(QTcpSocket* l,QObject *parent) :
    QThread(parent),m_socket(l)
{
    m_sendingQueue = new QList<Message*>;
    connect(this,SIGNAL(wakeUpSending()),this,SLOT(sendMessages()));
}
void WritingThread::sendMessages()
{
    foreach(Message* tmp,*m_sendingQueue)
    {
        m_socket->write(*tmp);
    }
}
void WritingThread::addMessage(Message* m)
{
    m_sendingQueue->append(m);
}
void WritingThread::run()
{
    exec();
}
