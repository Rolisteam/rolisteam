/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "participantmodel.h"

#include <QDebug>

#include "data/person.h"
#include "data/player.h"
#include "userlist/playermodel.h"

Player* findPlayer(const QString& id, PlayerModel* model)
{
    if(model == nullptr)
        return nullptr;

    return model->playerById(id);
}

ParticipantItem* findItem(const QString& id, ParticipantItem* root)
{
    ParticipantItem* parent= nullptr;
    for(int i= 0; i < root->childCount(); ++i)
    {
        auto item= root->childAt(i);
        for(int j= 0; j < item->childCount(); ++j)
        {
            auto child= item->childAt(j);
            if(nullptr == child)
                continue;
            if(child->player()->uuid() == id)
            {
                parent= item;
            }
        }
    }
    return parent;
}
ParticipantItem::ParticipantItem(const QString& name) : m_name(name) {}

ParticipantItem::ParticipantItem(Player* player) : m_player(player) {}

bool ParticipantItem::isLeaf() const
{
    return (m_player != nullptr);
}

QString ParticipantItem::name() const
{
    return isLeaf() ? m_player->name() : m_name;
}

Player* ParticipantItem::player() const
{
    return m_player;
}

int ParticipantItem::indexOf(ParticipantItem* child)
{
    return m_children.indexOf(child);
}

int ParticipantItem::childCount() const
{
    return m_children.size();
}

ParticipantItem* ParticipantItem::childAt(int pos)
{
    if(qBound(0, pos, m_children.size() - 1) != pos)
        return nullptr;
    return m_children.at(pos);
}

void ParticipantItem::appendChild(ParticipantItem* item)
{
    m_children.append(item);
    item->setParent(this);
}

void ParticipantItem::removeChild(ParticipantItem* item)
{
    m_children.removeOne(item);
}

ParticipantItem* ParticipantItem::parent() const
{
    return m_parent;
}

void ParticipantItem::setParent(ParticipantItem* parent)
{
    m_parent= parent;
}

ParticipantModel::ParticipantModel(PlayerModel* model, QObject* parent)
    : QAbstractItemModel(parent), m_playerList(model), m_root(new ParticipantItem(QString()))
{
    connect(model, &PlayerModel::playerJoin, this, &ParticipantModel::addNewPlayer);
    connect(model, &PlayerModel::playerLeft, this, &ParticipantModel::removePlayer);

    m_root->appendChild(new ParticipantItem(tr("Read Write")));
    m_root->appendChild(new ParticipantItem(tr("Read Only")));
    m_root->appendChild(new ParticipantItem(tr("Hidden")));

    initModel();
}

QVariant ParticipantModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

void ParticipantModel::initModel()
{
    if(m_ownerId.isEmpty())
        return;

    beginResetModel();
    auto c= m_playerList->rowCount();
    for(int i= 0; i < c; ++i)
    {
        auto idx= m_playerList->index(i, 0, QModelIndex());
        auto uuid= idx.data(PlayerModel::IdentifierRole).toString();
        auto player= dynamic_cast<Player*>(idx.data(PlayerModel::PersonPtrRole).value<Person*>());
        auto dest= m_root->childAt(hidden);
        if(uuid == m_ownerId)
            dest= m_root->childAt(readWrite);

        dest->appendChild(new ParticipantItem(player));
    }
    endResetModel();
}

QModelIndex ParticipantModel::index(int row, int column, const QModelIndex& parent) const
{
    if(!parent.isValid())
    {
        auto p= m_root->childAt(row);
        return createIndex(row, column, p);
    }
    else
    {

        auto p= m_root->childAt(parent.row());
        auto data= p->childAt(row);
        return createIndex(row, column, data);
    }
}

QModelIndex ParticipantModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    auto item= static_cast<ParticipantItem*>(index.internalPointer());

    if(item == nullptr)
        return {};

    if(!item->isLeaf())
        return QModelIndex();

    auto parentItem= item->parent();

    if(parentItem == nullptr)
        return {};

    return createIndex(m_root->indexOf(parentItem), 0, parentItem);
}

int ParticipantModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return m_root->childCount();

    auto item= static_cast<ParticipantItem*>(parent.internalPointer());

    if(item->isLeaf())
        return 0;

    return item->childCount();
}

int ParticipantModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant ParticipantModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    auto item= static_cast<ParticipantItem*>(index.internalPointer());

    return item->name();
}

