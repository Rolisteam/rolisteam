#ifndef ROLISTEAMDAEMON_H
#define ROLISTEAMDAEMON_H

#include <QObject>
#include <QThread>

#include "network/servermanager.h"


class RolisteamDaemon : public QObject
{
    Q_OBJECT
public:
    explicit RolisteamDaemon(QObject *parent = 0);


signals:

public slots:
    void readConfigFile(QString);
    void createEmptyConfigFile();

private slots:
    void notifyUser(QString);
    void errorMessage(QString);

private:
    int m_port;
    ServerManager m_serverManager;
    QThread m_thread;
};

#endif // ROLISTEAMDAEMON_H
