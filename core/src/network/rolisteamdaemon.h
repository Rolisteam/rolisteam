#ifndef ROLISTEAMDAEMON_H
#define ROLISTEAMDAEMON_H

#include <QObject>
#include <QThread>

#include "common/controller/logcontroller.h"
#include "servermanager.h"
/**
 * @brief The RolisteamDaemon class
 */
class RolisteamDaemon : public QObject
{
    Q_OBJECT
public:
    explicit RolisteamDaemon(QObject* parent= nullptr);

    void start();
signals:
    void stopped();

public slots:
    bool readConfigFile(QString);
    void createEmptyConfigFile(QString filepath);

private:
    ServerManager m_serverManager;
    LogController* m_logController= nullptr;
    QThread m_thread;
};

#endif // ROLISTEAMDAEMON_H
