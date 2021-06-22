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

#include "model/playermodel.h"
#include <QAbstractProxyModel>

/**
 * @brief The LocalPersonModel class
 */
class LocalPersonModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    LocalPersonModel();

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;

    QHash<int, QByteArray> roleNames() const override;

    virtual void setSourceModel(QAbstractItemModel* sourceModel) override;
    virtual QModelIndex index(int, int, const QModelIndex&) const override;
    virtual QModelIndex parent(const QModelIndex& idx= QModelIndex()) const override;
    virtual int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent= QModelIndex()) const override;
};

#endif
