#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QHash>
#include <QObject>
#include <QReadWriteLock>
#include <QTcpSocket>
#include <QThread>

// #include "networkmessage.h"
#include <common/logcontroller.h>

#include "network_global.h"
/**
 * @brief The ServerManager class
 *
 *
 */
class ChannelModel;
class Channel;
class RServer;
class ConnectionAccepter;
class MessageDispatcher;
class ServerConnection;
class NetworkMessageReader;
class NETWORK_EXPORT ServerConnectionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ChannelModel* channelModel READ channelModel CONSTANT)
public:
    enum Channels
    {
        Unique,
        Several
    };

    explicit ServerConnectionManager(const QMap<QString, QVariant>& parameters, QObject* parent= nullptr);
    ~ServerConnectionManager();

    int countConnection() const;
    ChannelModel* channelModel() const;
    const QHash<QTcpSocket*, ServerConnection*> connections() const;

signals:

    void eventOccured(QString, LogController::LogLevel);
    void messageMustBeDispatched(QByteArray array, Channel* channel, ServerConnection* client);
    void portChanged();
    void clientAccepted();
    void quitting();
    void finished();

public slots:
    // controller
    void quit();
    void accept(qintptr handle, ServerConnection* connection);

    // network
    void messageReceived(QByteArray);
    void initClient();

    void sendOffAdminAuthFail();
    void sendOffAdminAuthSuccessed();

    void serverAcceptClient(ServerConnection* client);
    void checkAuthToServer(ServerConnection* client);
    void checkAuthToChannel(ServerConnection* client, QString channelId, QByteArray password);
    void checkAuthAsAdmin(ServerConnection* client);

    // Accessors
    void setChannelPassword(QString chanId, QByteArray passwd);

protected:
    void removeSocket(QTcpSocket* socket);
    void kickClient(QString id, bool isAdmin, QString senderId);
    void banClient(QString id, bool isAdmin, QString senderId);

protected slots:
    // admin

    void processMessageAdmin(NetworkMessageReader* msg, Channel* chan, ServerConnection* tcp);
    void sendOffAuthSuccessed();
    void sendOffAuthFail();
    void disconnectedUser();
    void error(QAbstractSocket::SocketError socketError);
    // memory
    void memoryChannelChanged(quint64);

private:
    std::unique_ptr<ChannelModel> m_model;
    std::unique_ptr<ConnectionAccepter> m_corEndProcess;
    std::unique_ptr<ConnectionAccepter> m_tcpConnectionAccepter;
    std::unique_ptr<ConnectionAccepter> m_adminAccepter;
    std::unique_ptr<ConnectionAccepter> m_enterInRoomAccepter;

    MessageDispatcher* m_msgDispatcher= nullptr;
    QHash<QTcpSocket*, ServerConnection*> m_connections;
    const QMap<QString, QVariant>& m_parameters;

    mutable QReadWriteLock m_lock;
};

#endif // SERVERMANAGER_H
