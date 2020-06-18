/*************************************************************************
 *   Copyright (c) 2015 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   https://rolisteam.org/                                           *
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

#include <QDebug>

LocalPersonModel::LocalPersonModel() : QAbstractProxyModel() {}

QModelIndex LocalPersonModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();

    auto parent= sourceIndex.parent();

    if(!parent.isValid() && sourceIndex.row() > 0)
        return QModelIndex();

    if(!parent.isValid() && sourceIndex.row() == 0)
        return createIndex(0, 0, sourceIndex.internalPointer());

    if(parent.isValid() && parent.row() == 0)
        return createIndex(1 + sourceIndex.row(), 0, sourceIndex.internalPointer());

    return QModelIndex();
}

QModelIndex LocalPersonModel::mapToSource(const QModelIndex& proxyIndex) const
{
    QModelIndex idx;
    if(proxyIndex.row() == 0)
        idx= sourceModel()->index(0, 0, QModelIndex());
    else
        idx= sourceModel()->index(proxyIndex.row() - 1, 0, sourceModel()->index(0, 0, QModelIndex()));

    return idx;
}

void LocalPersonModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);
    auto func= [this]() {
        beginResetModel();

        endResetModel();
    };
    connect(sourceModel, &QAbstractItemModel::dataChanged, this, func);
    connect(sourceModel, &QAbstractItemModel::rowsInserted, this, func);
}

QModelIndex LocalPersonModel::index(int r, int c, const QModelIndex&) const
{
    return createIndex(r, c);
}

QModelIndex LocalPersonModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int LocalPersonModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    auto firstItem= sourceModel()->index(0, 0);
    if(!firstItem.isValid())
        return 0;

    auto child= sourceModel()->rowCount(firstItem);

    return child + 1;
}

int LocalPersonModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return 1;
}
