#include "messagedispatcher.h"
#include "network/networkmessagereader.h"


MessageDispatcher::MessageDispatcher(QObject *parent) : QObject(parent)
{

}

void MessageDispatcher::dispatchMessage(QByteArray data, Channel* channel, TcpClient *emitter)
{
    NetworkMessageReader* msg = new NetworkMessageReader();

    msg->setData(data);

    switch (msg->category())
    {
            case NetMsg::AdministrationCategory:
                emit messageForAdmin(msg,channel,emitter);
            break;
            default:
                channel->sendToAll(msg,emitter);
            break;
    }


}
