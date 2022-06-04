#include "network/channelmodel.h"

#include "network/serverconnection.h"
#include "network/treeitem.h"

#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QSettings>
#include <array>

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#include <QFont>
#include <QInputDialog>
#include <QStyle>
#endif

#include "network/channel.h"
#include "network/receiveevent.h"

quint64 computeTotalSize(const std::map<Channel*, quint64>& map)
{
    quint64 totalSize= 0;
    for(auto pair : map)
    {
        totalSize+= pair.second;
    }
    return totalSize;
}

//////////////////////////////////////
/// ClientMimeData
/////////////////////////////////////

ClientMimeData::ClientMimeData()
{
    setData("application/rolisteam.networkclient.list", QByteArray());
}

void ClientMimeData::addClient(ServerConnection* m, const QModelIndex index)
{
    if(nullptr != m)
    {
        m_clientList.insert(index, m);
    }
}
const QMap<QModelIndex, ServerConnection*>& ClientMimeData::getList() const
{
    return m_clientList;
}
bool ClientMimeData::hasFormat(const QString& mimeType) const
{
    return ((mimeType == "application/rolisteam.networkclient.list") || QMimeData::hasFormat(mimeType));
}

//////////////////////////////////////
/// ChannelModel
/////////////////////////////////////

ChannelModel::ChannelModel() {}

ChannelModel::~ChannelModel()
{
    qDeleteAll(m_root);
    std::vector<Channel*> keys;

    transform(std::begin(m_sizeMap), std::end(m_sizeMap), back_inserter(keys),
              [](decltype(m_sizeMap)::value_type const& pair) { return pair.first; });
    m_sizeMap.clear();
    qDeleteAll(keys);
}

QModelIndex ChannelModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();
    if(column < 0)
        return QModelIndex();

    TreeItem* childItem= nullptr;
    if(!parent.isValid())
    {
        if(m_root.size() > row)
            childItem= m_root.at(row);
    }
    else
    {
        TreeItem* parentItem= static_cast<TreeItem*>(parent.internalPointer());
        childItem= parentItem->getChildAt(row);
    }

    if(childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
        return QModelIndex();
}

std::pair<quint64, QString> ChannelModel::convert(quint64 size) const
{
    constexpr int cap= 1024;
    std::size_t i= 0;
    std::array<QString, 3> units= {tr("Bytes"), tr("KiB"), tr("MiB")};
    while(size > cap && i < units.size())
    {
        size/= cap;
        ++i;
    }
    return std::make_pair(size, units[i]);
}

QVariant ChannelModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TreeItem* tmp= static_cast<TreeItem*>(index.internalPointer());
    if(tmp != nullptr)
    {
        if((role == Qt::DisplayRole) || (Qt::EditRole == role))
        {
            if(!tmp->isLeaf() && role == Qt::DisplayRole)
            {
                auto channel= dynamic_cast<Channel*>(tmp);
                if(nullptr == channel)
                    return {};
                if(isAdmin(m_localPlayerId) || isGM(m_localPlayerId, channel->uuid()))
                {
                    auto size= channel->memorySize();
                    auto pair= convert(size);
                    return QStringLiteral("%1 (%2 %3)").arg(tmp->name()).arg(pair.first).arg(pair.second);
                }
            }
            return tmp->name();
        }
#ifdef QT_WIDGETS_LIB
        else if(role == Qt::FontRole)
        {
            if(tmp->isLeaf())
            {
                auto parent= tmp->getParentItem();
                if(parent)
                {
                    auto channel= dynamic_cast<Channel*>(parent);
                    if(channel)
                    {
                        if(channel->currentGM() == tmp)
                        {
                            QFont font;
                            font.setBold(true);
                            return font;
                        }
                    }
                }
            }
        }
        else if(role == Qt::DecorationRole)
        {
            if(!tmp->isLeaf())
            {
                auto channel= dynamic_cast<Channel*>(tmp);
                QStyle* style= qApp->style();
                if(nullptr != style && nullptr != channel)
                {
                    if(channel->password().isEmpty())
                        return style->standardIcon(QStyle::SP_DirIcon);
                    else
                        return style->standardIcon(QStyle::SP_DirClosedIcon);
                }
            }
        }
#endif
    }
    return QVariant();
}

