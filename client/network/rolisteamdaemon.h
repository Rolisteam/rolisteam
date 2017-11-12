#ifndef ROLISTEAMDAEMON_H
#define ROLISTEAMDAEMON_H

#include <QObject>
#include <QThread>

#include "servermanager.h"
/**
 * @brief The RolisteamDaemon class
 */
class RolisteamDaemon : public QObject
{
    Q_OBJECT
public:
    explicit RolisteamDaemon(QObject *parent = 0);


    void start();
signals:

public slots:
    void readConfigFile(QString);
    void createEmptyConfigFile(QString filepath);

    int getLevelOfLog();

private slots:
    void notifyUser(QString);
    void errorMessage(QString);

private:
    ServerManager m_serverManager;
    QThread m_thread;
};

#endif // ROLISTEAMDAEMON_H
