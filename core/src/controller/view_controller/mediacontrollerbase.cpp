/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "mediacontrollerbase.h"

#include "data/cleveruri.h"

MediaControllerBase::MediaControllerBase(const QString& id, Core::ContentType contentType, QObject* parent)
    : QObject(parent), m_uuid(id), m_name(tr("Unknown")), m_type(contentType)
{
}

MediaControllerBase::~MediaControllerBase()= default;

QString MediaControllerBase::name() const
{
    return m_name;
}

QString MediaControllerBase::uuid() const
{
    return m_uuid;
}

QString MediaControllerBase::title() const
{
    return name();
}

bool MediaControllerBase::isActive() const
{
    return m_active;
}

bool MediaControllerBase::localGM() const
{
    return m_localGM;
}

bool MediaControllerBase::modified() const
{
    return m_modified;
}
QString MediaControllerBase::path() const
{
    return m_path;
}

Core::ContentType MediaControllerBase::contentType() const
{
    return m_type;
}

QString MediaControllerBase::ownerId() const
{
    return m_ownerId;
}

QString MediaControllerBase::localId() const
{
    return m_localId;
}

void MediaControllerBase::setLocalGM(bool b)
{
    if(m_localGM == b)
        return;
    m_localGM= b;
    emit localGMChanged(m_localGM);
}

void MediaControllerBase::setUuid(const QString& id)
{
    if(m_uuid == id)
        return;
    m_uuid= id;
    emit uuidChanged(id);
}

void MediaControllerBase::setOwnerId(const QString& id)
{
    if(id == m_ownerId)
        return;
    m_ownerId= id;
    emit ownerIdChanged(m_ownerId);
}

bool MediaControllerBase::localIsOwner() const
{
    return (m_ownerId == m_localId);
}

bool MediaControllerBase::remote() const
{
    return m_remote;
}

void MediaControllerBase::setLocalId(const QString& id)
{
    if(id == m_localId)
        return;
    m_localId= id;
    emit localIdChanged(m_ownerId);
}

void MediaControllerBase::setModified(bool b)
{
    if(b == m_modified)
        return;

    m_modified= b;
    emit modifiedChanged(m_modified);
}

void MediaControllerBase::askToClose()
{
    emit closeMe(uuid());
}

void MediaControllerBase::aboutToClose()
{
    emit closeContainer();
}

void MediaControllerBase::setActive(bool b)
{
    if(b == m_active)
        return;
    m_active= b;
    emit activeChanged();
}
void MediaControllerBase::setName(const QString& name)
{
    if(name == m_name)
        return;
    m_name= name;
    emit nameChanged(m_name);
}
void MediaControllerBase::setPath(const QString& path)
{
    if(path == m_path)
        return;
    m_path= path;
    emit pathChanged(m_path);
}

void MediaControllerBase::setRemote(bool remote)
{
    m_remote= remote;
}
