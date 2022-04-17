#ifndef RSERVER_H
#define RSERVER_H

#include <QDebug>
#include <QList>
#include <QObject>
#include <QPair>
#include <QTcpServer>
#include <QThread>

#include "connectionaccepter.h"
#include "network_global.h"
#include "tcpclient.h"

class NETWORK_EXPORT RServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit RServer(int threadCount, QObject* parent= nullptr);
    virtual ~RServer() override;

    virtual bool listen(const QHostAddress& address, quint16 port);
    virtual void terminate();
    virtual qint64 port();

protected:
    virtual void incomingConnection(qintptr descriptor) override; // qint64, qHandle, qintptr, uint
    virtual void accept(qintptr descriptor, TcpClient* connection);

signals:
    void accepting(qintptr handle, TcpClient* connection, QThread* thread);
    void finished();

public slots:
    void complete();

protected:
    QList<QPair<QThread*, int>> m_threadPool;
    int m_numberOfThread= 0;
    ConnectionAccepter* m_corConnection= nullptr;
};
#endif // RSERVER_H
