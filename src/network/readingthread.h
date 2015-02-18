#ifndef READINGTHREAD_H
#define READINGTHREAD_H
#include "connection.h"
#include <QThread>
#include "message.h"
class QTcpSocket;
class ReadingThread : public QThread
{
    Q_OBJECT
public:
    explicit ReadingThread(QTcpSocket* l,QObject *parent = 0);
    void setConnection(Connection& m);

protected:
    void run();

signals:
    //void readingFinished();
    void messageRecieved(Message&);

public slots:
    void readDataFromSocket();

private:
    QTcpSocket* m_socket;
    QList<Message*>* m_messageQueue;
    Connection m_conn;
};

#endif // READINGTHREAD_H
