/***************************************************************************
 *	Copyright (C) 2018 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include "cleverurimimedata.h"
#include "cleveruri.h"

//////////////////////////////////////
/// CleverUriMimeData
/////////////////////////////////////

CleverUriMimeData::CleverUriMimeData()
{
    setData("rolisteam/cleverurilist", QByteArray());
}

void CleverUriMimeData::addResourceNode(ResourcesNode* m, const QModelIndex index)
{
    if(nullptr != m)
    {
        m_mediaList.insert(index, m);
    }
}
QMap<QModelIndex, ResourcesNode*> CleverUriMimeData::getList() const
{
    return m_mediaList;
}
bool CleverUriMimeData::hasFormat(const QString& mimeType) const
{
    return ((mimeType == "rolisteam/cleverurilist") | QMimeData::hasFormat(mimeType));
}
