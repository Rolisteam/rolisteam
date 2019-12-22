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
#include "profilemodel.h"

#include <QFileInfo>
#include <QSettings>

#include "data/character.h"
#include "data/player.h"
#include "network/connectionprofile.h"

#include <QDebug>

ProfileModel::ProfileModel() {}

ProfileModel::~ProfileModel()
{
    m_connectionProfileList.clear();
}

QVariant ProfileModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

int ProfileModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return static_cast<int>(m_connectionProfileList.size());
    return 0;
}
QVariant ProfileModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(Qt::DisplayRole == role)
    {
        return m_connectionProfileList.at(index.row())->getTitle();
    }
    return QVariant();
}

void ProfileModel::appendProfile()
{
    ConnectionProfile* profile= new ConnectionProfile();
    profile->setTitle(QStringLiteral("Profile #%1").arg(m_connectionProfileList.size() + 1));
    profile->setPort(6660);
    profile->setName(QStringLiteral("Unknown User"));
    Player* player= new Player();
    // player->setUserVersion(m_version);
    profile->setPlayer(player);
    profile->setCharacter(new Character(QStringLiteral("Unknown"), Qt::black, false));
    appendProfile(profile);
}
void ProfileModel::appendProfile(ConnectionProfile* profile)
{
    if(nullptr == profile)
        return;

    beginInsertRows(QModelIndex(), m_connectionProfileList.size(), m_connectionProfileList.size());
    m_connectionProfileList.push_back(std::unique_ptr<ConnectionProfile>(std::move(profile)));
    endInsertRows();

    auto player= profile->getPlayer();
    if(nullptr == player)
        return;
    /*connect(player, &Player::nameChanged, this, &ProfileModel::writeSettings);
    connect(player, &Player::colorChanged, this, &ProfileModel::writeSettings);
    connect(player, &Player::avatarChanged, this, &ProfileModel::writeSettings);*/

    auto character= profile->getCharacter();

    if(nullptr == character)
        return;

    /*connect(character, &Player::nameChanged, this, &ProfileModel::writeSettings);
    connect(character, &Player::colorChanged, this, &ProfileModel::writeSettings);
    connect(character, &Player::avatarChanged, this, &ProfileModel::writeSettings);*/
}

ConnectionProfile* ProfileModel::getProfile(const QModelIndex& index)
{
    return getProfile(index.row());
}

void ProfileModel::cloneProfile(const QModelIndex& index)
{
    auto profileSrc= getProfile(index.row());

    if(nullptr != profileSrc)
    {
        ConnectionProfile* clonedProfile= new ConnectionProfile();
        clonedProfile->cloneProfile(profileSrc);
        auto name= clonedProfile->getName();
        clonedProfile->setName(name.append(tr(" (clone)")));
        appendProfile(clonedProfile);
    }
}

int ProfileModel::indexOf(ConnectionProfile* tmp)
{
    auto const& pos
        = std::find_if(m_connectionProfileList.begin(), m_connectionProfileList.end(),
                       [tmp](const std::unique_ptr<ConnectionProfile>& profile) { return tmp == profile.get(); });

    if(pos == m_connectionProfileList.end())
        return -1;

    auto idx= std::distance(m_connectionProfileList.begin(), pos);

    return idx;
}
ConnectionProfile* ProfileModel::getProfile(int index)
{
    if((!m_connectionProfileList.empty()) && (m_connectionProfileList.size() > index))
    {
        return m_connectionProfileList.at(index).get();
    }
    return nullptr;
}
void ProfileModel::removeProfile(int index)
{
    auto idx= static_cast<std::size_t>(index);
    if(m_connectionProfileList.size() <= idx)
        return;

    auto pos= m_connectionProfileList.begin() + index;

    beginRemoveRows(QModelIndex(), index, index);
    m_connectionProfileList.erase(pos);
    endRemoveRows();
}

Qt::ItemFlags ProfileModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
