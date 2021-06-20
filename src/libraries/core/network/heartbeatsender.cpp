#include "heartbeatsender.h"
#include "networkmessagewriter.h"

HeartBeatSender::HeartBeatSender(QObject* parent) : QObject(parent)
{
    m_preferences= PreferencesManager::getInstance();

    m_preferences->registerListener("HeartBeatStatus", this);
    m_preferences->registerListener("HbFrequency", this);

    connect(&m_timer, SIGNAL(timeout()), SLOT(sendHeartBeatMsg()));

    updateStatus();
}
void HeartBeatSender::preferencesHasChanged(const QString&)
{
    updateStatus();
}
void HeartBeatSender::updateStatus()
{
    m_status= m_preferences->value("HeartBeatStatus", false).toBool();
    m_timeOut= m_preferences->value("HbFrequency", 60).toInt();
    updateTimer();
}

void HeartBeatSender::setIdLocalUser(QString str)
{
    m_localId= str;
}
void HeartBeatSender::updateTimer()
{
    m_timer.stop();
    if(m_status)
    {
        m_timer.start(m_timeOut * 1000);
    }
}
void HeartBeatSender::sendHeartBeatMsg()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::Heartbeat);
    msg.string8(m_localId);
    msg.sendToServer();
}
