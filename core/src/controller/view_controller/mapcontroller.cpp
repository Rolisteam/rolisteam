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
#include "mapcontroller.h"

#include <QVariant>

#include "data/cleveruri.h"

MapController::MapController(CleverURI* uri, const std::map<QString, QVariant>& args, QObject* parent)
    : AbstractMediaContainerController(uri, parent)
{
    setPermission(args.at(QStringLiteral("permission")).value<VMapFrame::PermissionMode>());
    setBgColor(args.at(QStringLiteral("bgcolor")).value<QColor>());
    setSize(args.at(QStringLiteral("size")).toSize());
}

void MapController::saveData() const {}

void MapController::loadData() const {}

QSize MapController::size() const
{
    return m_size;
}

void MapController::setHidden(bool hidden)
{
    if(m_hidden == hidden)
        return;

    m_hidden= hidden;
    emit hiddenChanged(m_hidden);
}

bool MapController::hidden() const
{
    return m_hidden;
}

void MapController::setSize(const QSize& size)
{
    if(m_size == size)
        return;
    m_size= size;
    emit sizeChanged();
}

QColor MapController::bgColor() const
{
    return m_bgColor;
}

void MapController::setBgColor(QColor bgColor)
{
    if(m_bgColor == bgColor)
        return;

    m_bgColor= bgColor;
    emit bgColorChanged(m_bgColor);
}

VMapFrame::PermissionMode MapController::permission() const
{
    return m_permission;
}

void MapController::setPermission(VMapFrame::PermissionMode permission)
{
    if(m_permission == permission)
        return;

    m_permission= permission;
    emit permissionChanged(m_permission);
}
