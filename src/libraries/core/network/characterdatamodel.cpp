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
#include "characterdatamodel.h"

#include <QDebug>
#include <QPixmap>

#include "connectionprofile.h"
#include "core/worker/iohelper.h"

CharacterDataModel::CharacterDataModel(QObject* parent) : QAbstractTableModel(parent) {}

int CharacterDataModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid() || !m_profile)
        return 0;

    return m_profile->characterCount();
}

int CharacterDataModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return 3;
}

QVariant CharacterDataModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || !m_profile)
        return QVariant();

    auto character= m_profile->character(index.row());
    QVariant var;
    Role col= None;

    static std::vector<std::map<int, Role>> matrix(
        {{{Qt::BackgroundRole, Color}, {Qt::DecorationRole, Avatar}, {Qt::EditRole, AvatarPath}},
         {{Qt::BackgroundRole, Color}, {Qt::EditRole, Color}, {Qt::DisplayRole, Color}},
         {{Qt::BackgroundRole, Color}, {Qt::DisplayRole, Name}, {Qt::EditRole, Name}}});

    if(role == Qt::TextAlignmentRole)
    {
        var= Qt::AlignCenter;
    }
    else
    {
        auto map= matrix[static_cast<std::size_t>(index.column())];
        auto it= map.find(role);

        if(it != map.end())
            col= map[role];
    }

    switch(col)
    {
    case Avatar:
    {
        auto pix= IOHelper::dataToImage(character.m_avatarData);
        if(pix.isNull())
            var= tr("No Avatar");
        else
            var= QVariant::fromValue(pix.scaled(50, 50, Qt::KeepAspectRatio));
    }
    break;
    case AvatarPath:
        var= character.m_avatarData;
        break;
    case Name:
        var= character.m_name;
        break;
    case Color:
        var= QVariant::fromValue(character.m_color);
        break;
    case None:
        break;
    }
    return var;
}

bool CharacterDataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(data(index, role) != value)
    {
        auto& character= m_profile->character(index.row());
        switch(index.column())
        {
        case 2:
            character.m_name= value.toString();
            break;
        case 1:
            character.m_color= value.value<QColor>();
            break;
        case 0:
            character.m_avatarData= value.toByteArray();
            break;
        default:
            return false;
        }
        // m_profile->setCharacter(index.row(), character);
        emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole << Qt::BackgroundRole << Qt::DecorationRole);
        return true;
    }
    return false;
}

Qt::ItemFlags CharacterDataModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    if(index.column() == 2)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool CharacterDataModel::insertCharacter()
{
    beginInsertRows(QModelIndex(), m_profile->characterCount(), m_profile->characterCount());
    CharacterData data({tr("New Character"), Qt::lightGray, "", QHash<QString, QVariant>()});
    m_profile->addCharacter(data);
    endInsertRows();
    return true;
}

bool CharacterDataModel::removeCharacter(const QModelIndex& index)
{
    if(m_profile->characterCount() <= 1)
        return false;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_profile->removeCharacter(index.row());
    endRemoveRows();
    return true;
}

void CharacterDataModel::setProfile(ConnectionProfile* profile)
{
    beginResetModel();
    m_profile= profile;
    endResetModel();
}
