/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "nonplayablecharactermodel.h"
namespace campaign
{
NonPlayableCharacter::NonPlayableCharacter(QObject* parent) : Character(parent)
{
    setNpc(true);
}

QString NonPlayableCharacter::tokenPath() const
{
    return m_tokenPath;
}

QStringList NonPlayableCharacter::tags() const
{
    return m_tags;
}

void NonPlayableCharacter::setTokenPath(const QString& path)
{
    if(path == m_tokenPath)
        return;
    m_tokenPath= path;
    emit tokenPathChanged();
}

void NonPlayableCharacter::setTags(const QStringList& list)
{
    if(list == m_tags)
        return;
    m_tags= list;
    emit tagsChanged();
}

NonPlayableCharacterModel::NonPlayableCharacterModel(QObject* parent) : QAbstractListModel(parent)
{
    m_header << tr("Name") << tr("Color") << tr("rtoken") << tr("Min HP") << tr("Current HP") << tr("Max HP")
             << tr("Initiative") << tr("Distance Per turn") << tr("State") << tr("Life Color") << tr("Init Command")
             << tr("Tags");
}

QVariant NonPlayableCharacterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return {};

    if(Qt::DisplayRole == role)
    {
        return m_header[section];
    }

    return {};
}

int NonPlayableCharacterModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_data.size();
}

int NonPlayableCharacterModel::columnCount(const QModelIndex&) const
{
    return m_header.size();
}

QVariant NonPlayableCharacterModel::data(const QModelIndex& index, int role) const
{
    QSet<int> acceptedRole({Qt::DecorationRole, Qt::DisplayRole, Qt::BackgroundRole, Qt::EditRole, RoleUuid});
    if(!index.isValid() || !acceptedRole.contains(role))
        return QVariant();

    auto customRole= Qt::UserRole + 1 + index.column();
    if(role == Qt::DecorationRole)
        customRole= RoleAvatar;
    if(role == Qt::BackgroundRole && (customRole != RoleColor && customRole != RoleLifeColor))
        return {};

    auto const& character= m_data[index.row()];
    QVariant res;
    switch(customRole)
    {
    case RoleUuid:
        res = character->uuid();
        break;
    case RoleName:
        res= character->name();
        break;
    case RoleColor:
        res= character->getColor();
        break;
    case RoleTokenPath:
        res= character->tokenPath();
        break;
    case RoleMinHp:
        res= character->getHealthPointsMin();
        break;
    case RoleCurrentHp:
        res= character->getHealthPointsCurrent();
        break;
    case RoleMaxHp:
        res= character->getHealthPointsMax();
        break;
    case RoleInitiative:
        res= character->getInitiativeScore();
        break;
    case RoleDistancePerTurn:
        res= character->getDistancePerTurn();
        break;
    case RoleState:
        res= character->stateId();
        break;
    case RoleLifeColor:
        res= character->getLifeColor();
        break;
    case RoleInitCommand:
        res= character->getInitCommand();
        break;
    case RoleTags:
        res= character->tags();
        break;
    case RoleAvatar:
        res= QVariant::fromValue(character->getAvatar());
        break;
    }

    return res;
}

bool NonPlayableCharacterModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(data(index, role) != value)
    {
        auto customRole= Qt::UserRole + 1 + index.column();

        bool res= true;
        auto const& character= m_data[index.row()];
        switch(customRole)
        {
        case RoleName:
            character->setName(value.toString());
            break;
        case RoleColor:
            character->setColor(value.value<QColor>());
            break;
        case RoleTokenPath:
            character->setTokenPath(value.toString());
            break;
        case RoleMinHp:
            character->setHealthPointsMin(value.toInt());
            break;
        case RoleCurrentHp:
            character->setHealthPointsCurrent(value.toInt());
            break;
        case RoleMaxHp:
            character->setHealthPointsMax(value.toInt());
            break;
        case RoleInitiative:
            character->setInitiativeScore(value.toInt());
            break;
        case RoleDistancePerTurn:
            character->setDistancePerTurn(value.toInt());
            break;
        case RoleState:
            character->setStateId(value.toString());
            break;
        case RoleLifeColor:
            character->setLifeColor(value.value<QColor>());
            break;
        case RoleInitCommand:
            character->setInitCommand(value.toString());
            break;
        case RoleTags:
            character->setTags(value.toStringList());
            break;
        default:
            res= false;
            break;
        }
        if(res)
            emit dataChanged(index, index, QVector<int>() << role);
        return res;
    }
    return false;
}

Qt::ItemFlags NonPlayableCharacterModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void NonPlayableCharacterModel::append()
{
    addCharacter(new NonPlayableCharacter);
}

void NonPlayableCharacterModel::addCharacter(NonPlayableCharacter* character)
{
    auto size= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), size, size);
    m_data.push_back(std::make_unique<NonPlayableCharacter>(character));
    endInsertRows();
    emit characterAdded();
}

void NonPlayableCharacterModel::setModelData(const std::vector<NonPlayableCharacter*>& data)
{
    beginResetModel();
    m_data.clear();
    std::for_each(std::begin(data), std::end(data), [this](NonPlayableCharacter* npc) {
        std::unique_ptr<NonPlayableCharacter> unique(npc);
        m_data.push_back(std::move(unique));
    });
    endResetModel();
}

void NonPlayableCharacterModel::removeNpc(const QModelIndex& index)
{
    if(!index.isValid())
        return;
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    auto const& it= m_data.begin() + index.row();
    m_data.erase(it);
    endRemoveRows();

    emit characterRemoved((*it)->uuid());
}

const std::vector<std::unique_ptr<NonPlayableCharacter>>& NonPlayableCharacterModel::npcList() const
{
    return m_data;
}
} // namespace campaign
