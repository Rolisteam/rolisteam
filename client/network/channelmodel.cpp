#include "channelmodel.h"

#include "treeitem.h"
#include "tcpclient.h"

#include <QSettings>
#include <QList>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QStyle>
#endif

#include "channel.h"
#include "receiveevent.h"
ChannelModel::ChannelModel()
{
    //m_root = new Channel();
   ReceiveEvent::registerNetworkReceiver(NetMsg::AdministrationCategory,this);
}

QModelIndex ChannelModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row<0)
        return QModelIndex();

    TreeItem* childItem = nullptr;
    if (!parent.isValid())
    {
       // qDebug() << "invalid parent";
        //childItem = m_root->getChildAt(row);
        childItem = m_root.at(row);
    }
    else
    {
       // qDebug() << "valid parent";
        TreeItem* parentItem = static_cast<TreeItem*>(parent.internalPointer());
        childItem = parentItem->getChildAt(row);
    }

    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
        return QModelIndex();
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TreeItem* tmp = static_cast<TreeItem*>(index.internalPointer());
   // qDebug() << "[data channelmodel]"<<  tmp;
    if(tmp!=nullptr)
    {
        if((role == Qt::DisplayRole)||(Qt::EditRole==role))
        {
            return tmp->getName();
        }
        #ifdef QT_WIDGETS_LIB
        else if(role == Qt::DecorationRole)
        {
            if(!tmp->isLeaf())
            {
                QStyle* style = qApp->style();

                return style->standardIcon(QStyle::SP_DirIcon);
                //return QIcon(":/resources/icons/folder.png");
            }
        }
        #endif
    }
    return QVariant();
}

bool ChannelModel::setData(const QModelIndex &index, const QVariant &value, int )
{
    if(!index.isValid())
        return false;

    TreeItem* tmp = static_cast<TreeItem*>(index.internalPointer());
    if(tmp!=nullptr)
    {
        tmp->setName(value.toString());
        return true;
    }
    return false;
}

QModelIndex ChannelModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeItem* childItem = static_cast<TreeItem*>(child.internalPointer());
    if(nullptr!=childItem)
    {
        TreeItem* parentItem = childItem->getParentItem();
       // qDebug() << "parent Item" << parentItem << childItem->childCount()<< childItem;

        if(m_root.contains(childItem))
        {
            return QModelIndex();
        }
        if(m_root.contains(parentItem))
        {
            return createIndex(m_root.indexOf(parentItem), 0, parentItem);
        }
        return createIndex(parentItem->rowInParent(), 0, parentItem);
    }
    return QModelIndex();
}

int ChannelModel::rowCount(const QModelIndex &parent) const
{
    int result = 0;
    if(!parent.isValid())
    {
        result = m_root.size();
    }
    else
    {
        TreeItem* item = static_cast<TreeItem*>(parent.internalPointer());
        if(nullptr!=item)
        {
            result = item->childCount();
        }
    }
   // qDebug() << "[Number of child]"<< result << parent.isValid();
    return result;
}

int ChannelModel::columnCount(const QModelIndex &) const
{
    return 1;
}

int ChannelModel::addChannel(QString name, QString password)
{
    Channel* chan = new Channel(name);
    chan->setPassword(password);
    QModelIndex index;
    addChannelToIndex(chan,index);
    return m_root.indexOf(chan);
}
QModelIndex ChannelModel::addChannelToIndex(Channel* channel,QModelIndex& parent)
{
    int row = -1;
    if(!parent.isValid())
    {
       beginInsertRows(parent,m_root.size(),m_root.size());
       m_root.append(channel);
       endInsertRows();
       row = m_root.size()-1;
    }
    else
    {
        Channel* item = static_cast<Channel*>(parent.internalPointer());
        if(nullptr!=item)
        {
            beginInsertRows(parent,item->childCount(),item->childCount());
            item->addChild(channel);
            endInsertRows();
            row = item->childCount()-1;
        }
    }
    return index(row,0,parent);
}
bool ChannelModel::addChannelToChannel(Channel* child, Channel* parent)
{
    bool result = false;
    if(nullptr == parent)
    {
        beginInsertRows(QModelIndex(),m_root.size(),m_root.size());
        m_root.append(child);
        endInsertRows();
        result = true;
    }
    else
    {

        QModelIndex index = channelToIndex(parent);
        beginInsertRows(index,parent->childCount(),parent->childCount());
        parent->addChild(child);
        endInsertRows();
        result = true;
    }
    return  result;
}
QModelIndex ChannelModel::channelToIndex(Channel* channel)
{
    QList<TreeItem*> listOfParent;
    TreeItem* tmp = channel;
    while(nullptr != tmp)
    {
        listOfParent.prepend(tmp);
        tmp = tmp->getParentItem();
    }
    QModelIndex parent;
    for(auto item : listOfParent)
    {
        if(nullptr!=item->getParentItem())
        {
            parent = parent.child(m_root.indexOf(item),0);
        }
        else
        {
            parent = parent.child(item->rowInParent(),0);
        }
    }
    return parent;
}

