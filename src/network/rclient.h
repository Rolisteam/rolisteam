#ifndef RCLIENT_H
#define RCLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include "connection.h"

class RClient : public QObject
{
    Q_OBJECT
public:
    enum State{DISCONNECTED,CONNECTED,ERROR};
    explicit RClient(Connection& connect,QObject *parent = 0);
    ~RClient();

    void startConnection();
protected slots:
    void errorOccurs();
    void isConnected();
    void readData();
private:
    Connection m_connection;
    QTcpSocket* m_client;
    State m_currentState;
};

#endif // RCLIENT_H
