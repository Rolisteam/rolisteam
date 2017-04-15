#include "tcpclientitem.h"
#include "tcpclient.h"


TcpClientItem::TcpClientItem(QString name, TcpClient *client)
    : m_client(client)
{
    m_name = name;
}

TcpClient *TcpClientItem::client() const
{
    return m_client;
}

void TcpClientItem::setClient(TcpClient *client)
{
    m_client = client;
}

int TcpClientItem::indexOf(TreeItem *child)
{
    return -1;
}
void TcpClientItem::setChannelParent(Channel* chan)
{
    if(nullptr != m_client)
    {
        m_client->setParentChannel(chan);
    }
}
