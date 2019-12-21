#ifndef REMOTELOGCONTROLLER_H
#define REMOTELOGCONTROLLER_H

#include "../task/uploadlogtoserver.h"
#include <QObject>

class RemoteLogController : public QObject
{
    Q_OBJECT
public:
    RemoteLogController();

    QString localUuid() const;
    void setLocalUuid(const QString& localUuid);

    int appId() const;
    void setAppId(int appId);

public slots:
    void addLog(const QString& msg, const QString& level, const QString& category, const QString& timestamps);
    void sendOffMessage();

private:
    std::vector<Log> m_temporyData;
    size_t m_numberPackages= 5;
    int m_appId= 0;
    QString m_localUuid;
};

#endif // REMOTELOGCONTROLLER_H
