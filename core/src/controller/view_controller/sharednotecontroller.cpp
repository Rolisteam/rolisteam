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
#include "sharednotecontroller.h"

#include "userlist/playermodel.h"

SharedNoteController::SharedNoteController(PlayerModel* model, CleverURI* uri, QObject* parent)
    : AbstractMediaContainerController(uri, parent), m_model(model)
{
    // m_model->setSourceModel();
}
SharedNoteController::~SharedNoteController()= default;

QString SharedNoteController::text() const
{
    return m_text;
}

void SharedNoteController::saveData() const {}

void SharedNoteController::loadData() const {}

SharedNoteController::Permission SharedNoteController::permission() const
{
    return m_permission;
}

SharedNoteController::HighlightedSyntax SharedNoteController::highligthedSyntax() const
{
    return m_highlightedSyntax;
}

PlayerModel* SharedNoteController::playerModel() const
{
    return m_model;
}

void SharedNoteController::setPermission(SharedNoteController::Permission permission)
{
    if(permission == m_permission)
        return;
    m_permission= permission;
    emit permissionChanged(m_permission);
}

void SharedNoteController::setText(const QString& text)
{
    if(text == m_text)
        return;
    m_text= text;
    emit textChanged(m_text);
}

void SharedNoteController::setHighligthedSyntax(SharedNoteController::HighlightedSyntax syntax)
{
    if(m_highlightedSyntax == syntax)
        return;
    m_highlightedSyntax= syntax;
    emit highligthedSyntaxChanged();
}
