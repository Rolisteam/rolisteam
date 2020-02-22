/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "playeronmapmodel.h"

#include "data/character.h"
#include "map/map.h"
#include "userlist/playermodel.h"

PlayerOnMapModel::PlayerOnMapModel(QObject* parent) : QIdentityProxyModel(parent), m_map(nullptr) {}

Qt::ItemFlags PlayerOnMapModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags ret= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if(m_map != nullptr && isCheckable(index))
        ret|= Qt::ItemIsUserCheckable;

    auto realCurrent= mapToSource(index);
    auto isLocal= realCurrent.data(PlayerModel::LocalRole).toBool();
    auto isGM= (isLocal && realCurrent.data(PlayerModel::GmRole).toBool());

    if(isLocal || isGM)
        ret|= Qt::ItemIsEditable;
    return ret;
}

QVariant PlayerOnMapModel::data(const QModelIndex& index, int role) const
{
    if(isCheckable(index) && role == Qt::CheckStateRole)
    {
        if(m_map == nullptr)
            return false;
        auto realIndex= mapToSource(index);
        Person* person= static_cast<Person*>(realIndex.internalPointer());
        return m_map->isVisiblePc(person->uuid());
    }
    return QAbstractProxyModel::data(index, role);
}

bool PlayerOnMapModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto realIndex= mapToSource(index);
    Person* person= static_cast<Person*>(realIndex.internalPointer());
    auto isLocal= realIndex.data(PlayerModel::LocalRole).toBool();
    auto isGM= (isLocal && realIndex.data(PlayerModel::GmRole).toBool());

    if(person != nullptr && (isLocal || isGM))
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            person->setName(value.toString());
            return true;
        case Qt::DecorationRole:
            if(value.type() == QVariant::Color)
            {
                person->setColor(value.value<QColor>());
                return true;
            }
        }

    if(role == Qt::CheckStateRole && isCheckable(index))
    {
        // isCheckable ensures person and m_map is not nullptr and person is a character.
        m_map->toggleCharacterView(static_cast<Character*>(person));
        emit dataChanged(index, index);
        return true;
    }

    return QAbstractProxyModel::setData(index, value, role);
}

void PlayerOnMapModel::setCurrentMap(Map* map)
{
    if(map == m_map)
        return;
    beginResetModel();
    m_map= map;
    endResetModel();
}

bool PlayerOnMapModel::isCheckable(const QModelIndex& index) const
{
    if(!index.isValid() || m_map == nullptr || !index.parent().isValid())
        return false;

    auto realIndex= mapToSource(index);
    auto isLocal= realIndex.data(PlayerModel::LocalRole).toBool();
    auto isGM= (isLocal && realIndex.data(PlayerModel::GmRole).toBool());
    return isLocal || isGM;
}
