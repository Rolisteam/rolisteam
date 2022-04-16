#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QThread>

#include "channelmodel.h"
#include "common/controller/logcontroller.h"
#include "connectionaccepter.h"
#include "messagedispatcher.h"
#include "networkmessage.h"
#include "rserver.h"
#include "tcpclient.h"
/**
 * @brief The ServerManager class
 *
 *
 */
class ServerManagerUpdater;
class ServerManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ServerState state READ state NOTIFY stateChanged)
    Q_PROPERTY(ChannelModel* channelModel READ channelModel CONSTANT)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)

public:
    enum ServerState
    {
        Stopped,
        Listening,
        Error
    };
    Q_ENUM(ServerState)
    enum Channels
    {
        Unique,
        Several
    };

    explicit ServerManager(QObject* parent= nullptr);
    ~ServerManager();

    int port() const;
    ServerManager::ServerState state() const;
    QVariant getValue(QString key) const;
    ChannelModel* channelModel() const;
    const QHash<QTcpSocket*, TcpClient*> connections() const;

    void insertField(QString, QVariant, bool erase= true);
    void initServerManager();

signals:
    void stateChanged(ServerManager::ServerState);
    void eventOccured(QString, LogController::LogLevel);
    void messageMustBeDispatched(QByteArray array, Channel* channel, TcpClient* client);
    void portChanged();

    // multithreading socket
    void clientAccepted();

public slots:
    // controller
    bool startListening();
    void stopListening();
    void quit();

    // network
    void messageReceived(QByteArray);
    void initClient();

    // admin
    void setState(const ServerManager::ServerState& state);
    void processMessageAdmin(NetworkMessageReader* msg, Channel* chan, TcpClient* tcp);
    void sendOffAuthSuccessed();
    void sendOffAuthFail();
    void accept(qintptr handle, TcpClient* connection, QThread* thread);
    void removeClient(TcpClient* socket);
    void disconnectedUser();
    void error(QAbstractSocket::SocketError socketError);
    // void sendOffModel(TcpClient*);
    // void sendOffModelToAll();
    void sendOffAdminAuthFail();
    void sendOffAdminAuthSuccessed();

    // Connection proccess tests
    void serverAcceptClient(TcpClient* client);
    void checkAuthToServer(TcpClient* client);
    void checkAuthToChannel(TcpClient* client, QString channelId, QByteArray password);
    void checkAuthAsAdmin(TcpClient* client);

    // memory
    void memoryChannelChanged(quint64);

    // Accessors
    void setPort(int p);
    // void setTryCount(int tryCount);
    void setChannelPassword(QString chanId, QByteArray passwd);
    // void sendMessage(NetworkMessage* msg);

protected:
    void sendEventToClient(TcpClient* client, TcpClient::ConnectionEvent event);
    void kickClient(QString id, bool isAdmin, QString senderId);
    void banClient(QString id, bool isAdmin, QString senderId);
    void closeAllConnections();

private:
    std::unique_ptr<RServer> m_server;
    std::unique_ptr<ChannelModel> m_model;
    std::unique_ptr<ServerManagerUpdater> m_serverUpdater;
    std::unique_ptr<ConnectionAccepter> m_corEndProcess;
    std::unique_ptr<ConnectionAccepter> m_corConnection;
    std::unique_ptr<ConnectionAccepter> m_adminAccepter;
    std::unique_ptr<ConnectionAccepter> m_enterInRoomAccepter;

    QMap<QString, QVariant> m_parameters;
    int m_port;

    MessageDispatcher* m_msgDispatcher= nullptr;
    QHash<QTcpSocket*, TcpClient*> m_connections;
    ServerState m_state= Stopped;
};

#endif // SERVERMANAGER_H
