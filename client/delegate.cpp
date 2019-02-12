/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
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

#include "delegate.h"

Delegate::Delegate(QObject* parent) : QItemDelegate(parent) {}

Delegate::~Delegate() {}

int Delegate::roleAt(const QStyleOptionViewItem& option, const QModelIndex& index, QPoint pos) const
{
    QRect decorationRect= rect(option, index, Qt::DecorationRole);
    QRect displayRect= rect(option, index, Qt::DisplayRole);
    QRect checkRect= rect(option, index, Qt::CheckStateRole);

    doLayout(option, &checkRect, &decorationRect, &displayRect, true);

    if(decorationRect.contains(pos))
        return Qt::DecorationRole;
    if(displayRect.contains(pos))
        return Qt::DisplayRole;
    if(checkRect.contains(pos))
        return Qt::CheckStateRole;

    return -1;
}
