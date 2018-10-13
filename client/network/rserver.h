#ifndef RSERVER_H
#define RSERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QThread>
#include <QPair>
#include <QList>

#include "tcpclient.h"

#include "connectionaccepter.h"

class ServerManager;

class RServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit RServer(ServerManager* serverMan, int threadCount, QObject* parent = nullptr);
    virtual ~RServer();

    virtual bool listen(const QHostAddress &address, quint16 port);
    virtual void close();
    virtual qint64 port();

protected:
    virtual void incomingConnection(qintptr descriptor); //qint64, qHandle, qintptr, uint
    virtual void accept(qintptr descriptor, TcpClient* connection);

signals:
    void accepting(qintptr handle, TcpClient* connection,QThread* thread);
    void finished();

public slots:
    void complete();

protected:
   QList< QPair<QThread*,int> > m_threadPool;
   int m_numberOfThread;
   ServerManager* m_serverManager;
   ConnectionAccepter* m_corConnection;
};
#endif // RSERVER_H
