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


    void dispatchMessage(QByteArray msg, Channel* channel, TcpClient* emitter);

signals:
    void messageForAdmin(NetworkMessageReader* );


public slots:
};

#endif // MESSAGEDISPATCHER_H
