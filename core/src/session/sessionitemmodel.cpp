/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "sessionitemmodel.h"

#include <QDebug>
#include <QFileInfo>
#include <QIcon>
#include <QUrl>

//////////////////////////////////////
/// SessionItemModel
/////////////////////////////////////
namespace session
{
SessionItemModel::SessionItemModel() : m_rootItem(new SessionItem())
{
    m_header << tr("Name") << tr("Path");

    m_rootItem->setParentNode(nullptr);
}

SessionItemModel::~SessionItemModel()= default;
QModelIndex SessionItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();

    SessionItem* parentItem= nullptr;

    if(!parent.isValid())
        parentItem= m_rootItem.get();
    else
        parentItem= static_cast<SessionItem*>(parent.internalPointer());

    SessionItem* childItem= parentItem->childAt(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
bool SessionItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    if(Qt::EditRole == role)
    {
        SessionItem* childItem= static_cast<SessionItem*>(index.internalPointer());
        childItem->setName(value.toString());
        return true;
    }
    return false;
}
Qt::ItemFlags SessionItemModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags | Qt::ItemIsDropEnabled;

    SessionItem* childItem= static_cast<SessionItem*>(index.internalPointer());
    if(!childItem->isLeaf())
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}
QStringList SessionItemModel::mimeTypes() const
{
    QStringList types;
    types << "rolisteam/cleverurilist"
          << "text/uri-list";
    return types;
}

QMimeData* SessionItemModel::mimeData(const QModelIndexList& indexes) const
{
    /*CleverUriMimeData* mimeData= new CleverUriMimeData();

    for(const QModelIndex& index : indexes)
    {
        if((index.isValid()) && (index.column() == 0))
        {
            ResourcesNode* item= static_cast<ResourcesNode*>(index.internalPointer());
            mimeData->addResourceNode(item, index);
        }
    }
    return mimeData;*/
}

/*void SessionItemModel::updateNode(ResourcesNode* node)
{
    QModelIndex nodeIndex;
    QModelIndex parent;
    QList<ResourcesNode*> path;
    if(m_rootItem->seekNode(path, node))
    {
        ResourcesNode* parentItem= nullptr;
        for(auto& i : path)
        {
            if(nullptr != parentItem)
            {
                nodeIndex= index(parentItem->indexOf(i), 0, parent);
                parent= nodeIndex;
            }
            parentItem= i;
        }
    }
    emit dataChanged(nodeIndex, nodeIndex);
}*/

Qt::DropActions SessionItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
bool SessionItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                                    const QModelIndex& parent)
{
    Q_UNUSED(column);

    if(action == Qt::IgnoreAction)
        return false;

    bool added= false;

    if(data->hasFormat("rolisteam/cleverurilist"))
    {
        // const CleverUriMimeData* mediaData= qobject_cast<const CleverUriMimeData*>(data);

        // if(nullptr != mediaData)
        {
            /*  QList<ResourcesNode*> mediaList= mediaData->getList().values();
              QList<QModelIndex> indexList= mediaData->getList().keys();
              {
                  if(action == Qt::MoveAction)
                  {
                      added= moveMediaItem(mediaList, parent, row, indexList);
                  }
              }*/
        }
    }
    else if((data->hasUrls()) && (!added))
    {
        QList<QUrl> list= data->urls();
        for(QUrl& url : list)
        {
            if(url.isLocalFile())
            {
                QFileInfo fileInfo(url.toLocalFile());
                if(fileInfo.isFile())
                {
                }
                // addChildMediaTo(medium,parent);
            }
            // else if(url.isLocalFile())
        }
    }
    return added;
}

