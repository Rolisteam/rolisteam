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
#ifndef PLAYERPROXYMODEL_H
#define PLAYERPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QObject>
#include <core_global.h>
class Player;
class CORE_EXPORT PlayerProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    PlayerProxyModel(QObject* parent= nullptr);

    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;

    QModelIndex parent(const QModelIndex&) const override;
    QModelIndex index(int, int, const QModelIndex&) const override;

    Player* player(const QString& id);
};

#endif // PLAYERPROXYMODEL_H
