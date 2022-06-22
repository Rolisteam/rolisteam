#ifndef REMOTELOGCONTROLLER_H
#define REMOTELOGCONTROLLER_H

#include "common_global.h"
#include "common_types.h"
#include <QObject>

class COMMON_EXPORT RemoteLogController : public QObject
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
    std::vector<common::Log> m_temporyData;
    size_t m_numberPackages= 5;
    int m_appId= 0;
    QString m_localUuid;
};

#endif // REMOTELOGCONTROLLER_H
