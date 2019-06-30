/*************************************************************************
 *   Copyright (c) 2015 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include "localpersonmodel.h"

#include "data/character.h"
#include "data/person.h"
#include "data/player.h"

LocalPersonModel::LocalPersonModel() : QAbstractListModel()
{
    m_playersList= PlayersList::instance();
}

QVariant LocalPersonModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return {};
    auto player= m_playersList->getLocalPlayer();
    if(nullptr == player)
        return {};

    auto row      = index.row();
    Person* person= nullptr;
    if(row == 0)
    {
        person= player;
    }
    else
    {
        person= player->getCharacterByIndex(row - 1);
    }
    if(nullptr == person)
        return {};

    if(role == Qt::DisplayRole)
    {
        return person->name();
    }
    return {};
}

int LocalPersonModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    Player* tmp= m_playersList->getLocalPlayer();
    if(nullptr != tmp)
    {
        return 1 + tmp->getChildrenCount();
    }
    else
    {
        return 0;
    }
}