bool SessionItemModel::moveMediaItem(QList<SessionItem*> items, const QModelIndex& parentToBe, int row,
                                     QList<QModelIndex>& formerPosition)
{
    SessionItem* parentItem= static_cast<SessionItem*>(parentToBe.internalPointer());

    if(nullptr == parentItem)
    {
        parentItem= m_rootItem.get();
    }
    int orignRow= row;

    QList<int> listRow;

    if((!items.isEmpty()) && (!formerPosition.isEmpty()))
    {
        SessionItem* item= items.at(0);
        SessionItem* parent= item->parentNode();
        QModelIndex formerPositionIndex= formerPosition.at(0);
        QModelIndex sourceParent= formerPositionIndex.parent();
        QModelIndex destinationParent= parentToBe;

        int sourceFirst= parent->indexOf(item);
        int sourceLast= parent->indexOf(item) + items.size() - 1;

        int destinationRow= orignRow < 0 ? parentItem->childrenCount() : orignRow;
        if((sourceParent == destinationParent)
           && ((destinationRow == parentItem->childrenCount()) || (destinationRow > sourceFirst)))
        {
            destinationRow-= items.size() - 1;
        }
        if((sourceParent == destinationParent) && (sourceFirst == destinationRow))
        {
            destinationRow-= items.size();
            return false;
        }

        if(!beginMoveRows(sourceParent, sourceFirst, sourceLast, destinationParent, destinationRow))
            return false;
    }

    for(int i= items.size() - 1; i >= 0; --i)
    {
        while(listRow.contains(row))
        {
            ++row;
        }

        SessionItem* item= items.at(i);
        SessionItem* parent= item->parentNode();
        QModelIndex formerPositionIndex= formerPosition.at(i);

        if(nullptr != parent)
        {
            parent->removeChild(item);
            if((orignRow == -1 && parentItem == m_rootItem.get()))
            {
                orignRow= parentItem->childrenCount();
                row= orignRow;
            }
            else if(formerPositionIndex.row() < orignRow && parentToBe == formerPositionIndex.parent())
            {
                orignRow-= 1;
                row= orignRow;
            }

            parentItem->insertChildAt(orignRow, item); // row
            //---
            // WARNING test move item and remove this code
            int oldRow= formerPositionIndex.row();
            if(oldRow > orignRow && parentItem == m_rootItem.get() && parent == m_rootItem.get())
            {
                oldRow+= items.size() - 1 - i;
            }
            listRow.append(row);
        }
    }

    endMoveRows();
    return true;
}
QModelIndex SessionItemModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    SessionItem* childItem= static_cast<SessionItem*>(index.internalPointer());
    SessionItem* parentItem= childItem->parentNode();

    if(parentItem == m_rootItem.get())
        return QModelIndex();

    SessionItem* grandParent= parentItem->parentNode();

    if(grandParent == nullptr)
        return QModelIndex();

    return createIndex(grandParent->indexOf(parentItem), 0, parentItem);
}
int SessionItemModel::rowCount(const QModelIndex& index) const
{
    if(index.isValid())
    {
        SessionItem* tmp= static_cast<SessionItem*>(index.internalPointer());
        return tmp->childrenCount();
    }
    else
    {
        return m_rootItem->childrenCount();
    }
}
int SessionItemModel::columnCount(const QModelIndex&) const
{
    return m_header.size();
}
QVariant SessionItemModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    SessionItem* tmp= static_cast<SessionItem*>(index.internalPointer());
    if(!tmp)
        return {};

    auto sessionData= tmp->data();

    QVariant var;
    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        var= sessionData->name();
        break;
    case Qt::DecorationRole:
        if(index.column() == Name)
        {
        }
        break;
    case Qt::BackgroundRole:
    {
        /*        if(tmp->type() != ResourcesNode::Cleveruri)
                    break;
                auto const& cleverUri= dynamic_cast<CleverURI*>(tmp);
                if(!cleverUri)
                    break;
                if(cleverUri->hasData() || cleverUri->exists() || cleverUri->contentType() ==
           Core::ContentType::WEBVIEW) break;

                var= QColor(Qt::red).lighter();*/
    }
    break;
    }
    return var;
}

void SessionItemModel::addResource(SessionItem* node, const QModelIndex& parent)
{
    if(!node)
        return;

    if(m_rootItem->contains(node->uuid()))
        return;

    SessionItem* parentItem= nullptr;
    auto parentbis= parent;
    if(!parent.isValid())
    {
        parentItem= m_rootItem.get();
    }
    else
    {
        SessionItem* node= static_cast<SessionItem*>(parent.internalPointer());
        if(node->isLeaf())
        {
            node= node->parentNode(); // leaf's parent is not a leaf indeed
            parentbis= parentbis.parent();
        }
        parentItem= dynamic_cast<SessionItem*>(node); // nullptr when it is not a chapter.
    }

    if(nullptr == parentItem)
        return;

    beginInsertRows(parentbis, parentItem->childrenCount(), parentItem->childrenCount());
    parentItem->insertChildAt(parentItem->childrenCount(), node);
    endInsertRows();
}

void SessionItemModel::remove(QModelIndex& index)
{
    if(!index.isValid())
        return;
    SessionItem* indexItem= static_cast<SessionItem*>(index.internalPointer());
    QModelIndex parent= index.parent();
    SessionItem* parentItem= nullptr;

    if(!parent.isValid())
        parentItem= m_rootItem.get();
    else
        parentItem= static_cast<SessionItem*>(parent.internalPointer());

    beginRemoveRows(index.parent(), index.row(), index.row());
    parentItem->removeChild(indexItem);
    endRemoveRows();
}