bool ChannelModel::setData(const QModelIndex& index, const QVariant& value, int)
{
    bool rightToSetName= isAdmin(m_localPlayerId);
    if((!rightToSetName && !localIsGM()) || !index.isValid())
        return false;

    auto tmp= static_cast<TreeItem*>(index.internalPointer());

    if(nullptr == tmp)
        return false;

    if(tmp->isLeaf())
        return false;

    auto chan= dynamic_cast<Channel*>(tmp);
    if(!chan)
        return false;

    if(isGM(m_localPlayerId, chan->uuid()) && !rightToSetName)
    {
        rightToSetName= chan->getCurrentGmId() == m_localPlayerId;
    }

    if(rightToSetName)
    {
        tmp->setName(value.toString());
        emit channelNameChanged(chan->name(), chan->uuid());
        return true;
    }
    return false;
}

QModelIndex ChannelModel::parent(const QModelIndex& child) const
{
    if(!child.isValid())
        return QModelIndex();

    TreeItem* childItem= static_cast<TreeItem*>(child.internalPointer());
    if(nullptr != childItem)
    {
        TreeItem* parentItem= childItem->getParentItem();

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

int ChannelModel::rowCount(const QModelIndex& parent) const
{
    int result= 0;
    if(!parent.isValid())
    {
        result= m_root.size();
    }
    else
    {
        TreeItem* item= static_cast<TreeItem*>(parent.internalPointer());
        if(nullptr != item)
        {
            result= item->childCount();
        }
    }
    return result;
}

int ChannelModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QString ChannelModel::addChannel(QString name, QByteArray password)
{
    Channel* chan= new Channel(name);
    chan->setPassword(password);
    QModelIndex index;
    addChannelToIndex(chan, index);
    return chan->uuid();
}
QModelIndex ChannelModel::addChannelToIndex(Channel* channel, const QModelIndex& parent)
{
    int row= -1;
    if(!parent.isValid())
    {
        beginInsertRows(parent, m_root.size(), m_root.size());
        appendChannel(channel);
        endInsertRows();
        row= m_root.size() - 1;
    }
    else
    {
        Channel* item= static_cast<Channel*>(parent.internalPointer());
        if(nullptr != item)
        {
            addChannelToChannel(channel, item);
            row= item->childCount() - 1;
        }
    }
    return index(row, 0, parent);
}
bool ChannelModel::addChannelToChannel(Channel* child, Channel* parent)
{
    if(child == nullptr)
        return false;

    bool result= false;
    if(nullptr == parent)
    {
        auto index= addChannelToIndex(child, QModelIndex());
        result= index.isValid();
    }
    else
    {
        QModelIndex index= channelToIndex(parent);
        beginInsertRows(index, parent->childCount(), parent->childCount());
        parent->addChild(child);
        endInsertRows();
        result= true;
    }
    return result;
}

void ChannelModel::renameChannel(const QString& senderId, const QString& id, const QString& value)
{
    bool right= isAdmin(senderId);
    if(!right)
    {
        right= isGM(senderId, id);
    }
    if(!right)
        return;

    auto item= getItemById(id);
    if(nullptr == item)
        return;

    item->setName(value);
}

void ChannelModel::appendChannel(Channel* channel)
{
    m_root.append(channel);
    connect(channel, &Channel::memorySizeChanged, this, &ChannelModel::modelChanged);
    connect(channel, &Channel::lockedChanged, this, &ChannelModel::modelChanged);
    connect(channel, &Channel::itemChanged, this, &ChannelModel::modelChanged);
    connect(channel, &Channel::nameChanged, this, &ChannelModel::modelChanged);
    connect(channel, &Channel::uuidChanged, this, &ChannelModel::modelChanged);
}

QModelIndex ChannelModel::channelToIndex(Channel* channel)
{
    QList<TreeItem*> listOfParent;
    TreeItem* tmp= channel;
    while(nullptr != tmp)
    {
        listOfParent.prepend(tmp);
        tmp= tmp->getParentItem();
    }
    QModelIndex parent;
    for(auto item : listOfParent)
    {
        if(nullptr == item->getParentItem())
        {
            parent= index(m_root.indexOf(item), 0, parent);
        }
        else
        {
            parent= index(item->rowInParent(), 0, parent);
        }
    }
    return parent;
}

void ChannelModel::setLocalPlayerId(const QString& id)
{
    m_localPlayerId= id;
}

Qt::ItemFlags ChannelModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    TreeItem* item= static_cast<TreeItem*>(index.internalPointer());

    auto admin= isAdmin(m_localPlayerId);
    if(admin && item->isLeaf())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
    }
    else if(admin && !item->isLeaf())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}
