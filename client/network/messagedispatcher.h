#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#include <QObject>
#include <QByteArray>
#include "channelmodel.h"
#include "tcpclient.h"
#include "networkmessage.h"


class NetworkLink;
class NetworkMessageReader;
/**
 * @brief The MessageDispatcher class
 */
class MessageDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit MessageDispatcher(QObject *parent = 0);

    static QString cat2String(NetworkMessageHeader *head);
    static QString act2String(NetworkMessageHeader *head);
signals:
    void messageForAdmin(NetworkMessageReader* , Channel* channel, TcpClient* emitter );


public slots:
    void dispatchMessage(QByteArray msg, Channel* channel, TcpClient* emitter);

};

#endif // MESSAGEDISPATCHER_H
