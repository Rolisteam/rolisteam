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
#include "singlecontenttypemodel.h"

#include "controller/view_controller/mediacontrollerbase.h"
#include "model/contentmodel.h"

SingleContentTypeModel::SingleContentTypeModel(Core::ContentType type, QObject* parent)
    : QSortFilterProxyModel(parent), m_type(type)
{
    setFilterRole(ContentModel::ContentTypeRole);
}

bool SingleContentTypeModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent)
{
    QModelIndex index0= sourceModel()->index(sourceRow, 0, sourceParent);
    return sourceModel()->data(index0, filterRole()).value<Core::ContentType>() == m_type;
}

Core::ContentType SingleContentTypeModel::type() const
{
    return m_type;
}

bool SingleContentTypeModel::contains(const QString& uuid) const
{
    bool found= false;
    for(int i= 0; i < rowCount() && !found; ++i)
    {
        auto idx= index(i, 0);
        auto id= idx.data(ContentModel::UuidRole);
        found= id == uuid;
    }
    return found;
}

MediaControllerBase* SingleContentTypeModel::controller(const QString& uuid) const
{
    QModelIndex idx;
    for(int i= 0; i < rowCount() && !idx.isValid(); ++i)
    {
        auto modelIndex= index(i, 0);
        auto id= modelIndex.data(ContentModel::UuidRole);
        if(id == uuid)
            idx= modelIndex;
    }
    return idx.data(ContentModel::ControllerRole).value<MediaControllerBase*>();
}
