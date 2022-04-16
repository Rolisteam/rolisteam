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

#include "data/link.h"
#include "data/mindnode.h"

#include <QDebug>
#include <QRectF>

namespace mindmap
{
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

Link* LinkModel::linkFromId(const QString& id) const
{
    if(id.isEmpty())
        return nullptr;

    auto it= std::find_if(m_data.begin(), m_data.end(), [id](const Link* link) { return link->id() == id; });
    if(it == m_data.end())
        return nullptr;
    return (*it);
}

QVariant LinkModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto link= m_data.at(static_cast<std::size_t>(index.row()));
    if(!link)
        return {};

    QVariant result;
    switch(role)
    {
    case Direction:
        result= QVariant::fromValue(link->direction());
        break;
    case P1Position:
        result= link->startPoint();
        break;
    case P2Position:
        result= link->endPoint();
        break;
    case Width:
        result= link->p2().x() - link->p1().x();
        break;
    case Height:
        result= link->p2().y() - link->p1().y();
        break;
    case LinkRole:
        result= QVariant::fromValue(link);
        break;
    case StartNodeId:
        result= link->p1Id();
        break;
    case EndNodeId:
        result= link->p2Id();
        break;
        /*    case StartBoxRole:
                result= QVariant::fromValue(link->start()->boundingRect());
                break;
            case EndBoxRole:
            {
                auto end= link->endNode();
                result= end ? QVariant::fromValue(end->boundingRect()) : QVariant();
            }*/
        break;
    case StartPointRole:
        result= QVariant::fromValue(link->p1());
        break;
    case EndPointRole:
        result= QVariant::fromValue(link->p2());
        break;
    case Label:
        result= link->text();
        break;
    case Visibility:
        result= link->isVisible();
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
        qDebug() << "set data" << index << value;
        emit dataChanged(index, index, QVector<int>() << role);
    }
    return ret;
}

QHash<int, QByteArray> LinkModel::roleNames() const
{
    // clang-format off
    static QHash<int, QByteArray> roles
        =  {{LinkModel::Direction, "direction"},
            {LinkModel::P1Position, "position"},
            {LinkModel::P2Position, "last"},
           // {LinkModel::StartBoxRole,"startBoxRole"},
           // {LinkModel::EndBoxRole,"endBoxRole"},
            {LinkModel::StartPointRole,"startPoint"},
            {LinkModel::EndPointRole,"endPoint"},
            {LinkModel::Height, "heightLink"},
            {LinkModel::Width, "widthLink"},
            {LinkModel::LinkRole, "link"},
            {LinkModel::Visibility, "visible"},
            {LinkModel::Label, "label"}};
    //clang-format on
    return roles;
}

void LinkModel::attachLinkSignal(Link* link)
{
    connect(link, &Link::visibleChanged, this, [this, link](){
        linkHasChanged(link, {Visibility});
    });
    connect(link, &Link::directionChanged, this, [this, link](){
        linkHasChanged(link, {Direction});
    });
    connect(link, &Link::startPositionChanged, this, [this, link](){
        linkHasChanged(link, {P1Position, StartPointRole, Height, Width});
    });
    connect(link, &Link::endPositionChanged, this, [this, link](){
        linkHasChanged(link, {P2Position, EndPointRole, Height, Width});
    });
    connect(link, &Link::textChanged, this, [this, link](){
        linkHasChanged(link, {Label});
    });
    connect(link, &Link::startPointChanged, this, [this, link](){
        linkHasChanged(link, {P1Position, StartNodeId, StartPointRole, Height, Width});
    });
    connect(link, &Link::endPointChanged, this, [this, link](){
        linkHasChanged(link, {P2Position, EndNodeId, EndPointRole, Height, Width});
    });


}

Link* LinkModel::addLink(MindNode* p1, MindNode* p2)
{
    if(nullptr == p1 || nullptr == p2)
        return nullptr;

    beginInsertRows(QModelIndex(), static_cast<int>(m_data.size()), static_cast<int>(m_data.size()));
    auto link= new Link();
    p2->setParentNode(p1);
    attachLinkSignal(link);
    link->setStart(p1);
    p1->addLink(link);
    link->setEnd(p2);
    m_data.push_back(link);
    endInsertRows();
    return link;
}

void LinkModel::append(const QList<Link*>& links, bool network)
{
    QList<Link*> realLinks;
    for(auto link : links)
    {
        auto it = std::find(std::begin(m_data), std::end(m_data), link);
        if(link == nullptr || it != std::end(m_data))
            continue;
        auto p1 = link->start();
        auto p2 = link->endNode();
        if(p1 == nullptr || p2 == nullptr)
            continue;



        attachLinkSignal(link);
        beginInsertRows(QModelIndex(), static_cast<int>(m_data.size()), static_cast<int>(m_data.size()));
        p2->setParentNode(p1);
        p1->addLink(link);
        m_data.push_back(link);
        endInsertRows();
        realLinks << link;
    }
}

void LinkModel::removeLink(const QStringList& ids, bool network)
{
    QStringList realIds;
    for(auto const& id : qAsConst(ids))
    {
        auto link = linkFromId(id);
        if(link == nullptr)
            continue;
        auto p1 = link->start();
        if(p1==nullptr)
            continue;

        auto it= std::find(m_data.begin(), m_data.end(), link);
        if(it == m_data.end())
            continue;
        auto idx= static_cast<int>(std::distance(m_data.begin(), it));
        beginRemoveRows(QModelIndex(), idx, idx);
        m_data.erase(it);
        p1->removeLink(link);
        endRemoveRows();
        realIds << id;
        //emit link->linkChanged({Link::P1_POSITION});
    }
}

void LinkModel::linkHasChanged(Link* link,QVector<int> roles)
{
    if(!link)
        return;

    QModelIndex parent;
    auto it= std::find(m_data.begin(), m_data.end(), link);
    auto offset= std::distance(m_data.begin(), it);
    auto idx1= index(offset, 0, parent);
    emit dataChanged(idx1, idx1, roles);
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
                 [id](const Link* link) { return link->p1Id() == id; });

    std::for_each(connectedLinks.begin(), connectedLinks.end(), [status](Link* link) { link->setVisible(status); });
}

QList<Link*> LinkModel::allLinkWithNodeId(const QString& id)
{
  QList<Link*> res;
    std::copy_if(m_data.begin(), m_data.end(), std::back_inserter(res),
                 [id](const Link* link) { return (link->p1Id() == id || link->p2Id() == id); });

  return res;
}

void LinkModel::clear()
{
    auto backup = m_data;
    beginResetModel();
    m_data.clear();
    endResetModel();
    qDeleteAll(backup);
}
}
