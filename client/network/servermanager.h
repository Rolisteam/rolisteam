#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H


#include <QObject>
#include <QThread>

#include "networkmessage.h"
#include "tcpclient.h"
#include "channelmodel.h"
#include "connectionaccepter.h"
#include "messagedispatcher.h"
#include "rserver.h"
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

    ServerManager::ServerState getState() const;
    void setState(const ServerManager::ServerState &state);

    void insertField(QString,QVariant, bool erase = true);


    void initServerManager();

    QVariant getValue(QString key) const;


    void kickClient(QString id);
signals:
    void stateChanged(ServerManager::ServerState);
    void errorOccurs(QString);
    void sendLog(QString);
    void messageMustBeDispatched(QByteArray array, Channel* channel,TcpClient* client);
    void finished();
    void listening();

public slots:
    void startListening();
    void messageReceived(QByteArray);
    void disconnection();
    void stopListening();

    void processMessageAdmin(NetworkMessageReader *msg,Channel* chan, TcpClient* tcp);
    void initClient();
    void sendOffAuthSuccessed();
    void sendOffAuthFail();
    void start();
    void quit();
    void accept(qintptr handle, TcpClient* connection, QThread* thread);
    void removeSocket(QTcpSocket *socket);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void sendOffModel(TcpClient*);
    void sendOffModelToAll();
private slots:
    void incomingClientConnection();

private:
    int m_port;
    RServer* m_server;
    ChannelModel* m_model;
    int m_defaultChannelIndex;
    ConnectionAccepter* m_corEndProcess;
    ConnectionAccepter* m_corConnection;

    QMap<QString,QVariant> m_parameters;


    MessageDispatcher* m_msgDispatcher;
    QHash<QTcpSocket*,TcpClient*> m_connections;
    ServerState m_state;

};

#endif // SERVERMANAGER_H
