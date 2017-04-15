#ifndef TCPCLIENTITEM_H
#define TCPCLIENTITEM_H
#include "treeitem.h"
#include "channel.h"
class TcpClient;
/**
 * @brief The TcpClientItem class
 */
class TcpClientItem : public TreeItem
{
public:
    TcpClientItem(QString name,TcpClient* client);

    TcpClient* client() const;
    void setClient(TcpClient *client);

    virtual int indexOf(TreeItem* child);

    void setChannelParent(Channel* chan);

private:
    TcpClient* m_client;
};
#endif // TCPCLIENTITEM_H
