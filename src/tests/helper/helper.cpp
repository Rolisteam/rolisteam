#include "helper.h"
#include "core/network/networkmessagewriter.h"

namespace Helper
{

TestMessageSender::TestMessageSender() {}

void TestMessageSender::sendMessage(const NetworkMessage* msg)
{
    auto const writer= dynamic_cast<const NetworkMessageWriter*>(msg);
    if(writer)
        m_msgData= writer->data();
}

QByteArray TestMessageSender::messageData() const
{
    return m_msgData;
}

} // namespace Helper
