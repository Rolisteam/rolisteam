#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QTcpServer>
#include <QObject>
#include <QThread>

#include "networkmessage.h"
#include "tcpclient.h"
#include "channelmodel.h"
#include "connectionaccepter.h"
#include "messagedispatcher.h"

/**
 * @brief The ServerManager class
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
    //void setPort(int port);

   // void create

    ServerManager::ServerState getState() const;
    void setState(const ServerManager::ServerState &state);

    void insertField(QString,QVariant, bool erase = true);
    QVariant getValue(QString) const;

    void initServerManager();

    void sendOffModel(TcpClient*);
signals:
    void stateChanged(ServerState);
    void errorOccurs(QString);
    void sendLog(QString);

public slots:
    void startListening();
    void messageReceived(QByteArray);
    void disconnection();
    void stopListening();

    void processMessageAdmin(NetworkMessageReader *msg,Channel* chan, TcpClient* tcp);
    void initClient();
    void sendOffAuthSuccessed();
    void sendOffAuthFail();
private slots:
    void incomingClientConnection();

private:
    int m_port;
    QTcpServer* m_server;
    ChannelModel* m_model;
    int m_defaultChannelIndex;
    ConnectionAccepter* m_corConnection;
    ConnectionAccepter* m_corEndProcess;

    QMap<QString,QVariant> m_parameters;

    QList< QPair<QThread*,int> > m_threadPool;
    MessageDispatcher* m_msgDispatcher;
    QList <TcpClient*> m_clients;
    ServerState m_state;

};

#endif // SERVERMANAGER_H
