#ifndef WRITINGTHREAD_H
#define WRITINGTHREAD_H

#include <QThread>
#include "message.h"
class QTcpSocket;
class WritingThread : public QThread
{
    Q_OBJECT
public:
    explicit WritingThread(QTcpSocket* l,QObject *parent = 0);
public slots:
    void addMessage(Message* m);

signals:
    void wakeUpSending();
protected:
    void run();
protected slots:
    void sendMessages();

private:
    QTcpSocket* m_socket;
    QList<Message*>* m_sendingQueue;
};

#endif // WRITINGTHREAD_H
