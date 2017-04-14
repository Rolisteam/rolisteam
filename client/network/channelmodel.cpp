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
        childItem = m_root.at(row);
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
    if (!child.isValid())
    return QModelIndex();

    TreeItem* childItem = static_cast<TreeItem*>(child.internalPointer());
    if(nullptr!=childItem)
    {
        TreeItem* parentItem = childItem->getParent();

        if (m_root.contains(parentItem))
        {
            return QModelIndex();
        }
        return createIndex(parentItem->rowInParent(), 0, parentItem);
    }
    return QModelIndex();
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
        return m_root[indexChan]->addChild(new TcpClientItem(tr("channel_%1").arg(m_root.size()),client));
    }
}

void ChannelModel::readSettings()
{

}
#include <QSettings>
#include <QList>
void ChannelModel::writeSettings()
{
    QSettings settings("Rolisteam","roliserver");

    settings.beginWriteArray("channels");
    for (int i = 0; i < m_child.size(); ++i)
    {
        if(m_child.at(i).isLeaf())
        {
            Channel* item = dynamic_cast<Channel*>(m_child.at(i));
            if(nullptr != item)
            {
                settings.setArrayIndex(i);
                settings.setValue("title", item->title());
                settings.setValue("password", item->password());
                settings.setValue("description", item->description());
                settings.setValue("usersListed", item->usersListed());
            }
        }
    }
    settings.endArray();

}

TreeItem::TreeItem()
{

}

void TreeItem::addChild()
{

}

bool TreeItem::isLeaf() const
{
    return true;
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

int TreeItem::rowInParent()
{
    return m_parent->indexOf(this);
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

int TcpClientItem::indexOf(TreeItem *child)
{
    return -1;
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

int Channel::indexOf(TreeItem *child)
{
    return m_child.indexOf(child);
}

QString Channel::title() const
{
    return m_title;
}

void Channel::setTitle(const QString &title)
{
    m_title = title;
}

QString Channel::description() const
{
    return m_description;
}

void Channel::setDescription(const QString &description)
{
    m_description = description;
}

bool Channel::usersListed() const
{
    return m_usersListed;
}

void Channel::setUsersListed(bool usersListed)
{
    m_usersListed = usersListed;
}

bool Channel::isLeaf() const
{
    return false;
}