void SessionItemModel::removeNode(SessionItem* node)
{
    if(nullptr == node)
        return;

    auto parent= node->parentNode();
    if(parent == nullptr)
        return;

    auto idx= createIndex(parent->indexOf(node), 0, node);
    remove(idx);
}

void SessionItemModel::removeMedia(const QString& id)
{
    auto item= m_rootItem->find(id);
    // auto idx= createIndex(item->rowInParent(), 0, item);

    removeNode(item);
}

void SessionItemModel::addMedia(const QString& id, const QString& path, Core::ContentType type, const QString& name)
{
    auto uri= new SessionItem();
    auto data= uri->data();
    data->setUuid(id);
    data->setData(QVariant::fromValue(type));
    data->setName(name);
    data->setType(SessionItemType::Media);
    addResource(uri, QModelIndex());
}

void SessionItemModel::clearData()
{
    if(!m_rootItem)
        return;

    m_rootItem->clear();
}

QVariant SessionItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant var;
    if(orientation == Qt::Horizontal)
    {
        switch(role)
        {
        case Qt::DisplayRole:
            var= m_header.at(section);
            break;
        case Qt::TextAlignmentRole:
            var= Qt::AlignHCenter;
            break;
        default:
            break;
        }
    }
    return var;
}
////////////////////////
////////////////////////

SessionData::SessionData() {}

void SessionData::setUuid(const QString& uuid)
{
    m_uuid= uuid;
}

QString SessionData::uuid() const
{
    return m_uuid;
}

void SessionData::setName(const QString& name)
{
    m_name= name;
}

QString SessionData::name() const
{
    return m_name;
}

void SessionData::setPath(const QString& path)
{
    m_path= path;
}

QString SessionData::path() const
{
    return m_path;
}

void SessionData::setData(const QVariant& data)
{
    m_data= data;
}

QVariant SessionData::data() const
{
    return m_data;
}

SessionItemType SessionData::type() const
{
    return m_type;
}

void SessionData::setType(SessionItemType type)
{
    m_type= type;
}

////////////////////////
////////////////////////

SessionItem::SessionItem() : m_data(new SessionData) {}

bool SessionItem::isLeaf() const
{
    return (m_data->type() != Chapter);
}

int SessionItem::childrenCount() const
{
    return static_cast<int>(m_children.size());
}

SessionData* SessionItem::data() const
{
    return m_data.get();
}

int SessionItem::indexOf(SessionItem* item) const
{
    auto it= std::find_if(std::begin(m_children), std::end(m_children),
                          [item](const std::unique_ptr<SessionItem>& child) { return item == child.get(); });

    if(it == std::end(m_children))
        return -1;

    return std::distance(std::begin(m_children), it);
}

void SessionItem::insertChildAt(int row, SessionItem* item)
{
    std::unique_ptr<SessionItem> child(item);
    child->setParentNode(this);
    m_children.insert(m_children.begin() + row, std::move(child));
}

SessionItem* SessionItem::childAt(int i) const
{
    if(m_children.size() <= i && i < 0)
        return nullptr;

    auto it= m_children.begin() + i;
    return it->get();
}

void SessionItem::setParentNode(SessionItem* parent)
{
    m_parent= parent;
}

SessionItem* SessionItem::parentNode() const
{
    return m_parent;
}

void SessionItem::removeChild(SessionItem* item)
{
    auto it= std::find_if(std::begin(m_children), std::end(m_children),
                          [item](const std::unique_ptr<SessionItem>& child) { return item == child.get(); });

    if(it == std::end(m_children))
        return;

    m_children.erase(it);
}

void SessionItem::setName(const QString& name)
{
    if(m_data)
        m_data->setName(name);
}

bool SessionItem::contains(const QString& id) const
{
    auto it= std::find_if(std::begin(m_children), std::end(m_children),
                          [id](const std::unique_ptr<SessionItem>& child) { return id == child->uuid(); });

    return it != m_children.end();
}

QString SessionItem::uuid() const
{
    if(m_data)
        return m_data->uuid();

    return {};
}

void SessionItem::clear()
{
    m_children.clear();
}

SessionItem* SessionItem::find(const QString& id) const
{
    auto it= std::find_if(std::begin(m_children), std::end(m_children),
                          [id](const std::unique_ptr<SessionItem>& child) { return id == child->uuid(); });

    if(it != m_children.end())
        return it->get();

    SessionItem* item= nullptr;
    for(auto const& child : m_children)
    {
        item= child->find(id);
        if(item != nullptr)
            break;
    }
    return item;
}
} // namespace session
