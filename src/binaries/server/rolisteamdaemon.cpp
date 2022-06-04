#include "rolisteamdaemon.h"
#include <QDebug>
#include <QFile>
#include <QTime>

#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>

RolisteamDaemon::RolisteamDaemon(QObject* parent)
    : QObject(parent), m_server(m_parameters, false), m_logController(new LogController(true, this))
{
    qRegisterMetaType<RServer::ServerState>();
}

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

    auto listIpBan= ipBan.split(",", Qt::SkipEmptyParts);

    if(deepInspectionLog)
    {
        m_logController->listenObjects(&m_server);
    }
    m_logController->setLogLevel(static_cast<LogController::LogLevel>(logLevel));

    LogController::StorageModes modes= LogController::Console;

    if(!pathToLog.isEmpty())
    {
        m_logController->setCurrentPath(pathToLog);
        modes|= LogController::File;
    }

    m_logController->setCurrentModes(modes);

    m_parameters.insert("port", port);
    m_parameters.insert("ServerPassword", password);
    m_parameters.insert("IpRange", range);
    m_parameters.insert("IpBan", listIpBan);
    m_parameters.insert("ConnectionMax", connectionMax);
    m_parameters.insert("TimeStart", timeStart);
    m_parameters.insert("TimeEnd", timeEnd);
    m_parameters.insert("AdminPassword", adminPassword);
    m_parameters.insert("IpMode", ipMode);                       // v4 v6 any
    m_parameters.insert("ThreadCount", threadCount);             // thread count
    m_parameters.insert("ChannelCount", channelCount);           // channel count
    m_parameters.insert("TimeToRetry", timeToRetry);             // TimeToRetry
    m_parameters.insert("TryCount", tryCount);                   // TimeToRetry
    m_parameters.insert("LogLevel", logLevel);                   // loglevel
    m_parameters.insert("LogFile", pathToLog);                   // logpath
    m_parameters.insert("DeepInspectionLog", deepInspectionLog); // logpath
    m_parameters.insert("MemorySize", memorySize);               // max memory size

    return true;
}

void RolisteamDaemon::start()
{
    connect(&m_thread, &QThread::started, &m_server, &RServer::listen);
    connect(&m_server, &RServer::eventOccured, m_logController, &LogController::manageMessage, Qt::QueuedConnection);
    connect(&m_server, &RServer::completed, &m_thread, &QThread::quit);
    connect(&m_server, &RServer::stateChanged, this, [this]() {
        if(m_server.state() == RServer::Stopped)
            m_thread.quit();
    });
    connect(&m_thread, &QThread::finished, this, [this]() {
        if(m_restart)
        {
            qApp->exit(0);
        }
        m_restart= false;
    });

    m_server.moveToThread(&m_thread);

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
    settings.setValue("AdminPassword", QStringLiteral(""));
    settings.setValue("MaxMemorySize", QStringLiteral(""));

    settings.sync();
}

void RolisteamDaemon::restart()
{
    m_restart= true;
    QMetaObject::invokeMethod(&m_server, &RServer::close, Qt::QueuedConnection);
}

void RolisteamDaemon::stop()
{
    QMetaObject::invokeMethod(&m_server, &RServer::close, Qt::QueuedConnection);
}
