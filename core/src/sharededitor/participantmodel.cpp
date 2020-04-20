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

#include "data/person.h"
#include "data/player.h"
#include "userlist/playermodel.h"

Player* findPlayer(const QString& id, PlayerModel* model)
{
    if(model == nullptr)
        return nullptr;

    return model->playerById(id);
}

ParticipantModel::ParticipantModel(PlayerModel* model, QObject* parent)
    : QAbstractItemModel(parent), m_playerList(model)
{
    connect(model, &PlayerModel::playerJoin, this, &ParticipantModel::addNewPlayer);
    connect(model, &PlayerModel::playerLeft, this, &ParticipantModel::removePlayer);

    m_data.insert({readWrite, new PermissionData({tr("Read Write"), QStringList()})});
    m_data.insert({readOnly, new PermissionData({tr("Read Only"), QStringList()})});
    m_data.insert({hidden, new PermissionData({tr("Hidden"), QStringList()})});

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
    for(int i= 0; i < m_playerList->rowCount(); ++i)
    {
        auto idx= m_playerList->index(i, 0, QModelIndex());
        auto uuid= idx.data(PlayerModel::IdentifierRole).toString();
        if(uuid == m_ownerId)
            m_data[readWrite]->m_ids.append(uuid);
        else
            m_data[hidden]->m_ids.append(uuid);
    }
    endResetModel();
}

QModelIndex ParticipantModel::index(int row, int column, const QModelIndex& parent) const
{
    if(!parent.isValid() && (m_data.find(static_cast<Permission>(row)) != m_data.end()))
    {
        auto permData= m_data.at(static_cast<Permission>(row));
        return createIndex(row, column, permData);
    }
    else if(parent.isValid() && (m_data.find(static_cast<Permission>(parent.row())) != m_data.end()))
    {
        auto permData= m_data.at(static_cast<Permission>(parent.row()));
        if(permData->m_ids.size() > row)
        {
            auto id= permData->m_ids.at(row);
            auto player= m_playerList->playerById(id);
            if(player)
                return createIndex(row, column, player);
        }
    }
    return QModelIndex();
}

QModelIndex ParticipantModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    auto perm= static_cast<PermissionData*>(index.internalPointer());
    auto it= std::find_if(m_data.begin(), m_data.end(),
                          [perm](const std::pair<Permission, PermissionData*>& data) { return perm == data.second; });
    if(it != m_data.end())
    {
        return QModelIndex();
    }

    auto person= static_cast<Person*>(index.internalPointer());
    auto player= dynamic_cast<Player*>(person);
    if(nullptr != player)
    {
        auto id= player->uuid();
        auto it= std::find_if(m_data.begin(), m_data.end(), [id](const std::pair<Permission, PermissionData*>& data) {
            return data.second->m_ids.contains(id);
        });

        if(it != m_data.end())
        {
            auto r= it->second->m_ids.indexOf(id);
            return createIndex(r, 0, it->second);
        }
    }

    return QModelIndex();
}

int ParticipantModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return static_cast<int>(m_data.size());

    auto perm= static_cast<PermissionData*>(parent.internalPointer());
    auto it= std::find_if(m_data.begin(), m_data.end(),
                          [perm](const std::pair<Permission, PermissionData*>& data) { return perm == data.second; });
    if(it == m_data.end())
    {
        return 0;
    }

    return perm->m_ids.size();
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

    auto perm= static_cast<PermissionData*>(index.internalPointer());
    auto it= std::find_if(m_data.begin(), m_data.end(),
                          [perm](const std::pair<Permission, PermissionData*>& data) { return perm == data.second; });
    if(it != m_data.end())
    {
        return it->second->m_name;
    }

    auto person= static_cast<Person*>(index.internalPointer());
    auto player= dynamic_cast<Player*>(person);

    if(nullptr != player)
        player->name();

    return QVariant();
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

    auto hiddenInt= static_cast<int>(hidden);
    auto permData= m_data.at(hidden);
    auto parent= createIndex(hiddenInt, 0, permData);
    beginInsertRows(parent, permData->m_ids.size(), permData->m_ids.size());
    permData->m_ids.append(player->uuid());
    endInsertRows();
}

void ParticipantModel::removePlayer(Player* player)
{
    if(nullptr == player)
        return;

    auto id= player->uuid();

    auto it= std::find_if(m_data.begin(), m_data.end(), [id](const std::pair<Permission, PermissionData*>& data) {
        return data.second->m_ids.contains(id);
    });

    if(it == m_data.end())
        return;

    auto pos= it->second->m_ids.indexOf(id);
    auto parent= createIndex(static_cast<int>(it->first), 0, it->second);

    beginRemoveRows(parent, pos, pos);
    it->second->m_ids.removeAll(id);
    endRemoveRows();
}

int ParticipantModel::promotePlayer(const QModelIndex& index)
{
    ParticipantModel::Permission perm= getPermissionFor(index);
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

    ParticipantModel::Permission perm= getPermissionFor(index);
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
ParticipantModel::Permission ParticipantModel::getPermissionFor(const QModelIndex& index)
{
    if(!index.isValid())
        return hidden;

    auto parent= index.parent();
    if(!parent.isValid())
        return hidden;

    return static_cast<Permission>(index.row());
}

ParticipantModel::Permission ParticipantModel::getPermissionFor(const QString& id)
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [id](const std::pair<Permission, PermissionData*>& data) {
        return data.second->m_ids.contains(id);
    });

    if(it == m_data.end())
        return hidden;

    return it->first;
}

void ParticipantModel::setPlayerInto(const QModelIndex& index, Permission level)
{
    QModelIndex parent= index.parent();
    if(!parent.isValid())
        return;

    auto person= static_cast<Person*>(index.internalPointer());
    auto player= dynamic_cast<Player*>(person);
    if(player == nullptr)
        return;

    QModelIndex destParent= createIndex(static_cast<int>(level), 0);
    /*auto sourceRow= static_cast<Permission>(parent.row());
    auto destRow= static_cast<Permission>(destParent.row());
    if(destParent == parent)
    {
        return;
    }*/

    auto r= rowCount(destParent);
    auto perm= m_data.at(level);
    auto permSource= static_cast<PermissionData*>(parent.internalPointer());

    auto it
        = std::find_if(m_data.begin(), m_data.end(), [permSource](const std::pair<Permission, PermissionData*>& data) {
              return data.second == permSource;
          });

    if(it == m_data.end())
        return;

    beginMoveRows(parent, index.row(), index.row(), destParent, r);
    it->second->m_ids.removeAll(player->uuid());
    perm->m_ids.append(player->uuid());
    endMoveRows();
}

void ParticipantModel::setPlayerPermission(const QString& id, ParticipantModel::Permission level)
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [id](const std::pair<Permission, PermissionData*>& data) {
        return data.second->m_ids.contains(id);
    });
    if(it == m_data.end())
        return;

    auto parent= index(it->first, 0, QModelIndex());
    auto idx= index(it->second->m_ids.indexOf(id), 0, parent);

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
