#include "rolisteamdaemon.h"
#include <QFile>
#include <QDebug>
#include <QTime>

#include <QSettings>

RolisteamDaemon::RolisteamDaemon(QObject *parent)
    : QObject(parent)//,m_serverManager(new ServerManager())
{

}

void RolisteamDaemon::readConfigFile(QString filepath)
{
    QSettings settings(filepath,QSettings::IniFormat);

    int port = settings.value("port").toInt();
    QString password = settings.value("AdminPassword").toString();
    //QFile file(filepath);
    m_serverManager.setPort(port);

    connect(&m_thread,SIGNAL(started()),&m_serverManager,SLOT(startListening()));
    connect(&m_serverManager,SIGNAL(sendLog(QString)),this,SLOT(notifyUser(QString)));
    connect(&m_serverManager,SIGNAL(errorOccurs(QString)),this,SLOT(errorMessage(QString)));
    m_serverManager.moveToThread(&m_thread);

    m_thread.start();

}

#include <iostream>
void RolisteamDaemon::createEmptyConfigFile()
{
    QTextStream out(stdout);

    out << "configuration file:\n";
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
