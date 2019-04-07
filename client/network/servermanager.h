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
class ServerManager : public QObject
{
    Q_OBJECT
public:
    enum ServerState
    {
        Listening,
        Off
    };
    enum Channels
    {
        Unique,
        Several
    };

    explicit ServerManager(QObject* parent= nullptr);
    ~ServerManager();
    void sendMessage(NetworkMessage* msg);
    int getPort() const;
    ServerManager::ServerState getState() const;
    void setState(const ServerManager::ServerState& state);
    void insertField(QString, QVariant, bool erase= true);
    void initServerManager();
    QVariant getValue(QString key) const;

    void setChannelPassword(QString chanId, QByteArray passwd);
signals:
    void stateChanged(ServerManager::ServerState);
    void sendLog(QString, LogController::LogLevel);
    void messageMustBeDispatched(QByteArray array, Channel* channel, TcpClient* client);
    void finished();
    void listening();
    void clientAccepted();

public slots:
    void startListening();
    void stopListening();
    void messageReceived(QByteArray);
    void processMessageAdmin(NetworkMessageReader* msg, Channel* chan, TcpClient* tcp);
    void initClient();
    void sendOffAuthSuccessed();
    void sendOffAuthFail();
    void quit();
    void accept(qintptr handle, TcpClient* connection, QThread* thread);
    void removeClient(TcpClient* socket);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void sendOffModel(TcpClient*);
    void sendOffModelToAll();
    void sendOffAdminAuthFail();
    void sendOffAdminAuthSuccessed();

    // Connection proccess tests
    void serverAcceptClient(TcpClient* client);
    void checkAuthToServer(TcpClient* client);
    void checkAuthToChannel(TcpClient* client, QString channelId, QByteArray password);
    void checkAuthAsAdmin(TcpClient* client);

    // memory
    void memoryChannelChanged(quint64);

protected:
    void sendEventToClient(TcpClient* client, TcpClient::ConnectionEvent event);
    void kickClient(QString id, bool isAdmin, QString senderId);
    void banClient(QString id, bool isAdmin, QString senderId);

private:
    int m_port;
    RServer* m_server= nullptr;
    std::unique_ptr<ChannelModel> m_model;
    ConnectionAccepter* m_corEndProcess= nullptr;
    ConnectionAccepter* m_corConnection= nullptr;
    ConnectionAccepter* m_adminAccepter= nullptr;
    ConnectionAccepter* m_enterInRoomAccepter= nullptr;

    QMap<QString, QVariant> m_parameters;

    MessageDispatcher* m_msgDispatcher= nullptr;
    QHash<QTcpSocket*, TcpClient*> m_connections;
    ServerState m_state= Off;
    int m_tryCount= 0;
};

#endif // SERVERMANAGER_H
