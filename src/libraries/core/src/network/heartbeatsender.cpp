#include "network/heartbeatsender.h"
#include "logcategories.h"
#include "network/networkmessagewriter.h"

HeartBeatSender::HeartBeatSender(QObject* parent) : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this,
            [this]()
            {
                qCDebug(logCategory::network) << "Timeout send heartbeatAsk";
                emit sendOff(new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::HeartbeatAsk));
                m_awaitingAnswer++;
                if(m_awaitingAnswer > m_maxMissingAnswer)
                    emit disconnectionDetected();
            });
    connect(this, &HeartBeatSender::intervalChanged, this, &HeartBeatSender::updateTimer);
}

void HeartBeatSender::updateTimer()
{
    qCDebug(logCategory::network) << "UpdateTimer";
    auto act= active();
    m_timer.stop();
    if(act)
        m_timer.start(m_interval * 1000);
}

bool HeartBeatSender::active() const
{
    return m_timer.isActive();
}

int HeartBeatSender::maxMissingAnswer() const
{
    return m_maxMissingAnswer;
}

void HeartBeatSender::setMaxMissingAnswer(int missingAnswerCount)
{
    if(m_maxMissingAnswer == missingAnswerCount)
        return;
    m_maxMissingAnswer= missingAnswerCount;
    emit maxMissingAnswerChanged();
}

int HeartBeatSender::interval() const
{
    return m_interval;
}

void HeartBeatSender::setInterval(int newInterval)
{
    if(m_interval == newInterval)
        return;
    m_interval= newInterval;
    emit intervalChanged();
}

void HeartBeatSender::start()
{
    qCDebug(logCategory::network) << "Start Timer";
    m_timer.start(m_interval * 1000);
}

void HeartBeatSender::stop()
{
    qCDebug(logCategory::network) << "Stop Timer";
    m_timer.stop();
}

void HeartBeatSender::receivedAnswer()
{
    --m_awaitingAnswer;
}
