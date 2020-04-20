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
#include "playerproxymodel.h"

#include "data/person.h"
#include "data/player.h"
#include "userlist/playermodel.h"

PlayerProxyModel::PlayerProxyModel(QObject* parent)
: QAbstractProxyModel(parent)
{

}

int PlayerProxyModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return 1;
}

int PlayerProxyModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return sourceModel()->rowCount(); // only item at one depth level
}

QModelIndex PlayerProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    auto parent= sourceIndex.parent();

    if(parent.isValid())
        return QModelIndex();

    return createIndex(sourceIndex.row(), 0, sourceIndex.internalPointer());
}

QModelIndex PlayerProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return sourceModel()->index(proxyIndex.row(), 0, QModelIndex());
}

QModelIndex PlayerProxyModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

QModelIndex PlayerProxyModel::index(int r, int c, const QModelIndex&) const
{
    return createIndex(r, c);
}

Player* PlayerProxyModel::player(const QString& id)
{
    Player* find= nullptr;

    for(int i= 0; i < rowCount() && find == nullptr; ++i)
    {
        auto idx= index(i, 0, QModelIndex());
        auto uuid= idx.data(PlayerModel::IdentifierRole).toString();
        auto person= idx.data(PlayerModel::PersonPtrRole).value<Person*>();
        if(uuid == id)
            find= dynamic_cast<Player*>(person);
    }

    return find;
}
