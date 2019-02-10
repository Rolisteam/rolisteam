#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#include "channelmodel.h"
#include "networkmessage.h"
#include "tcpclient.h"
#include <QByteArray>
#include <QObject>

class NetworkLink;
class NetworkMessageReader;
/**
 * @brief The MessageDispatcher class
 */
class MessageDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit MessageDispatcher(QObject* parent= nullptr);

    static QString cat2String(NetworkMessageHeader* head);
    static QString act2String(NetworkMessageHeader* head);
signals:
    void messageForAdmin(NetworkMessageReader*, Channel* channel, TcpClient* emitter);
    void playerNameChanged(const QString& uuid, const QString& name);

public slots:
    void dispatchMessage(QByteArray msg, Channel* channel, TcpClient* emitter);
};

#endif // MESSAGEDISPATCHER_H