bool ChannelModel::hasChildren(const QModelIndex& parent) const
{
    if(!parent.isValid()) // root
    {
        return !m_root.isEmpty(); //==0?false:true;
    }
    else
    {
        TreeItem* childItem= static_cast<TreeItem*>(parent.internalPointer());
        if(childItem->childCount() == 0)
            return false;
        else
            return true;
    }
}
QStringList ChannelModel::mimeTypes() const
{
    QStringList types;
    types << "application/rolisteam.networkclient.list";
    return types;
}
Qt::DropActions ChannelModel::supportedDropActions() const
{
    return Qt::MoveAction;
}
QMimeData* ChannelModel::mimeData(const QModelIndexList& indexes) const
{
    ClientMimeData* mimeData= new ClientMimeData();

    for(const QModelIndex& index : indexes)
    {
        if((index.isValid()) && (index.column() == 0))
        {
            ServerConnection* item= static_cast<ServerConnection*>(index.internalPointer());
            mimeData->addClient(item, index);
        }
    }
    return mimeData;
}

bool ChannelModel::moveMediaItem(QList<ServerConnection*> items, const QModelIndex& parentToBe, int row,
                                 QList<QModelIndex>& formerPosition)
{
    Q_UNUSED(row)
    Q_UNUSED(formerPosition)
    if(isAdmin(m_localPlayerId))
    {
        if(parentToBe.isValid())
        {
            Channel* item= static_cast<Channel*>(parentToBe.internalPointer());
            QString id= item->uuid();

            QByteArray pw;
#ifdef QT_WIDGETS_LIB
            if(!item->password().isEmpty())
            {
                pw= QInputDialog::getText(nullptr, tr("Channel Password"),
                                          tr("Channel %1 required password:").arg(item->name()), QLineEdit::Password)
                        .toUtf8()
                        .toBase64();
            }
#endif

            for(auto client : items)
            {
                if(!id.isEmpty())
                {
                    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::JoinChannel);
                    msg.string8(id);
                    msg.string8(client->uuid());
                    msg.byteArray32(pw);
                    msg.sendToServer();
                    return true;
                }
            }
        }
    }
    return false;
}
bool ChannelModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                const QModelIndex& parent)
{
    Q_UNUSED(column);

    if(action == Qt::IgnoreAction)
        return false;

    bool added= false;

    if(data->hasFormat("application/rolisteam.networkclient.list"))
    {
        const ClientMimeData* clientData= qobject_cast<const ClientMimeData*>(data);

        if(nullptr != clientData)
        {
            QList<ServerConnection*> clientList= clientData->getList().values();
            QList<QModelIndex> indexList= clientData->getList().keys();
            {
                if(action == Qt::MoveAction)
                {
                    added= moveMediaItem(clientList, parent, row, indexList);
                }
            }
        }
    }
    return added;
}
bool ChannelModel::addConnectionToDefaultChannel(ServerConnection* client)
{
    if(m_defaultChannel.isEmpty())
    {
        qDebug() << "call addConnectionToDefaultChannel while channel is empty";
        if(!m_root.isEmpty())
        {
            auto item= m_root.at(0);
            if(nullptr != item)
            {
                m_defaultChannel= item->uuid();
            }
        }
        else
        {
            return false;
        }
    }

    return addConnectionToChannel(m_defaultChannel, client);
}

bool ChannelModel::addConnectionToChannel(QString chanId, ServerConnection* client)
{
    bool found= false;
    for(auto& item : m_root)
    {
        if(nullptr != item && !found)
        {
            found= item->addChildInto(chanId, client);
        }
    }
    return found;
}

