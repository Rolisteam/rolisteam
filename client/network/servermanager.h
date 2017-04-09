#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QTcpServer>
#include <QObject>

#include "networkmessage.h"
#include "tcpclient.h"
#include "channelmodel.h"
/**
 * @brief The ServerManager class
 * @Todo: Add management of password
 * @Todo: Add management of time socket
 * @Todo: Add management of ip ban
 * @Todo: Add management of Range ip acception
 * @Todo: Add management of kick user
 *
 *
 */
class ServerManager : public QObject
{
    Q_OBJECT
public:
    enum ServerState {Listening,Off};
    enum Channels {Unique,Several};

    explicit ServerManager(QObject *parent = 0);

    void sendMessage(NetworkMessage* msg);

    int getPort() const;
    void setPort(int port);

    ServerManager::ServerState getState() const;
    void setState(const ServerManager::ServerState &state);

signals:
    void stateChanged(ServerState);
    void errorOccurs(QString);
    void sendLog(QString);

public slots:
    void startListening();

private slots:
    void incomingClientConnection();

private:
    int m_port;
    QTcpServer* m_server;
    ChannelModel* m_model;
    int m_defaultChannelIndex;
    ServerState m_state;
};

#endif // SERVERMANAGER_H
