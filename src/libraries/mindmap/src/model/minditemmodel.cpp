/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "minditemmodel.h"

#include <QColor>
#include <QDebug>
#include <QRectF>

#include "mindmap/data/linkcontroller.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/nodeimageprovider.h"

namespace mindmap
{
std::tuple<std::vector<MindItem*>&, int> getVector(std::vector<MindItem*>& links, std::vector<MindItem*>& package,
                                                   std::vector<MindItem*>& node, MindItem::Type type)
{

    if(type == MindItem::LinkType)
        return {links, 0};
    else if(type == MindItem::PackageType)
        return {package, links.size()};
    else // if(type == MindItem::NodeType)
        return {node, links.size() + package.size()};
}

MindItem* itemFromIndex(int r, const std::vector<MindItem*>& links, const std::vector<MindItem*>& packages,
                        const std::vector<MindItem*>& nodes)
{
    auto linkCount= static_cast<int>(links.size());
    auto packageCount= static_cast<int>(packages.size());
    auto nodeCount= static_cast<int>(nodes.size());

    MindItem* mindNode= nullptr;

    if(r < linkCount)
    {
        mindNode= links[r];
    }
    else
    {
        r-= linkCount;
        if(r < packageCount)
            mindNode= packages[r];
        else
        {
            r-= packageCount;
            if(r < nodeCount)
                mindNode= nodes[r];
        }
    }
    return mindNode;
}

MindItemModel::MindItemModel(ImageModel* imgModel, QObject* parent) : QAbstractListModel(parent), m_imgModel(imgModel)
{
}

MindItemModel::~MindItemModel() {}

int MindItemModel::rowCount(const QModelIndex& parent) const
{
    // qDebug() << "rowCount :";
    if(parent.isValid())
        return 0;
    // qDebug() << "links" << m_links.size() << "node:" << m_nodes.size();
    return static_cast<int>(m_links.size() + m_packages.size() + m_nodes.size());
}

QVariant MindItemModel::data(const QModelIndex& index, int role) const
{

    if(!index.isValid())
        return QVariant();

    QVariant result;

    auto r= index.row();
    auto mindNode= itemFromIndex(r, m_links, m_packages, m_nodes);

    if(!mindNode)
        return {};

    if(role == Qt::DisplayRole)
        role= Label;

    QSet<int> allowedRole{Visible, Label, Selected, Type, Uuid, Object, HasPicture};

    if(!allowedRole.contains(role))
        return {};

    switch(role)
    {
    case Visible:
        result= mindNode->isVisible();
        break;
    case Label:
        result= mindNode->text().isEmpty() ? QString("Node") : mindNode->text();
        break;
    case Selected:
        result= mindNode->selected();
        break;
    case Type:
        result= mindNode->type();
        break;
    case Uuid:
        result= mindNode->id();
        break;
    case Object:
        result= QVariant::fromValue(mindNode);
        break;
    case HasPicture:
        result= mindNode->type() == MindItem::NodeType ? m_imgModel->hasPixmap(mindNode->id()) : false;
        break;
    }
    return result;
}

bool MindItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(data(index, role) != value)
    {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

void MindItemModel::update(const QString& id, int role)
{
    auto current= item(id);

    if(!current)
        return;

    int row= 0;
    if(current->type() == MindItem::LinkType)
    {
        auto it= std::find(std::begin(m_links), std::end(m_links), current);
        if(it != std::end(m_links))
            row= std::distance(std::begin(m_links), it);
    }
    else if(current->type() == MindItem::PackageType)
    {
        auto it= std::find(std::begin(m_packages), std::end(m_packages), current);
        if(it != std::end(m_packages))
            row= std::distance(std::begin(m_packages), it) + m_links.size();
    }
    else if(current->type() == MindItem::NodeType)
    {
        auto it= std::find(std::begin(m_nodes), std::end(m_nodes), current);
        if(it != std::end(m_nodes))
            row= std::distance(std::begin(m_nodes), it) + m_links.size() + m_packages.size();
    }

    emit dataChanged(index(row, 0), index(row, 0), {role});
}

void MindItemModel::setImageUriToNode(const QString& id)
{
    auto it= std::find_if(m_nodes.begin(), m_nodes.end(), [id](const MindItem* node) { return node->id() == id; });
    if(it == m_nodes.end())
        return;

    auto dis= std::distance(m_nodes.begin(), it);
    auto node= dynamic_cast<MindNode*>(*it);
    if(!node)
        return;

    auto idx= index(dis, 0, QModelIndex());
    emit dataChanged(idx, idx, QVector<int>());
}

Qt::ItemFlags MindItemModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QHash<int, QByteArray> MindItemModel::roleNames() const
{
    static QHash<int, QByteArray> roles= {{MindItemModel::Label, "label"},
                                          {MindItemModel::Visible, "isVisible"},
                                          {MindItemModel::Selected, "isSelected"},
                                          {MindItemModel::Type, "type"},
                                          {MindItemModel::Uuid, "id"},
                                          {MindItemModel::Object, "objectItem"},
                                          {MindItemModel::HasPicture, "hasPicture"}};
    return roles;
}

void MindItemModel::clear()
{

    QVector<MindItem*> backup;
    backup.reserve(rowCount());
    for(auto const& l : m_links)
        backup.append(l);

    for(auto const& l : m_packages)
        backup.append(l);

    for(auto const& l : m_nodes)
        backup.append(l);

    beginResetModel();
    m_links.clear();
    m_packages.clear();
    m_nodes.clear();
    endResetModel();
}

void MindItemModel::appendItem(const QList<MindItem*>& nodes, bool network)
{
    for(auto const& node : nodes)
    {
        if(node == nullptr)
            return;

        auto [vec, offset]= getVector(m_links, m_packages, m_nodes, node->type());

        int row= offset + vec.size();

        if(node->type() == MindItem::LinkType)
        {

            auto link= dynamic_cast<mindmap::LinkController*>(node);
            if(link)
            {
                connect(link, &mindmap::LinkController::startPointChanged, this,
                        [this, link]()
                        {
                            QModelIndex parent;
                            auto it= std::find(m_links.begin(), m_links.end(), link);
                            if(it == m_links.end())
                                return;
                            auto offset= std::distance(m_links.begin(), it);
                            auto idx1= index(offset, 0, parent);
                            auto start= link->start();
                            emit dataChanged(idx1, idx1,
                                             start->isDragged() ? QVector<int>{Object, LinkStartPosition} :
                                                                  QVector<int>{Object, LinkPositionFromSpacing});
                        });
            }
        }
        else
        {
            auto pItem= dynamic_cast<mindmap::PositionedItem*>(node);
            if(pItem)
            {
                connect(pItem, &mindmap::PositionedItem::positionChanged, this, &MindItemModel::geometryChanged);
                connect(pItem, &mindmap::PositionedItem::textChanged, this, &MindItemModel::geometryChanged);
                connect(pItem, &mindmap::PositionedItem::textChanged, this,
                        [pItem, this]()
                        {
                            auto [vec, offset]= getVector(m_links, m_packages, m_nodes, pItem->type());
                            auto row= offset + static_cast<int>(vec.size());
                            auto idx= index(row, 0);
                            emit dataChanged(idx, idx, {Roles::Label});
                        });
                connect(pItem, &mindmap::PositionedItem::widthChanged, this, &MindItemModel::geometryChanged);
                connect(pItem, &mindmap::PositionedItem::heightChanged, this, &MindItemModel::geometryChanged);
            }
        }

        beginInsertRows(QModelIndex(), row, row);
        vec.push_back(node);
        endInsertRows();
    }
    if(!network)
        emit itemAdded(nodes);
}
std::vector<PositionedItem*> MindItemModel::positionnedItems() const
{
    std::vector<PositionedItem*> vec;
    vec.reserve(m_packages.size() + m_nodes.size());

    for(auto const& item : m_packages)
    {
        auto pack= dynamic_cast<PositionedItem*>(item);

        if(!pack)
            continue;

        vec.push_back(pack);
    }

    for(auto const& item : m_nodes)
    {
        auto pack= dynamic_cast<PositionedItem*>(item);

        if(!pack)
            continue;

        vec.push_back(pack);
    }

    return vec;
}

MindItem* MindItemModel::createItem(MindItem::Type type)
{
    MindItem* result= nullptr;

    switch(type)
    {
    case MindItem::NodeType:
    {
        auto root= new MindNode();
        root->setStyleIndex(defaultStyleIndex());
        result= root;
    }
    break;
    case MindItem::PackageType:
    {
        auto pack= new PackageNode();
        emit latestInsertedPackage(pack);
        result= pack;
    }
    break;
    case MindItem::LinkType:
    {
        result= new LinkController();
    }
    break;
    case mindmap::MindItem::InvalidType:
        break;
    }

    return result;
}

void MindItemModel::removeAllSubItem(const mindmap::PositionedItem* item, QSet<QString>& items)
{
    QList<const mindmap::MindItem*> links{};

    std::for_each(std::begin(m_links), std::end(m_links),
                  [&links, item, this](const mindmap::MindItem* tmp)
                  {
                      auto const link= dynamic_cast<const mindmap::LinkController*>(tmp);
                      Q_ASSERT(link);
                      if(link->relatedTo(item->id()))
                      {
                          disconnect(link, 0, this, 0);
                          links << link;
                      }
                  });

    std::for_each(std::begin(links), std::end(links),
                  [this, &items](const mindmap::MindItem* tmp)
                  {
                      items << tmp->id();
                      auto it= std::find(std::begin(m_links), std::end(m_links), tmp);

                      if(it == std::end(m_links))
                          return;

                      auto idx= std::distance(std::begin(m_links), it);
                      beginRemoveRows(QModelIndex(), idx, idx);
                      m_links.erase(it);
                      endRemoveRows();
                  });

    std::for_each(std::begin(m_nodes), std::end(m_nodes),
                  [item](mindmap::MindItem* tmp)
                  {
                      auto pItem= dynamic_cast<PositionedItem*>(tmp);

                      if(!pItem)
                          return;

                      if(pItem->parentId() == item->id())
                      {
                          pItem->setParentNode(nullptr); // remove parent
                      }
                  });
}

bool MindItemModel::removeItem(const MindItem* item)
{
    if(item == nullptr)
        return false;

    QSet<QString> items{item->id()};
    auto [vec, offset]= getVector(m_links, m_packages, m_nodes, item->type());

    auto it= std::find(vec.begin(), vec.end(), item);

    if(it == std::end(m_nodes))
        return false;

    auto idx= offset + static_cast<int>(std::distance(vec.begin(), it));

    if(item->type() == MindItem::LinkType)
    {
        auto const link= dynamic_cast<const mindmap::LinkController*>(item);
        if(link)
        {
            disconnect(link, 0, this, 0);
        }
    }
    else if(item->type() == MindItem::NodeType)
    {
        // removeAllSubItem(dynamic_cast<const mindmap::PositionedItem*>(item), items);
    }
    else if(item->type() == MindItem::PackageType)
    {
        removeAllSubItem(dynamic_cast<const mindmap::PositionedItem*>(item), items);
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    vec.erase(it);
    endRemoveRows();

    emit itemRemoved(items.values());

    return true;
}

void MindItemModel::openItem(const QString& id, bool status)
{
    auto it= item(id);

    if(nullptr == it)
        return;

    auto node= dynamic_cast<PositionedItem*>(it);

    if(nullptr == node)
        return;

    if(node->open() == status)
        return;

    node->setOpen(status);
}

MindItem* MindItemModel::item(const QString& id) const
{
    // MindItem* result= nullptr;

    auto it= std::find_if(m_nodes.begin(), m_nodes.end(), [id](const MindItem* node) { return node->id() == id; });

    if(it != m_nodes.end())
        return *it;

    auto it2= std::find_if(m_links.begin(), m_links.end(), [id](const MindItem* node) { return node->id() == id; });

    if(it2 != m_links.end())
        return *it2;

    auto it3
        = std::find_if(m_packages.begin(), m_packages.end(), [id](const MindItem* node) { return node->id() == id; });

    if(it3 != m_packages.end())
        return *it3;

    return nullptr;
}

PositionedItem* MindItemModel::positionItem(const QString& id) const
{
    auto it= std::find_if(m_nodes.begin(), m_nodes.end(), [id](const MindItem* node) { return node->id() == id; });

    if(it != m_nodes.end())
        return dynamic_cast<PositionedItem*>(*it);

    auto it3
        = std::find_if(m_packages.begin(), m_packages.end(), [id](const MindItem* node) { return node->id() == id; });

    if(it3 != m_packages.end())
        return dynamic_cast<PositionedItem*>(*it3);

    return nullptr;
}

QRectF MindItemModel::contentRect() const
{
    QRectF rect(0., 0., 1., 1.);

    for(auto const& item : m_nodes)
    {
        auto node= dynamic_cast<PositionedItem*>(item);
        if(!node)
            continue;
        rect= node->boundingRect().united(rect);
    }

    for(auto const& item : m_packages)
    {
        auto pack= dynamic_cast<PositionedItem*>(item);
        if(!pack)
            continue;
        rect= pack->boundingRect().united(rect);
    }
    return rect;
}

std::vector<MindItem*>& MindItemModel::items(MindItem::Type type)
{
    return std::get<0>(getVector(m_links, m_packages, m_nodes, type));
}

std::vector<LinkController*> MindItemModel::sublink(const QString& id) const
{
    std::vector<LinkController*> vec;

    for(auto item : m_links)
    {
        auto link= dynamic_cast<LinkController*>(item);
        if(!link)
            continue;

        auto st= link->start();
        if(!st)
            continue;

        if(id == st->id() && (std::end(vec) == std::find(std::begin(vec), std::end(vec), link)))
            vec.push_back(link);

        auto end= link->end();
        if(!end)
            continue;

        if(id == end->id() && (std::end(vec) == std::find(std::begin(vec), std::end(vec), link)))
            vec.push_back(link);
    }

    return vec;
}

QString MindItemModel::idFromIndex(int index) const
{
    auto mindNode= itemFromIndex(index, m_links, m_packages, m_nodes);

    if(!mindNode)
        return {};

    return mindNode->id();
}

int MindItemModel::defaultStyleIndex() const
{
    return m_defaultStyleIndex;
}

void MindItemModel::setDefaultStyleIndex(int newDefaultStyleIndex)
{
    if(m_defaultStyleIndex == newDefaultStyleIndex)
        return;
    m_defaultStyleIndex= newDefaultStyleIndex;
    emit defaultStyleIndexChanged();
}

} // namespace mindmap
