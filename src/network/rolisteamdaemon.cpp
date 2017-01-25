#include "rolisteamdaemon.h"
<<<<<<< 362882ae05f6609442b8044e79149758d3b522f5
=======
#include <QFile>
#include <QDebug>
#include <QTime>
>>>>>>> -Work on server. The server is on but it can't understand anything.

RolisteamDaemon::RolisteamDaemon(QObject *parent)
    : QObject(parent)
{

}

void RolisteamDaemon::readConfigFile(QString)
{
<<<<<<< 362882ae05f6609442b8044e79149758d3b522f5
=======
    //QFile file(filepath);
    m_serverManager.setPort(6660);

    connect(&m_thread,SIGNAL(started()),&m_serverManager,SLOT(startListening()));
    connect(&m_serverManager,SIGNAL(sendLog(QString)),this,SLOT(notifyUser(QString)));
    connect(&m_serverManager,SIGNAL(errorOccurs(QString)),this,SLOT(errorMessage(QString)));
    m_serverManager.moveToThread(&m_thread);

    m_thread.start();
>>>>>>> -Work on server. The server is on but it can't understand anything.

}

void RolisteamDaemon::createEmptyConfigFile(QString)
{

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
