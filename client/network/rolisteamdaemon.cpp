#include "rolisteamdaemon.h"
#include <QFile>
#include <QDebug>
#include <QTime>

RolisteamDaemon::RolisteamDaemon(QObject *parent)
    : QObject(parent)
{

}

void RolisteamDaemon::readConfigFile(QString)
{
    //QFile file(filepath);
    m_serverManager.setPort(6660);

    connect(&m_thread,SIGNAL(started()),&m_serverManager,SLOT(startListening()));
    connect(&m_serverManager,SIGNAL(sendLog(QString)),this,SLOT(notifyUser(QString)));
    connect(&m_serverManager,SIGNAL(errorOccurs(QString)),this,SLOT(errorMessage(QString)));
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
    settings.setValue("LogLevel",m_serverManager.getValue("LogLevel"));
    settings.setValue("AdminPassword",m_serverManager.getValue("AdminPassword"));

    settings.sync();

    //QTextStream out(stdout);

    //out << "configuration file:\n";
}

int RolisteamDaemon::getLevelOfLog()
{
    return m_serverManager.getValue("LogLevel").toInt();
}
void RolisteamDaemon::notifyUser(QString str)
{
    qDebug() << str;
}

void RolisteamDaemon::errorMessage(QString str)
{

    QString time = QTime::currentTime().toString("hh:mm:ss") + " - ";
    qDebug() << time << tr("Error!")<< str;
}
