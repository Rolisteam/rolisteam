#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QThread>
class Server;
class ServerManager : public QThread
{
    Q_OBJECT
public:
    explicit ServerManager(int port,QObject *parent = 0);


public slots:
    void errorOccur(QString);

protected:
    void run();



private:
    int m_port;
    Server* m_server;
};

#endif // SERVERMANAGER_H
