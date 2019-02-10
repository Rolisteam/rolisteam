#include "rolisteamdaemon.h"
#include <QDebug>
#include <QFile>
#include <QTime>

#include <QFileInfo>
#include <QSettings>

RolisteamDaemon::RolisteamDaemon(QObject* parent) : QObject(parent), m_logController(new LogController(true, this)) {}

bool RolisteamDaemon::readConfigFile(QString filepath)
{
    if(!QFileInfo::exists(filepath))
        return false;

    QSettings settings(filepath, QSettings::IniFormat);

    int port= settings.value("port").toInt();
    auto password= QByteArray::fromBase64(settings.value("ServerPassword").toByteArray());
    QString range= settings.value("IpRange").toString();
    QString ipBan= settings.value("IpBan").toString();
    QString connectionMax= settings.value("ConnectionMax").toString();
    QString timeStart= settings.value("TimeStart").toString();
    QString timeEnd= settings.value("TimeEnd").toString();
    QString ipMode= settings.value("IpMode").toString();
    auto adminPassword= QByteArray::fromBase64(settings.value("AdminPassword").toByteArray());
    int threadCount= settings.value("ThreadCount").toInt();
    int channelCount= settings.value("ChannelCount").toInt();
    int timeToRetry= settings.value("TimeToRetry").toInt();
    int tryCount= settings.value("TryCount").toInt();
    int logLevel= settings.value("LogLevel").toInt();
    QString maxMemorySize= settings.value("MaxMemorySize").toString();
    bool deepInspectionLog= settings.value("DeepInspectionLog").toBool();

    if(threadCount <= 0)
        return false;

    QString pathToLog= settings.value("LogFile").toString();

    quint64 memorySize= 0;
    quint64 factor= 0;
    if(maxMemorySize.endsWith("G"))
    {
        factor= 1024 * 1024 * 1024;
    }
    else if(maxMemorySize.endsWith("M"))
    {
        factor= 1024 * 1024;
    }
    maxMemorySize= maxMemorySize.remove(maxMemorySize.length() - 1, 1);

    memorySize= factor * maxMemorySize.toULongLong();

    auto listIpBan= ipBan.split(",", QString::SkipEmptyParts);

    if(deepInspectionLog)
    {
        m_logController->listenObjects(&m_serverManager);
    }
    m_logController->setLogLevel(static_cast<LogController::LogLevel>(logLevel));

    LogController::StorageModes modes= LogController::Console;

    if(!pathToLog.isEmpty())
        modes|= LogController::File;

    m_logController->setCurrentModes(modes);

    m_serverManager.insertField("port", port);
    m_serverManager.insertField("ServerPassword", password);
    m_serverManager.insertField("IpRange", range);
    m_serverManager.insertField("IpBan", listIpBan);
    m_serverManager.insertField("ConnectionMax", connectionMax);
    m_serverManager.insertField("TimeStart", timeStart);
    m_serverManager.insertField("TimeEnd", timeEnd);
    m_serverManager.insertField("AdminPassword", adminPassword);
    m_serverManager.insertField("IpMode", ipMode);                       // v4 v6 any
    m_serverManager.insertField("ThreadCount", threadCount);             // thread count
    m_serverManager.insertField("ChannelCount", channelCount);           // channel count
    m_serverManager.insertField("TimeToRetry", timeToRetry);             // TimeToRetry
    m_serverManager.insertField("TryCount", tryCount);                   // TimeToRetry
    m_serverManager.insertField("LogLevel", logLevel);                   // loglevel
    m_serverManager.insertField("LogFile", pathToLog);                   // logpath
    m_serverManager.insertField("DeepInspectionLog", deepInspectionLog); // logpath
    m_serverManager.insertField("MemorySize", memorySize);               // max memory size

    m_serverManager.initServerManager();

    return true;
}

void RolisteamDaemon::start()
{
    connect(&m_thread, SIGNAL(started()), &m_serverManager, SLOT(startListening()));
    connect(&m_serverManager, &ServerManager::sendLog, m_logController, &LogController::manageMessage,
        Qt::QueuedConnection);
    connect(&m_serverManager, &ServerManager::finished, &m_thread, &QThread::quit);
    connect(&m_thread, &QThread::finished, this, &RolisteamDaemon::stopped, Qt::QueuedConnection);
    m_serverManager.moveToThread(&m_thread);

    m_thread.start();
}

void RolisteamDaemon::createEmptyConfigFile(QString filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);

    settings.setValue("port", 6660);
    settings.setValue("ServerPassword", QStringLiteral(""));
    settings.setValue("IpRange", QStringLiteral(""));
    settings.setValue("IpBan", QStringLiteral(""));
    settings.setValue("ConnectionMax", 50);
    settings.setValue("TimeStart", QStringLiteral(""));
    settings.setValue("TimeEnd", QStringLiteral(""));
    settings.setValue("IpMode", QStringLiteral(""));
    settings.setValue("ThreadCount", 1);
    settings.setValue("ChannelCount", 1);
    settings.setValue("TimeToRetry", 100);
    settings.setValue("TryCount", 10);
    settings.setValue("LogLevel", 1);
    settings.setValue("LogFile", QStringLiteral(""));
    settings.setValue("DeepInspectionLog", false);
    settings.setValue("AdminPassword", m_serverManager.getValue("AdminPassword"));
    settings.setValue("MaxMemorySize", m_serverManager.getValue("MemorySize"));

    settings.sync();
}
