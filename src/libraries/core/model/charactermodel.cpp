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
#include "charactermodel.h"

#include "data/character.h"
#include "data/person.h"
#include "data/player.h"
#include "model/playermodel.h"

CharacterModel::CharacterModel(QObject* parent) : QAbstractProxyModel(parent)
{
    // setDynamicSortFilter(true);
}

int CharacterModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return 1;
}

int CharacterModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    int row= 0;
    auto size= sourceModel()->rowCount();
    for(int i= 0; i < size; ++i)
    {
        auto index= sourceModel()->index(i, 0);
        row+= sourceModel()->rowCount(index);
    }
    return row;
}

QModelIndex CharacterModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    auto parent= sourceIndex.parent();

    if(!parent.isValid())
        return QModelIndex();

    int row= sourceIndex.row();
    for(int i= 0; i < parent.row(); ++i)
    {
        auto index= sourceModel()->index(i, 0);
        row+= sourceModel()->rowCount(index);
    }

    if(parent.isValid())
        return createIndex(row, 0, sourceIndex.internalPointer());

    return QModelIndex();
}

QModelIndex CharacterModel::mapToSource(const QModelIndex& proxyIndex) const
{
    auto row= proxyIndex.row();
    QModelIndex parent;
    bool found= false;
    for(int i= 0; i < sourceModel()->rowCount() && !found; ++i)
    {
        auto index= sourceModel()->index(i, 0);
        auto count= sourceModel()->rowCount(index);
        if((row - count) >= 0)
        {
            row-= count;
            parent= index;
        }
        else if(row < count)
        {
            parent= index;
            found= true;
        }
    }
    return sourceModel()->index(row, 0, parent);
}

QModelIndex CharacterModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

QModelIndex CharacterModel::index(int r, int c, const QModelIndex&) const
{
    return createIndex(r, c);
}

Character* CharacterModel::character(const QString& id)
{
    Character* find= nullptr;

    for(int i= 0; i < rowCount() && find == nullptr; ++i)
    {
        auto idx= index(i, 0, QModelIndex());
        auto uuid= idx.data(PlayerModel::IdentifierRole).toString();
        auto person= idx.data(PlayerModel::PersonPtrRole).value<Person*>();
        if(uuid == id)
            find= dynamic_cast<Character*>(person);
    }

    return find;
}
