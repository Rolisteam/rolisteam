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
#include "abstractmediacontroller.h"

#include "data/cleveruri.h"

AbstractMediaContainerController::AbstractMediaContainerController(CleverURI* uri, QObject* parent)
    : QObject(parent), m_uri(uri)
{
    Q_ASSERT(uri != nullptr);
}

AbstractMediaContainerController::~AbstractMediaContainerController()= default;

QString AbstractMediaContainerController::name() const
{
    return m_uri->name();
}

QString AbstractMediaContainerController::uuid() const
{
    return m_uri->uuid();
}

CleverURI* AbstractMediaContainerController::uri() const
{
    return m_uri.get();
}

QString AbstractMediaContainerController::title() const
{
    return name();
}

bool AbstractMediaContainerController::isActive() const
{
    return m_active;
}

void AbstractMediaContainerController::setUri(CleverURI* uri)
{
    if(m_uri.get() == uri)
        return;
    m_uri.reset(std::move(uri));
    emit uriChanged();
}

void AbstractMediaContainerController::aboutToClose()
{
    emit closeContainer();
}

void AbstractMediaContainerController::setActive(bool b)
{
    if(b == m_active)
        return;
    m_active= b;
    emit activeChanged();
}
void AbstractMediaContainerController::setName(const QString& name)
{
    if(nullptr == m_uri)
        return;
    if(m_uri->name() == name)
        return;
    m_uri->setName(name);
    emit nameChanged();
}
