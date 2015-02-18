#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(int port,QObject *parent = 0);
    void startConnection();

signals:
    void error(QString);
private:
    int m_port;

};

#endif // SERVER_H
