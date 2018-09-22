#include "logsenderscheduler.h"
#include "../task/uploadlogtoserver.h"
#include <QThread>
#include <QCoreApplication>

LogSenderScheduler::LogSenderScheduler()
{

}
void LogSenderScheduler::addLog(QString msg, LogController::LogLevel level, QString category, QString timestamp)
{
    Log tm = {category, msg, timestamp, LogController::typeToText(level)};
    m_temporyData.push_back(tm);
    if(m_temporyData.size() == m_numberPackages)
        sendOffMessage();
}

void LogSenderScheduler::sendOffMessage()
{
    QThread* thread = new QThread();
    LogUploader* uploader = new LogUploader();
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

int LogSenderScheduler::appId() const
{
    return m_appId;
}

void LogSenderScheduler::setAppId(int appId)
{
    m_appId = appId;
}

QString LogSenderScheduler::localUuid() const
{
    return m_localUuid;
}

void LogSenderScheduler::setLocalUuid(const QString &localUuid)
{
    m_localUuid = localUuid;
}
