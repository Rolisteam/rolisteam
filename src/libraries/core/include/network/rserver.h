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
#include "serverconnection.h"
#include "serverconnectionmanager.h"

#include "updater/controller/servermanagerupdater.h"

struct NETWORK_EXPORT ThreadInfo
{
    QPointer<QThread> m_thread;
    int m_connectionCount;
};

class NETWORK_EXPORT RServer : public QTcpServer
{
    Q_OBJECT
    Q_PROPERTY(qint64 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(ServerState state READ state NOTIFY stateChanged)
    Q_PROPERTY(int threadCount READ threadCount CONSTANT)
    Q_PROPERTY(bool internal READ internal CONSTANT)
public:
    enum ServerState
    {
        Stopped,
        Listening,
        Error
    };
    Q_ENUM(ServerState)
    explicit RServer(const QMap<QString, QVariant>& parameter, bool internal, QObject* parent= nullptr);
    virtual ~RServer() override;

    virtual bool listen();
    virtual void close();
    virtual qint64 port();
    bool internal() const;

    int threadCount() const;

    RServer::ServerState state() const;

public slots:
    void setPort(int p);

protected:
    virtual void incomingConnection(qintptr descriptor) override; // qint64, qHandle, qintptr, uint
    virtual void accept(qintptr descriptor, ServerConnection* connection);

protected slots:
    void setState(const RServer::ServerState& state);
    void complete();

signals:
    void stateChanged(RServer::ServerState);
    void accepting(qintptr handle, ServerConnection* connection);
    void finished();
    void completed();
    void portChanged();
    void eventOccured(QString message, LogController::LogLevel type);

protected:
    int m_threadCount{1};
    std::unique_ptr<QThread> m_connectionThread;
    QList<ThreadInfo> m_threadPool;
    std::unique_ptr<ServerConnectionManager> m_connectionsManager;
    std::unique_ptr<ConnectionAccepter> m_corConnection;
    std::unique_ptr<ServerManagerUpdater> m_updater;
    const QMap<QString, QVariant>& m_data;
    ServerState m_state= Stopped;
    quint16 m_port{6660};
    bool m_internal{true};
};
#endif // RSERVER_H
