#include "remotelogcontroller.h"
#include "../task/uploadlogtoserver.h"
#include <QCoreApplication>
#include <QThread>

RemoteLogController::RemoteLogController() {}
void RemoteLogController::addLog(const QString& msg, const QString& level, const QString& category,
                                 const QString& timestamp)
{
    Log tm= {category, msg, timestamp, level};
    m_temporyData.push_back(tm);
    if(m_temporyData.size() == m_numberPackages)
        sendOffMessage();
}

void RemoteLogController::sendOffMessage()
{
    QThread* thread= new QThread();
    LogUploader* uploader= new LogUploader();
    uploader->setAppId(m_appId);
    uploader->setLogs(m_temporyData);
    uploader->setUuid(m_localUuid);
    uploader->setVersion(QCoreApplication::applicationVersion());

    uploader->moveToThread(thread);

    connect(uploader, &LogUploader::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, uploader, &LogUploader::deleteLater);
    connect(thread, &QThread::started, uploader, &LogUploader::uploadLog);
    connect(uploader, &LogUploader::destroyed, thread, &QThread::deleteLater);

    thread->start();

    m_temporyData.clear();
}

int RemoteLogController::appId() const
{
    return m_appId;
}

void RemoteLogController::setAppId(int appId)
{
    m_appId= appId;
}

QString RemoteLogController::localUuid() const
{
    return m_localUuid;
}

void RemoteLogController::setLocalUuid(const QString& localUuid)
{
    m_localUuid= localUuid;
}
