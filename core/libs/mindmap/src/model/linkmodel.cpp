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
#include "linkmodel.h"

#include "data/mindnode.h"

#include <QRectF>

LinkModel::LinkModel(QObject* parent) : QAbstractItemModel(parent) {}

QModelIndex LinkModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    if(row < 0 || m_data.size() <= static_cast<std::size_t>(row))
        return QModelIndex();

    auto const link= m_data.at(static_cast<std::size_t>(row));

    return createIndex(row, column, link);
}

QModelIndex LinkModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return {};
}

int LinkModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_data.size());
}

int LinkModel::columnCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return 0;

    return 1;
}

QVariant LinkModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto link= m_data.at(static_cast<std::size_t>(index.row()));
    QVariant result;
    switch(role)
    {
    case Direction:
        result= link->direction();
        break;
    case Position:
        result= link->startPoint();
        break;
    case Last:
        result= link->endPoint();
        break;
    case Width:
        result= link->end()->position().x() - link->start()->position().x();
        break;
    case Height:
        result= link->end()->position().y() - link->start()->position().y();
        break;
    case LinkRole:
        result= QVariant::fromValue(link);
        break;
    case StartBoxRole:
        result= QVariant::fromValue(link->start()->boundingRect());
        break;
    case EndBoxRole:
        result= QVariant::fromValue(link->end()->boundingRect());
        break;
    case Label:
        result= link->text();
        break;
    }

    return result;
}

bool LinkModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool ret= false;
    if(index.isValid() && !index.parent().isValid())
    {
        // auto link= m_data.at(index.row());
        ret= true;
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
    }
    return ret;
}

QHash<int, QByteArray> LinkModel::roleNames() const
{
    // clang-format off
    static QHash<int, QByteArray> roles
        =  {{LinkModel::Direction, "direction"},
            {LinkModel::Position, "position"},
            {LinkModel::Last, "last"},
            {LinkModel::StartBoxRole,"startBoxRole"},
            {LinkModel::EndBoxRole,"endBoxRole"},
            {LinkModel::Height, "heightLink"},
            {LinkModel::Width, "widthLink"},
            {LinkModel::LinkRole, "link"},
            {LinkModel::Label, "label"}};
    //clang-format on
    return roles;
}

Link* LinkModel::addLink(MindNode* p1, MindNode* p2)
{
    if(nullptr == p1 || nullptr == p2)
        return nullptr;

    beginInsertRows(QModelIndex(), static_cast<int>(m_data.size()), static_cast<int>(m_data.size()));
    auto link= new Link();
    p2->setParentNode(p1);
    connect(link, &Link::linkChanged, this, &LinkModel::linkHasChanged);
    link->setStart(p1);
    p1->addLink(link);
    link->setEnd(p2);
    m_data.push_back(link);
    endInsertRows();
    return link;
}

void LinkModel::append(Link* link)
{
    if(link == nullptr)
        return;
    auto p1 = link->start();
    if(p1==nullptr)
        return;
    beginInsertRows(QModelIndex(), static_cast<int>(m_data.size()), static_cast<int>(m_data.size()));
    connect(link, &Link::linkChanged, this, &LinkModel::linkHasChanged);
    m_data.push_back(link);
    p1->addLink(link);
    endInsertRows();
}

void LinkModel::removeLink(Link* link)
{
    if(link == nullptr)
        return;
    auto p1 = link->start();
    if(p1==nullptr)
        return;

    auto id= std::find(m_data.begin(), m_data.end(), link);
    if(id == m_data.end())
        return;
    auto idx= static_cast<int>(std::distance(m_data.begin(), id));
    beginRemoveRows(QModelIndex(), idx, idx);
    m_data.erase(id);
    p1->removeLink(link);
    endRemoveRows();
}

void LinkModel::linkHasChanged()
{
    auto link= qobject_cast<Link*>(sender());
    QModelIndex parent;
    auto it= std::find(m_data.begin(), m_data.end(), link);
    auto offset= std::distance(m_data.begin(), it);
    auto idx1= index(offset, 0, parent);
    Q_EMIT dataChanged(idx1, idx1, QVector<int>() << Position << Last << Width << Height);
}

Qt::ItemFlags LinkModel::flags(const QModelIndex& index) const
{
    auto flags= QAbstractItemModel::flags(index);
    flags|= Qt::ItemIsEditable;
    return flags;
}

std::vector<Link*>& LinkModel::getDataSet()
{
    return m_data;
}
void LinkModel::openLinkAndChildren(const QString& id, bool status)
{
    std::vector<Link*> connectedLinks;
    std::copy_if(m_data.begin(), m_data.end(), std::back_inserter(connectedLinks),
                 [id](const Link* link) { return link->start()->id() == id; });

    std::for_each(connectedLinks.begin(), connectedLinks.end(), [status](Link* link) { link->setVisible(status); });
}

void LinkModel::clear()
{
    auto backup = m_data;
    beginResetModel();
    m_data.clear();
    endResetModel();
    qDeleteAll(backup);
}
