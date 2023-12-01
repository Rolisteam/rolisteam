#ifndef HEARTBEATSENDER_H
#define HEARTBEATSENDER_H

#include <QObject>
#include <QTimer>

/**
 * @brief The heartBeatSender class is dedicated to send off heartbeat messages preventing
 * disconnection for some users.
 */
class HeartBeatSender : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeOut READ timeOut WRITE setTimeOut NOTIFY timeOutChanged FINAL)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged FINAL)
    Q_PROPERTY(QString localId READ localId WRITE setIdLocalUser NOTIFY localIdChanged FINAL)
public:
    explicit HeartBeatSender( QObject* parent= nullptr);

    void setIdLocalUser(const QString& id);
    QString localId()const;

    int timeOut() const;
    void setTimeOut(int newTimeOut);

    bool active() const;
    void setActive(bool newActive);

public slots:
    void sendHeartBeatMsg();
    void updateTimer();

signals:
    void timeOutChanged();
    void localIdChanged();
    void activeChanged();

private:
    QTimer m_timer;
    int m_timeOut{10000};
    QString m_localId{QStringLiteral("unknown")};
    bool m_active{false};
};

#endif // HEARTBEATSENDER_H
