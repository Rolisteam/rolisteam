/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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


#include "userlist/playerslistproxy.h"

#include "userlist/playersList.h"


/*************************
 * PlayersListProxyModel *
 *************************/

PlayersListProxyModel::PlayersListProxyModel(QObject * parent)
    : QAbstractProxyModel(parent), m_rowsAboutToBeInserted(false), m_rowsAboutToBeRemoved(false)
{
    PlayersList* g_playersList = PlayersList::instance();
    setSourceModel(g_playersList);

    connect(g_playersList, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(p_rowsAboutToBeInserted(QModelIndex,int,int)));
    connect(g_playersList, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(p_rowsInserted()));
    connect(g_playersList, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(p_rowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(g_playersList, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(p_rowsRemoved()));
    connect(g_playersList, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(p_dataChanged(QModelIndex,QModelIndex)));
}

QModelIndex PlayersListProxyModel::mapFromSource(const QModelIndex & sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();

    quint32 parentRow = (quint32)(sourceIndex.internalId() & PlayersList::NoParent);
    return createIndex(sourceIndex.row(), sourceIndex.column(), parentRow);
}

QModelIndex PlayersListProxyModel::mapToSource(const QModelIndex & proxyIndex) const
{
    return PlayersList::instance()->mapIndexToMe(proxyIndex);
}

QModelIndex PlayersListProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex sourceIndex = sourceModel()->index(row, column, mapToSource(parent));
    return mapFromSource(sourceIndex);
}

QModelIndex PlayersListProxyModel::parent(const QModelIndex &index) const
{
    QModelIndex sourceIndex = sourceModel()->parent(mapToSource(index));
    return mapFromSource(sourceIndex);
}

int PlayersListProxyModel::rowCount(const QModelIndex &parent) const
{
    return sourceModel()->rowCount(mapToSource(parent));
}

int PlayersListProxyModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool PlayersListProxyModel::filterChangingRows(QModelIndex & parent, int & start, int & end)
{
    Q_UNUSED(start);
    Q_UNUSED(end);
    parent = mapFromSource(parent);
    return true;
}

void PlayersListProxyModel::p_rowsAboutToBeInserted(const QModelIndex & parent, int start, int end)
{
    QModelIndex model(parent);
    m_rowsAboutToBeInserted = this->filterChangingRows(model, start, end);
    if (m_rowsAboutToBeInserted)
        beginInsertRows(model, start, end);
}

void PlayersListProxyModel::p_rowsInserted()
{
    if (m_rowsAboutToBeInserted)
    {
        m_rowsAboutToBeInserted = false;
        endInsertRows();
    }
}

void PlayersListProxyModel::p_rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end)
{
    QModelIndex model(parent);
    m_rowsAboutToBeRemoved = this->filterChangingRows(model, start, end);
    if (m_rowsAboutToBeRemoved)
        beginRemoveRows(model, start, end);
}

void PlayersListProxyModel::p_rowsRemoved()
{
    if (m_rowsAboutToBeRemoved)
    {
        m_rowsAboutToBeRemoved = false;
        endRemoveRows();
    }
}

void PlayersListProxyModel::p_dataChanged(const QModelIndex & from, const QModelIndex & to)
{
    emit dataChanged(mapFromSource(from), mapFromSource(to));
}
