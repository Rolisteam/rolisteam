#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QTcpServer>
#include <QObject>

#include "networkmessage.h"
#include "tcpclient.h"
/**
 * @brief The ServerManager class
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
    QTcpServer * m_server;
};

#endif // SERVERMANAGER_H
