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
#include "data/media.h"
namespace campaign
{
Media::Media(const QString& id, const QString& name, const QString& path, Core::MediaType type, QDateTime creation,
             QObject* parent)
    : QObject(parent), m_id(id), m_type(type), m_name(name), m_path(path), m_creation(creation)
{
}

QString Media::name() const
{
    return m_name;
}

QString Media::path() const
{
    return m_path;
}

Core::MediaType Media::type() const
{
    return m_type;
}

QString Media::id() const
{
    return m_id;
}

QDateTime Media::addedTime() const
{
    return m_creation;
}

void Media::setPath(const QString& path)
{
    if(path == m_path)
        return;
    m_path= path;
    emit pathChanged();
}

void Media::setName(const QString& name)
{
    if(name == m_name)
        return;
    m_name= name;
    emit nameChanged();
}
} // namespace campaign