bool ChannelModel::moveClient(Channel* origin, const QString& id, Channel* dest)
{
    if(nullptr == dest || nullptr == origin)
        return false;

    auto sourceParent= channelToIndex(origin);
    auto client= origin->getClientById(id);
    auto indxSource= origin->indexOf(client);
    auto destParent= channelToIndex(dest);
    auto indexDest= dest->childCount();

    if(!sourceParent.isValid() || !destParent.isValid())
        return false;

    beginMoveRows(sourceParent, indxSource, indxSource, destParent, indexDest);
    origin->removeChild(client);
    dest->addChild(client);
    endMoveRows();
    return true;
}

const QList<TreeItem*>& ChannelModel::modelData()
{
    return m_root;
}

void ChannelModel::resetData(QList<TreeItem*> data)
{
    qDeleteAll(m_root);
    m_root.clear();
    beginResetModel();
    m_root= data;
    endResetModel();
}

void ChannelModel::kick(const QString& id, bool isAdmin, const QString& senderId)
{
    for(auto& item : m_root)
    {
        if(nullptr == item)
            continue;

        item->kick(id, isAdmin, senderId);
    }
}

bool ChannelModel::isAdmin(const QString& id) const
{
    auto player= getServerConnectionById(id);
    if(nullptr == player)
        return false;
    return player->isAdmin();
}

bool ChannelModel::isGM(const QString& id, const QString& chanId) const
{
    auto player= getServerConnectionById(id);
    auto item= getItemById(chanId);
    if(nullptr == player || item == nullptr)
        return false;

    auto chan= dynamic_cast<Channel*>(item);
    if(nullptr == chan)
        return false;

    return chan->currentGM() == player;
}

TreeItem* ChannelModel::getItemById(QString id) const
{
    for(auto& item : m_root)
    {
        if(nullptr != item)
        {
            if(item->uuid() == id)
            {
                return item;
            }
            else
            {
                TreeItem* child= item->getChildById(id);
                if(nullptr != child)
                {
                    return child;
                }
            }
        }
    }
    return nullptr;
}

ServerConnection* ChannelModel::getServerConnectionById(QString id) const
{
    ServerConnection* client= nullptr;
    for(auto& item : m_root)
    {
        if(nullptr == item)
            continue;

        if(!item->isLeaf())
        {
            auto channel= dynamic_cast<Channel*>(item);
            if(nullptr != channel)
            {
                client= channel->getClientById(id);
            }
        }

        if(nullptr != client)
            break;
    }
    return client;
}

void ChannelModel::removeChild(QString id)
{
    auto item= getItemById(id);
    if(nullptr == item) //&&(!item->isLeaf())
        return;

    auto parent= item->getParentItem();
    if(nullptr != parent)
    {
        Channel* channel= dynamic_cast<Channel*>(parent);
        if(nullptr != channel)
        {
            QModelIndex index= channelToIndex(channel);
            beginRemoveRows(index, channel->indexOf(item), channel->indexOf(item));
            channel->removeChild(item);
            endRemoveRows();
        }
    }
    else
    {
        QModelIndex index;
        beginRemoveRows(index, m_root.indexOf(item), m_root.indexOf(item));
        m_root.removeAll(item);
        endRemoveRows();
    }
}
void ChannelModel::cleanUp()
{
    beginResetModel();
    qDeleteAll(m_root);
    m_root.clear();
    endResetModel();
}
void ChannelModel::setChannelMemorySize(Channel* chan, quint64 size)
{
    if(m_shield)
        return;
    m_sizeMap[chan]= size;

    emit totalSizeChanged(computeTotalSize(m_sizeMap));
}

void ChannelModel::emptyChannelMemory()
{
    m_shield= true;
    for(auto pair : m_sizeMap)
    {
        QMetaObject::invokeMethod(pair.first, "clearData", Qt::QueuedConnection);
        pair.second= 0;
    }
    m_shield= false;
}
bool ChannelModel::localIsGM() const
{
    auto local= getServerConnectionById(m_localPlayerId);
    if(local == nullptr)
        return false;

    return local->isGM();
}
