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
#include "filterinstantmessagingmodel.h"

#include <QUuid>

#include "instantmessagingmodel.h"

namespace InstantMessaging
{
FilterInstantMessagingModel::FilterInstantMessagingModel(QObject* parent)
    : QSortFilterProxyModel(parent), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}
void FilterInstantMessagingModel::setFilterParameter(bool b, QStringList data)
{
    if(m_allBut == b && m_filteredId == data)
        return;

    m_allBut= b;
    m_filteredId= data;
    invalidateFilter();
}

QVariant FilterInstantMessagingModel::get(int idx)
{
    auto modelIndex= index(idx, 0);
    return modelIndex.data(InstantMessagingModel::ChatRole);
}

QString FilterInstantMessagingModel::uuid() const
{
    return m_uuid;
}

bool FilterInstantMessagingModel::all() const
{
    return m_allBut;
}

bool FilterInstantMessagingModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    bool value= true;
    if(m_allBut && m_filteredId.isEmpty())
        value= true;
    else if(m_allBut && m_filteredId.isEmpty())
        value= false;
    else
    {
        auto index= sourceModel()->index(source_row, 0, source_parent);
        auto id= index.data(InstantMessagingModel::IdRole).toString();
        value= m_filteredId.contains(id) ? !m_allBut : m_allBut;
    }
    return value;
}
} // namespace InstantMessaging
