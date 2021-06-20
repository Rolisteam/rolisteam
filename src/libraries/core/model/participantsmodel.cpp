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
#include "participantsmodel.h"

#include "data/player.h"
#include "userlist/playermodel.h"
#include <QJsonArray>

ParticipantsModel::Permission permissionFor(const QModelIndex& index)
{
    return static_cast<ParticipantsModel::Permission>(index.parent().row());
}

ParticipantsModel::ParticipantsModel()
{
    m_permissionGroup << tr("Read Write") << tr("Read Only") << tr("Hidden");
}

int ParticipantsModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return Permission_count;

    if(parent.row() == ReadWrite)
        return m_readWrite.size();

    if(parent.row() == ReadOnly)
        return m_readOnly.size();

    auto total= sourceModel()->rowCount(QModelIndex());
    return total - (m_readOnly.size() + m_readWrite.size());
}

int ParticipantsModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return 1;
}

QVariant ParticipantsModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || Qt::DisplayRole != role)
        return QVariant();

    QModelIndex parent= index.parent();
    if(!parent.isValid())
        return m_permissionGroup.at(index.row());

    auto mapIndex= mapToSource(index);

    if(index.isValid())
        return mapIndex.data(role);

    return {};
}

QModelIndex ParticipantsModel::parent(const QModelIndex& child) const
{
    if(!child.isValid())
        return QModelIndex();

    Player* childItem= static_cast<Player*>(child.internalPointer());

    if(nullptr == childItem)
        return QModelIndex();

    auto uuid= childItem->uuid();
    if(m_readWrite.contains(uuid))
        return createIndex(ReadWrite, 0, nullptr);

    if(m_readOnly.contains(uuid))
        return createIndex(ReadOnly, 0, nullptr);

    return createIndex(Hidden, 0, nullptr);
}

QModelIndex ParticipantsModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();

    if(!parent.isValid())
        return createIndex(row, column, nullptr); // internalPointer is null for category

    auto cat= parent.row();
    QString uuid;
    if(cat == ReadWrite)
        uuid= m_readWrite[row];

    if(cat == ReadOnly)
        uuid= m_readOnly[row];

    Player* player= nullptr;
    if(uuid.isNull())
    {
        auto tmpRow= row;
        for(int i= 0; i < sourceModel()->rowCount() && tmpRow >= 0; ++i)
        {
            auto idx= sourceModel()->index(i, 0);
            auto person= idx.data(PlayerModel::PersonPtrRole).value<Person*>();

            auto uuid= person->uuid();

            if(m_readWrite.contains(uuid) || m_readOnly.contains(uuid))
                continue;

            if(tmpRow == 0)
            {
                player= dynamic_cast<Player*>(person);
            }
            --tmpRow;
        }
    }
    else
    {
        for(int i= 0; i < sourceModel()->rowCount() && player == nullptr; ++i)
        {
            auto idx= sourceModel()->index(i, 0);
            auto person= idx.data(PlayerModel::PersonPtrRole).value<Person*>();

            if(person->uuid() == uuid)
                player= dynamic_cast<Player*>(person);
        }
    }

    if(nullptr != player)
        return createIndex(row, column, player);

    return {};
}

Qt::ItemFlags ParticipantsModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}

QModelIndex ParticipantsModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    auto parent= sourceIndex.parent();

    if(parent.isValid())
        return QModelIndex();

    auto p= static_cast<Person*>(sourceIndex.internalPointer());
    auto id= p->uuid();

    auto row= m_readWrite.indexOf(id);
    if(row < 0)
        row= m_readOnly.indexOf(id);

    if(row < 0)
    {
        QStringList listId;
        for(int i= 0; i < sourceIndex.row(); ++i)
        {
            auto idx= sourceModel()->index(i, 0, QModelIndex());
            listId << idx.data(PlayerModel::IdentifierRole).toString();
        }
    }

    return createIndex(sourceIndex.row(), 0, sourceIndex.internalPointer());
}

QModelIndex ParticipantsModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return sourceModel()->index(proxyIndex.row(), 0, QModelIndex());
}

void ParticipantsModel::saveModel(QJsonObject& root)
{
    QJsonArray hidden;
    QJsonArray readOnly;
    QJsonArray readWrite;

    for(auto& id : m_readOnly)
    {
        readOnly << id;
    }

    for(auto& id : m_readWrite)
    {
        readWrite << id;
    }

    root["readOnly"]= readOnly;
    root["readWrite"]= readWrite;
}
void ParticipantsModel::loadModel(QJsonObject& root)
{
    QJsonArray readOnly= root["readOnly"].toArray();
    QJsonArray readWrite= root["readWrite"].toArray();

    beginResetModel();

    m_readOnly.clear();
    m_readWrite.clear();

    for(auto playerId : readOnly)
    {
        m_readOnly.append(playerId.toString());
    }

    for(auto playerId : readWrite)
    {
        m_readWrite.append(playerId.toString());
    }
    endResetModel();
}

void ParticipantsModel::promotePlayer(const QModelIndex& index)
{
    ParticipantsModel::Permission perm= permissionFor(index);
    switch(perm)
    {
    case ReadOnly:
        promotePlayerToReadWrite(index);
        break;
    case Hidden:
        promotePlayerToRead(index);
        break;
    default:
        break;
    }
}

void ParticipantsModel::demotePlayer(const QModelIndex& index)
{
    ParticipantsModel::Permission perm= permissionFor(index);
    switch(perm)
    {
    case ParticipantsModel::ReadOnly:
        demotePlayerToHidden(index);
        break;
    case ParticipantsModel::ReadWrite:
        demotePlayerToRead(index);
        break;
    default:
        break;
    }
}

void ParticipantsModel::promotePlayerToRead(const QModelIndex& index)
{
    beginResetModel();
    auto uuid= index.data(PlayerModel::IdentifierRole).toString();
    m_readOnly.append(uuid);
    endResetModel();
}

void ParticipantsModel::promotePlayerToReadWrite(const QModelIndex& index)
{
    auto uuid= index.data(PlayerModel::IdentifierRole).toString();
    auto parent= createIndex(1, 0);
    beginRemoveRows(parent, m_readOnly.indexOf(uuid), m_readOnly.indexOf(uuid));
    auto b= m_readOnly.removeOne(uuid);
    Q_ASSERT(b);
    endRemoveRows();

    parent= createIndex(0, 0);
    beginInsertRows(parent, m_readWrite.size(), m_readWrite.size());
    m_readWrite.append(uuid);
    endInsertRows();
}

void ParticipantsModel::demotePlayerToRead(const QModelIndex& index)
{
    auto uuid= index.data(PlayerModel::IdentifierRole).toString();
    auto parent= createIndex(0, 0);
    beginRemoveRows(parent, m_readWrite.indexOf(uuid), m_readWrite.indexOf(uuid));
    auto b= m_readWrite.removeOne(uuid);
    Q_ASSERT(b);
    endRemoveRows();

    parent= createIndex(1, 0);
    beginInsertRows(parent, m_readOnly.size(), m_readOnly.size());
    m_readOnly.append(uuid);
    endInsertRows();
}

void ParticipantsModel::demotePlayerToHidden(const QModelIndex& index)
{
    beginResetModel();
    auto uuid= index.data(PlayerModel::IdentifierRole).toString();
    auto b= m_readOnly.removeOne(uuid);
    Q_ASSERT(b);
    endResetModel();
}
