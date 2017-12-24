#include "rolisteamdaemon.h"
#include <QFile>
#include <QDebug>
#include <QTime>

#include <QSettings>
#include <QFileInfo>

RolisteamDaemon::RolisteamDaemon(QObject *parent)
    : QObject(parent)//,m_serverManager(new ServerManager())
{

}

bool RolisteamDaemon::readConfigFile(QString filepath)
{

    if(!QFileInfo::exists(filepath))
        return false;

    QSettings settings(filepath,QSettings::IniFormat);

    int port = settings.value("port").toInt();
    QString password = settings.value("password").toString();
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


    m_serverManager.insertField("port",port);
    m_serverManager.insertField("password",password);
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
    m_serverManager.insertField("LogLevel",logLevel);//channel count

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

#include <iostream>
void RolisteamDaemon::createEmptyConfigFile(QString filepath)
{
    QSettings settings(filepath,QSettings::IniFormat);

    settings.setValue("port",m_serverManager.getValue("port"));
    settings.setValue("password",m_serverManager.getValue("password"));
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
    settings.setValue("AdminPassword",m_serverManager.getValue("AdminPassword"));

    settings.sync();
}

int RolisteamDaemon::getLevelOfLog()
{
    return m_serverManager.getValue("LogLevel").toInt();
}
void RolisteamDaemon::notifyUser(QString str)
{
    qInfo() << str;
}

void RolisteamDaemon::errorMessage(QString str)
{

    QString time = QTime::currentTime().toString("hh:mm:ss") + " - ";
    qDebug() << time << tr("Error!")<< str;
}
