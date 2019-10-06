#include "heartbeatsender.h"
#include "networkmessagewriter.h"

#include <QDebug>

heartBeatSender::heartBeatSender(QObject* parent) : QObject(parent)
{
    m_preferences= PreferencesManager::getInstance();

    m_preferences->registerListener("HeartBeatStatus", this);
    m_preferences->registerListener("HbFrequency", this);

    connect(&m_timer, SIGNAL(timeout()), SLOT(sendHeartBeatMsg()));

    updateStatus();
}
void heartBeatSender::preferencesHasChanged(QString)
{
    updateStatus();
}
void heartBeatSender::updateStatus()
{
    m_status= m_preferences->value("HeartBeatStatus", false).toBool();
    m_timeOut= m_preferences->value("HbFrequency", 60).toInt();
    updateTimer();
}

void heartBeatSender::setIdLocalUser(QString str)
{
    m_localId= str;
}
void heartBeatSender::updateTimer()
{
    m_timer.stop();
    if(m_status)
    {
        m_timer.start(m_timeOut * 1000);
    }
}
void heartBeatSender::sendHeartBeatMsg()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::Heartbeat);
    msg.string8(m_localId);
    msg.sendToServer();
}
