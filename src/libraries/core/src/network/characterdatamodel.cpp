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
#include "network/characterdatamodel.h"

#include <QDebug>
#include <QPixmap>

#include "iohelper.h"
#include "network/connectionprofile.h"

CharacterDataModel::CharacterDataModel(QObject* parent) : QAbstractListModel(parent) {}

int CharacterDataModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid() || !m_profile)
        return 0;

    return m_profile->characterCount();
}

QVariant CharacterDataModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || !m_profile)
        return QVariant();

    auto character= m_profile->character(index.row());
    QVariant var;

    QSet<int> roles{AvatarData, Color, Name, Avatar, None};

    int col= role;
    if(!roles.contains(role))
    {
        static std::vector<std::map<int, Role>> matrix(
            {{{Qt::BackgroundRole, Color}, {Qt::DecorationRole, Avatar}, {Qt::EditRole, AvatarData}},
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
    }

    switch(col)
    {
    case Avatar:
    {
        auto pix= utils::IOHelper::dataToImage(character.m_avatarData);
        if(pix.isNull())
            var= tr("No Avatar");
        else
            var= QVariant::fromValue(pix.scaled(50, 50, Qt::KeepAspectRatio));
    }
    break;
    case AvatarData:
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

connection::CharacterData CharacterDataModel::character(int i) const
{
    if(m_profile)
        return m_profile->character(i);
    else
        return {};
}
/*bool CharacterDataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(m_profile.isNull())
        return false;
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
}*/

Qt::ItemFlags CharacterDataModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool CharacterDataModel::insertCharacter()
{
    if(m_profile.isNull())
        return false;
    beginInsertRows(QModelIndex(), m_profile->characterCount(), m_profile->characterCount());
    connection::CharacterData data({tr("New Character"), Qt::lightGray, "", QHash<QString, QVariant>()});
    m_profile->addCharacter(data);
    endInsertRows();
    return true;
}

bool CharacterDataModel::addCharacter(const connection::CharacterData& data)
{
    if(m_profile.isNull())
        return false;
    beginInsertRows(QModelIndex(), m_profile->characterCount(), m_profile->characterCount());
    m_profile->addCharacter(data);
    endInsertRows();
    return true;
}

bool CharacterDataModel::removeCharacter(int index)
{
    if(m_profile.isNull())
        return false;
    if(m_profile->characterCount() < 1)
        return false;

    if(index >= rowCount())
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    m_profile->removeCharacter(index);
    endRemoveRows();
    return true;
}

void CharacterDataModel::setAvatar(int i, const QByteArray& img)
{
    if(!m_profile)
        return;

    auto& character= m_profile->character(i);
    character.m_avatarData= img;
    emit dataChanged(index(i, 0), index(i, 0), {Avatar, AvatarData});
}

void CharacterDataModel::setName(int i, const QString& string)
{
    if(!m_profile)
        return;

    auto& character= m_profile->character(i);
    character.m_name= string;
    emit dataChanged(index(i, 0), index(i, 0), {Name});
}
void CharacterDataModel::setColor(int i, const QColor& color)
{
    if(!m_profile)
        return;

    auto& character= m_profile->character(i);
    character.m_color= color;
    emit dataChanged(index(i, 0), index(i, 0), {Color});
}

void CharacterDataModel::setProfile(ConnectionProfile* profile)
{
    beginResetModel();
    m_profile= profile;
    endResetModel();
}

QHash<int, QByteArray> CharacterDataModel::roleNames() const
{
    return {{None, "none"}, {Avatar, "avatar"}, {Color, "color"}, {AvatarData, "avatarData"}, {Name, "name"}};
}
