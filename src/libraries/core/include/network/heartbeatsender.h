#ifndef HEARTBEATSENDER_H
#define HEARTBEATSENDER_H

#include <QObject>
#include <QTimer>

class NetworkMessageWriter;
/**
 * @brief The heartBeatSender class is dedicated to send off heartbeat messages preventing
 * disconnection for some users.
 */
class HeartBeatSender : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged FINAL)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged FINAL)
    Q_PROPERTY(
        int maxMissingAnswer READ maxMissingAnswer WRITE setMaxMissingAnswer NOTIFY maxMissingAnswerChanged FINAL)
public:
    explicit HeartBeatSender(QObject* parent= nullptr);

    bool active() const;
    void setActive(bool newActive);

    int maxMissingAnswer() const;
    void setMaxMissingAnswer(int newMissingAnswerCont);

    int interval() const;
    void setInterval(int newInterval);

public slots:
    void start();
    void stop();
    void receivedAnswer();

signals:
    void sendOff(NetworkMessageWriter* msg);
    void activeChanged();
    void disconnectionDetected();
    void maxMissingAnswerChanged();
    void intervalChanged();

private slots:
    void updateTimer();

private:
    QTimer m_timer;
    int m_maxMissingAnswer{3};
    int m_interval{10};
    int m_awaitingAnswer{0};
};

#endif // HEARTBEATSENDER_H
