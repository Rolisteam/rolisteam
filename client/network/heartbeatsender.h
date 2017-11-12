#ifndef HEARTBEATSENDER_H
#define HEARTBEATSENDER_H

#include <QObject>
#include <QTimer>

#include "preferences/preferenceslistener.h"
#include "preferences/preferencesmanager.h"
/**
 * @brief The heartBeatSender class is dedicated to send off heartbeat messages preventing
 * disconnection for some users.
 */
class heartBeatSender : public QObject, public PreferencesListener
{
    Q_OBJECT
public:
    explicit heartBeatSender(QObject *parent = 0);

    void preferencesHasChanged(QString);
    void updateStatus();

    void setIdLocalUser(QString);

public slots:

    void sendHeartBeatMsg();
    void updateTimer();
private:
    QTimer m_timer;
    PreferencesManager* m_preferences;
    int m_timeOut;
    QString m_localId;
    bool m_status;
};

#endif // HEARTBEATSENDER_H
