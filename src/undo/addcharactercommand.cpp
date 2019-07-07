/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "addcharactercommand.h"

#include "controllers/charactercontroller.h"

AddCharacterCommand::AddCharacterCommand(CharacterController* ctrl, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl)
{
    setText(QObject::tr("Add Character #%1").arg(0));
}

void AddCharacterCommand::undo()
{
    m_ctrl->removeCharacter(m_ctrl->characterCount());
}

void AddCharacterCommand::redo()
{
    m_ctrl->addCharacter();
}
