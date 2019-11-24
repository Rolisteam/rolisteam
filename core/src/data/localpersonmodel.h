/*************************************************************************
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

#ifndef LOCAL_PERSON_MODEL_H
#define LOCAL_PERSON_MODEL_H

#include "userlist/playermodel.h"
#include <QAbstractProxyModel>

/**
 * @brief The LocalPersonModel class
 */
class LocalPersonModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    LocalPersonModel();
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    // QVariant data(const QModelIndex& index, int role) const;
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    // int rowCount(const QModelIndex& parent= QModelIndex()) const;
    Q_INVOKABLE QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    Q_INVOKABLE QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

    Q_INVOKABLE virtual QModelIndex index(int, int, const QModelIndex&) const;
    Q_INVOKABLE virtual QModelIndex parent(const QModelIndex&) const;
    Q_INVOKABLE virtual int rowCount(const QModelIndex& parent) const;
    Q_INVOKABLE virtual int columnCount(const QModelIndex&) const;
};

#endif
