#include "network/heartbeatsender.h"
#include "network/networkmessagewriter.h"

HeartBeatSender::HeartBeatSender(QObject* parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this,  &HeartBeatSender::sendHeartBeatMsg);
    connect(this, &HeartBeatSender::timeOutChanged, this, &HeartBeatSender::updateTimer);
    connect(this, &HeartBeatSender::activeChanged, this, &HeartBeatSender::updateTimer);
}


void HeartBeatSender::setIdLocalUser(const QString& str)
{
    if(str == m_localId)
        return;
    m_localId= str;
    emit localIdChanged();
}

QString HeartBeatSender::localId() const
{
    return m_localId;
}
void HeartBeatSender::updateTimer()
{
    m_timer.stop();
    if(m_active)
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

int HeartBeatSender::timeOut() const
{
    return m_timeOut;
}

void HeartBeatSender::setTimeOut(int newTimeOut)
{
    if (m_timeOut == newTimeOut)
        return;
    m_timeOut = newTimeOut;
    emit timeOutChanged();
}

bool HeartBeatSender::active() const
{
    return m_active;
}

void HeartBeatSender::setActive(bool newActive)
{
    if (m_active == newActive)
        return;
    m_active = newActive;
    emit activeChanged();
}
