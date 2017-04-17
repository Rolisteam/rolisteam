#include "channelmodel.h"

#include "treeitem.h"
#include "tcpclientitem.h"

#include <QSettings>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "channel.h"

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

        if(m_root.contains(childItem))
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

bool ChannelModel::addConnectionToDefaultChannel(TcpClient* client)
{
    if(m_defaultChannel.isEmpty())
    {
        if(!m_root.isEmpty())
        {
            m_defaultChannel = m_root.at(0)->getId();
        }
        else
        {
            return false;
        }
    }

    return addConnectionToChannel(m_defaultChannel,client);

}

bool ChannelModel::addConnectionToChannel(QString chanId, TcpClient* client)
{
    bool found=false;
    for(int i = 0; i < m_root.size() && !found ; ++i)
    {
        TreeItem* item = m_root.at(i);
        if(nullptr != item)
        {
            found = item->addChildInto(chanId,new TcpClientItem(tr("channel_%1").arg(m_root.size()),client));
        }
    }
    return found;
}
void ChannelModel::readDataJson(const QJsonObject& obj)
{
    beginResetModel();
    QJsonArray channels = obj["channel"].toArray();
    for(auto channelJson : channels)
    {
        Channel* tmp = new Channel();
        QJsonObject obj = channelJson.toObject();
        tmp->readChannel(obj);
        m_root.append(tmp);
    }
    endResetModel();
}

void ChannelModel::writeDataJson(QJsonObject& obj)
{
    QJsonArray array;
    for (int i = 0; i < m_root.size(); ++i)
    {
        if(!m_root.at(i)->isLeaf())
        {
            Channel* item = dynamic_cast<Channel*>(m_root.at(i));
            if(nullptr != item)
            {
                QJsonObject jsonObj;
                item->writeChannel(jsonObj);
                array.append(jsonObj);
            }
        }
    }
    obj["channel"]=array;
}

void ChannelModel::readSettings()
{
    QSettings settings("Rolisteam","roliserver");

    QJsonDocument doc= QJsonDocument::fromVariant(settings.value("channeldata",""));

    QJsonObject obj = doc.object();

    readDataJson(obj);
}


void ChannelModel::writeSettings()
{
    QSettings settings("Rolisteam","roliserver");

    QJsonDocument doc;

    QJsonObject obj;

    writeDataJson(obj);

    doc.setObject(obj);
    settings.setValue("channeldata",doc);


}






