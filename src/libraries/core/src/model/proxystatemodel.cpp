/***************************************************************************
 *	Copyright (C) 2023 by Renaud Guezennec                               *
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
#include "model/proxystatemodel.h"
#include "model/characterstatemodel.h"

ProxyStateModel::ProxyStateModel(QAbstractItemModel* source, QObject* parent)
    : QAbstractListModel(parent), m_source(source)
{
}

QVariant ProxyStateModel::headerData(int, Qt::Orientation, int) const
{
    return {};
}

int ProxyStateModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_source->rowCount() + 1;
}

QVariant ProxyStateModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto r= index.row();

    QVariant res;
    if(r == 0 && role == Qt::DisplayRole)
        res= tr("Any");
    else if(r == 0 && role == CharacterStateModel::ID)
        res= QString{};
    else if(r > 0)
        res= m_source->data(m_source->index(r - 1, index.column()), role);

    return res;
}
