/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "mediafilteredmodel.h"

#include <QDebug>

MediaFilteredModel::MediaFilteredModel()
{
    setDynamicSortFilter(true);
    connect(this, &MediaFilteredModel::typeChanged, this, &MediaFilteredModel::invalidateFilter);
    connect(this, &MediaFilteredModel::patternChanged, this, &MediaFilteredModel::invalidateFilter);
}

Core::MediaType MediaFilteredModel::type() const
{
    return m_type;
}

QString MediaFilteredModel::pattern() const
{
    return m_pattern;
}

void MediaFilteredModel::setType(Core::MediaType type)
{
    if(m_type == type)
        return;
    m_type= type;
    emit typeChanged();
}

void MediaFilteredModel::setPattern(const QString& pattern)
{
    if(m_pattern == pattern)
        return;
    m_pattern= pattern;
    emit patternChanged();
}

bool MediaFilteredModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceparent) const
{
    QModelIndex index0= sourceModel()->index(sourceRow, 0, sourceparent);
    QModelIndex index2= sourceModel()->index(sourceRow, 2, sourceparent);
    auto mediaType= sourceModel()->data(index2).value<Core::MediaType>();
    auto name= sourceModel()->data(index0).toString();

    QRegularExpression re(m_pattern);
    auto match= re.match(name);

    auto respectPattern= m_pattern.isEmpty() ? true : match.hasMatch();

    auto respectMediaType= m_type == Core::MediaType::Unknown ? true : mediaType == m_type;

    return respectPattern && respectMediaType;
}
