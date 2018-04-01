#include "rolisteamdaemon.h"
#include <QFile>
#include <QDebug>
#include <QTime>

#include <QSettings>
#include <QFileInfo>

RolisteamDaemon::RolisteamDaemon(QObject *parent)
    : QObject(parent),m_logController(new LogController(true,this))
{

}

bool RolisteamDaemon::readConfigFile(QString filepath)
{

    if(!QFileInfo::exists(filepath))
        return false;

    QSettings settings(filepath,QSettings::IniFormat);

    int port = settings.value("port").toInt();
    QString password = settings.value("ServerPassword").toString();
    QString range = settings.value("IpRange").toString();
    QString ipBan = settings.value("IpBan").toString();
    QString connectionMax = settings.value("ConnectionMax").toString();
    QString timeStart = settings.value("TimeStart").toString();
    QString timeEnd= settings.value("TimeEnd").toString();
    QString ipMode= settings.value("IpMode").toString();
    QString adminPassword= settings.value("AdminPassword").toString();
    int threadCount= settings.value("ThreadCount").toInt();
    int channelCount= settings.value("ChannelCount").toInt();
    int timeToRetry= settings.value("TimeToRetry").toInt();
    int tryCount= settings.value("TryCount").toInt();
    int logLevel= settings.value("LogLevel").toInt();
    bool deepInspectionLog = settings.value("DeepInspectionLog").toInt();

    QString pathToLog = settings.value("LogFile").toString();


    if(deepInspectionLog)
    {
        m_logController->listenObjects(&m_serverManager);
    }
    m_logController->setLogLevel(static_cast<LogController::LogLevel>(logLevel));

    m_logController->setCurrentModes(LogController::Console | LogController::File );//*/

    m_serverManager.insertField("port",port);
    m_serverManager.insertField("ServerPassword",password);
    m_serverManager.insertField("IpRange",range);
    m_serverManager.insertField("IpBan",ipBan);
    m_serverManager.insertField("ConnectionMax",connectionMax);
    m_serverManager.insertField("TimeStart",timeStart);
    m_serverManager.insertField("TimeEnd",timeEnd);
    m_serverManager.insertField("AdminPassword",adminPassword);
    m_serverManager.insertField("IpMode",ipMode);//v4 v6 any
    m_serverManager.insertField("ThreadCount",threadCount);//thread count
    m_serverManager.insertField("ChannelCount",channelCount);//channel count
    m_serverManager.insertField("TimeToRetry",timeToRetry);//TimeToRetry
    m_serverManager.insertField("TryCount",tryCount);//TimeToRetry
    m_serverManager.insertField("LogLevel",logLevel);//loglevel
    m_serverManager.insertField("LogFile",pathToLog);//logpath
    m_serverManager.insertField("DeepInspectionLog",deepInspectionLog);//logpath


    m_serverManager.initServerManager();

    return true;
}

void RolisteamDaemon::start()
{
    connect(&m_thread,SIGNAL(started()),&m_serverManager,SLOT(startListening()));
    connect(&m_serverManager,SIGNAL(sendLog(QString)),this,SLOT(notifyUser(QString)));
    connect(&m_serverManager,SIGNAL(errorOccurs(QString)),this,SLOT(errorMessage(QString)));
    connect(&m_serverManager,&ServerManager::finished,&m_thread,&QThread::quit);
    connect(&m_thread,&QThread::finished,this,&RolisteamDaemon::stopped,Qt::QueuedConnection);
    m_serverManager.moveToThread(&m_thread);

    m_thread.start();
}

void RolisteamDaemon::createEmptyConfigFile(QString filepath)
{
    QSettings settings(filepath,QSettings::IniFormat);

    settings.setValue("port",m_serverManager.getValue("port"));
    settings.setValue("ServerPassword",m_serverManager.getValue("ServerPassword"));
    settings.setValue("IpRange",m_serverManager.getValue("IpRange"));
    settings.setValue("IpBan",m_serverManager.getValue("IpBan"));
    settings.setValue("ConnectionMax",m_serverManager.getValue("ConnectionMax"));
    settings.setValue("TimeStart",m_serverManager.getValue("TimeStart"));
    settings.setValue("TimeEnd",m_serverManager.getValue("TimeEnd"));
    settings.setValue("IpMode",m_serverManager.getValue("IpMode"));
    settings.setValue("ThreadCount",m_serverManager.getValue("ThreadCount"));
    settings.setValue("ChannelCount",m_serverManager.getValue("ChannelCount"));
    settings.setValue("TimeToRetry",m_serverManager.getValue("TimeToRetry"));
    settings.setValue("TryCount",m_serverManager.getValue("TryCount"));
    settings.setValue("LogLevel",m_serverManager.getValue("LogLevel"));
    settings.setValue("LogFile",m_serverManager.getValue("LogFile"));
    settings.setValue("DeepInspectionLog",m_serverManager.getValue("DeepInspectionLog"));
    settings.setValue("AdminPassword",m_serverManager.getValue("AdminPassword"));

    settings.sync();
}