NetWorkReceiver::SendType ChannelModel::processMessage(NetworkMessageReader *msg, NetworkLink *link)
{
    if(NetMsg::AddChannel == msg->action())
    {
        Channel* channel = new Channel();
        QString idParent = msg->string8();
        channel->read(*msg);

        auto item = getItemById(idParent);
        Channel* parent = static_cast<Channel*>(item);

        addChannelToChannel(channel, parent);

    }
}
Qt::ItemFlags ChannelModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    if(isAdmin())
    {
        return Qt::ItemIsEnabled  | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else
    {
        return Qt::ItemIsEnabled  | Qt::ItemIsSelectable;//| Qt::ItemIsEditable
    }
}
bool ChannelModel::hasChildren ( const QModelIndex & parent  ) const
{

    if(!parent.isValid())//root
    {
        return  !m_root.isEmpty();//==0?false:true;
    }
    else
    {
        TreeItem* childItem = static_cast<TreeItem*>(parent.internalPointer());
        if(childItem->childCount()==0)
            return false;
        else
            return true;
    }
}
bool ChannelModel::addConnectionToDefaultChannel(TcpClient* client)
{
    if(m_defaultChannel.isEmpty())
    {
        if(!m_root.isEmpty())
        {
            auto item = m_root.at(0);
            if(nullptr != item)
            {
                m_defaultChannel = item->getId();
            }
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
    for(auto item : m_root)
    {
        //TreeItem* item = m_root->getChildAt(i);
        if(nullptr != item)
        {
            found = item->addChildInto(chanId,client);
        }
    }
    return found;
}
void ChannelModel::readDataJson(const QJsonObject& obj)
{
    beginResetModel();
    for(auto item : m_root)
    {
        item->clear();
    }
    m_root.clear();
    QJsonArray channels = obj["channel"].toArray();
    for(auto channelJson : channels)
    {
        Channel* tmp = new Channel();
        QJsonObject obj = channelJson.toObject();
        tmp->setParentItem(nullptr);
        tmp->readFromJson(obj);
        m_root.append(tmp);
    }
    endResetModel();


    ///////////
  /*  qDebug() << "m_root:" << m_root->childCount();

    for(auto item : m_root)
    {
        qDebug() << "child:" << item->childCount();
    }*/
}

void ChannelModel::writeDataJson(QJsonObject& obj)
{
    QJsonArray array;
    for(auto item : m_root) //int i = 0; i< m_root->childCount(); ++i)
    {
        //auto item = m_root->getChildAt(i);
        if(nullptr != item)
        {
            QJsonObject jsonObj;
            item->writeIntoJson(jsonObj);
            array.append(jsonObj);
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

void ChannelModel::kick(QString id)
{
    for(auto item : m_root)
    {
        if(nullptr != item)
        {
            item->kick(id);
        }
    }
}

bool ChannelModel::isAdmin() const
{
    return m_admin;
}

void ChannelModel::setAdmin(bool admin)
{
    m_admin = admin;
}
TreeItem* ChannelModel::getItemById(QString id)
{
    for(auto item : m_root)
    {
        if(nullptr != item)
        {
            if(item->getId() == id)
            {
                return item;
            }
            else
            {
                TreeItem* child = item->getChildById(id);
                if(nullptr!= child)
                {
                    return child;
                }
            }
        }
    }
    return nullptr;
}



