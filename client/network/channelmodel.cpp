#include "channelmodel.h"

ChannelModel::ChannelModel()
{

}

QModelIndex ChannelModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row<0)
        return QModelIndex();



    TreeItem* childItem = nullptr;
    if (!parent.isValid())
    {
        childItem = m_root.indexOf(parent);
    }
    else
    {
        TreeItem* parentItem = static_cast<TreeItem*>(parent.internalPointer());
        childItem = parentItem->getChildAt(row);
    }

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TreeItem* tmp = static_cast<TreeItem*>(index.internalPointer());
    if(tmp)
    {
        if((role == Qt::DisplayRole)||(Qt::EditRole==role))
        {
            return tmp->getName();
        }
    }
    return QVariant();
}

QModelIndex ChannelModel::parent(const QModelIndex &child) const
{
    if (!index.isValid())
    return QModelIndex();

    TreeItem* childItem = static_cast<TreeItem*>(child.internalPointer());
    if(nullptr!=childItem)
    {
        TreeItem* parentItem = childItem->getParentNode();

        if (parentItem == m_rootItem)
        {
            return QModelIndex();
        }
    }

    return createIndex(parentItem->rowInParent(), 0, parentItem);
}

int ChannelModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
    {
        return m_root.size();
    }
    else
    {
        TreeItem* item = static_cast<TreeItem*>(parent.internalPointer());
        if(NULL!=item)
        {
            return item->childCount();
        }
    }
    return 0;
}

int ChannelModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int ChannelModel::addChannel(QString name, QString password)
{
    Channel* chan = new Channel(name);
    chan->setPassword(password);
    m_root.append(chan);
    return m_root.indexOf(chan);
}

int ChannelModel::addConnectionToChannel(int indexChan, TcpClient* client)
{
    if(!m_root.isEmpty() && m_root.size()<indexChan)
    {
        return m_root[indexChan]->addChild(new TcpClientItem(client));
    }
}

TreeItem::TreeItem()
{

}

void TreeItem::addChild()
{

}

bool TreeItem::isLeaf() const
{

}

int TreeItem::childCount() const
{

}

int TreeItem::addChild(TreeItem *)
{

}

TreeItem *TreeItem::getChildAt(int row)
{

}

TreeItem *TreeItem::getParent() const
{
    return m_parent;
}

void TreeItem::setParent(TreeItem *parent)
{
    m_parent = parent;
}

QString TreeItem::getName() const
{
    return m_name;
}

void TreeItem::setName(const QString &name)
{
    m_name = name;
}

TcpClientItem::TcpClientItem(QString name, TcpClient *client)
{

}

TcpClient *TcpClientItem::client() const
{
    return m_client;
}

void TcpClientItem::setClient(TcpClient *client)
{
    m_client = client;
}

Channel::Channel(QString)
{

}

Channel::~Channel()
{

}

QString Channel::password() const
{
    return m_password;
}

void Channel::setPassword(const QString &password)
{
    m_password = password;
}
