#include "rolisteamdaemon.h"
#include <QFile>

RolisteamDaemon::RolisteamDaemon(QObject *parent)
    : QObject(parent)//,m_serverManager(new ServerManager())
{

}

void RolisteamDaemon::readConfigFile(QString filepath)
{
    //QFile file(filepath);
    m_serverManager.setPort(6660);

    connect(&m_thread,SIGNAL(started()),&m_serverManager,SLOT(startListening()));
    m_serverManager.moveToThread(&m_thread);

    m_thread.start();

}

void RolisteamDaemon::createEmptyConfigFile(QString)
{

}
