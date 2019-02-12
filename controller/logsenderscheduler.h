#ifndef LOGSENDERSCHEDULER_H
#define LOGSENDERSCHEDULER_H

#include "../task/uploadlogtoserver.h"
#include "logcontroller.h"
#include <QObject>

class LogSenderScheduler : public QObject
{
    Q_OBJECT
public:
    LogSenderScheduler();

    QString localUuid() const;
    void setLocalUuid(const QString& localUuid);

    int appId() const;
    void setAppId(int appId);

public slots:
    void addLog(QString msg, LogController::LogLevel level, QString category, QString timestamps);
    void sendOffMessage();

private:
    std::vector<Log> m_temporyData;
    size_t m_numberPackages= 5;
    int m_appId= 0;
    QString m_localUuid;
};

#endif // LOGSENDERSCHEDULER_H