Qt::ItemFlags ParticipantModel::flags(const QModelIndex& index) const
{
    if(index.parent().isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ParticipantModel::addNewPlayer(Player* player)
{
    if(nullptr == player)
        return;

    auto catInt= hidden;
    if(player->uuid() == m_ownerId)
        catInt= readWrite;
    auto permData= m_root->childAt(catInt);
    auto parent= createIndex(static_cast<int>(catInt), 0, permData);
    beginInsertRows(parent, permData->childCount(), permData->childCount());
    permData->appendChild(new ParticipantItem(player));
    endInsertRows();
}

void ParticipantModel::removePlayer(Player* player)
{
    if(nullptr == player)
        return;

    ParticipantItem* origin= nullptr;
    ParticipantItem* parent= nullptr;
    for(int i= 0; i < m_root->childCount(); ++i)
    {
        auto item= m_root->childAt(i);
        for(int j= 0; j < item->childCount(); ++j)
        {
            auto child= item->childAt(j);
            if(child->player() == player)
            {
                origin= child;
                parent= item;
            }
        }
    }

    if(origin == nullptr || parent == nullptr)
        return;

    auto pos= parent->indexOf(origin);
    auto parentIdx= createIndex(m_root->indexOf(parent), 0, parent);
    beginRemoveRows(parentIdx, pos, pos);
    parent->removeChild(origin);
    endRemoveRows();
}

int ParticipantModel::promotePlayer(const QModelIndex& index)
{
    ParticipantModel::Permission perm= permissionFor(index);
    switch(perm)
    {
    case readOnly:
    case hidden:
        setPlayerInto(index, static_cast<Permission>(perm - 1));
        return static_cast<Permission>(perm - 1);
    default:
        break;
    }
    return -1;
}

int ParticipantModel::demotePlayer(const QModelIndex& index)
{
    if(!index.isValid())
        return 0;

    ParticipantModel::Permission perm= permissionFor(index);
    switch(perm)
    {
    case readOnly:
    case readWrite:
        setPlayerInto(index, static_cast<Permission>(perm + 1));
        return static_cast<Permission>(perm + 1);
    default:
        break;
    }
    return -1;
}
QString ParticipantModel::getOwner() const
{
    return m_ownerId;
}

void ParticipantModel::setOwner(const QString& owner)
{
    if(owner == m_ownerId)
        return;
    m_ownerId= owner;
    initModel();
}
ParticipantModel::Permission ParticipantModel::permissionFor(const QModelIndex& index)
{
    if(!index.isValid())
        return hidden;

    auto parent= index.parent();
    if(!parent.isValid())
        return hidden;

    return static_cast<Permission>(parent.row());
}

ParticipantModel::Permission ParticipantModel::permissionFor(const QString& id)
{
    auto parent= findItem(id, m_root.get());
    auto val= m_root->indexOf(parent);

    if(val < 0)
        return hidden;

    return static_cast<ParticipantModel::Permission>(val);
}

void ParticipantModel::setPlayerInto(const QModelIndex& index, Permission level)
{
    QModelIndex parent= index.parent();
    if(!parent.isValid())
        return;

    auto item= static_cast<ParticipantItem*>(index.internalPointer());
    if(item == nullptr)
        return;

    QModelIndex destParent= createIndex(static_cast<int>(level), 0, m_root.get());

    auto r= rowCount(destParent);
    auto perm= m_root->childAt(level);
    auto permSource= static_cast<ParticipantItem*>(parent.internalPointer());

    QString id;
    auto p= item->player();
    if(nullptr != p)
        id= p->uuid();

    beginMoveRows(parent, index.row(), index.row(), destParent, r);
    permSource->removeChild(item);
    perm->appendChild(item);
    endMoveRows();

    auto hideRoot= m_root->childAt(hidden);
    auto roRoot= m_root->childAt(readOnly);
    auto rwRoot= m_root->childAt(readWrite);

    if(permSource == hideRoot && perm == roRoot)
        userReadPermissionChanged(id, true);
    else if(permSource == roRoot && perm == hideRoot)
        userReadPermissionChanged(id, false);

    if(permSource == roRoot && perm == rwRoot)
        userWritePermissionChanged(id, true);
    else if(permSource == rwRoot && perm == roRoot)
        userWritePermissionChanged(id, false);
}

void ParticipantModel::setPlayerPermission(const QString& id, ParticipantModel::Permission level)
{
    auto parent= findItem(id, m_root.get());
    auto idx= index(m_root->indexOf(parent), 0, QModelIndex());
    setPlayerInto(idx, level);
}

void ParticipantModel::saveModel(QJsonObject& root)
{
    /*   QJsonArray hidden;
       QJsonArray readOnly;
       QJsonArray readWrite;

    for(auto& player : m_hidden)
    {
        hidden << player->getUuid();
    }

    for(auto& player : m_readOnly)
    {
        readOnly << player->getUuid();
    }

    for(auto& player : m_readWrite)
    {
        readWrite << player->getUuid();
    }

    root["hidden"]= hidden;
    root["readOnly"]= readOnly;
    root["readWrite"]= readWrite;*/
}
void ParticipantModel::loadModel(QJsonObject& root)
{
    /*PlayerModel* playerList= PlayerModel::instance();

    QJsonArray hidden= root["hidden"].toArray();
    QJsonArray readOnly= root["readOnly"].toArray();
    QJsonArray readWrite= root["readWrite"].toArray();

    beginResetModel();

    m_hidden.clear();
    m_readOnly.clear();
    m_readWrite.clear();

    for(auto playerId : hidden)
    {
        Player* p= dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_hidden.append(p);
    }

    for(auto playerId : readOnly)
    {
        Player* p= dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_readOnly.append(p);
    }

    for(auto playerId : readWrite)
    {
        Player* p= dynamic_cast<Player*>(playerList->getPerson(playerId.toString()));
        m_readWrite.append(p);
    }
    endResetModel();*/
}
