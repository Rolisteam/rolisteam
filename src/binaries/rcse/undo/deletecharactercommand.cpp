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
#include "deletecharactercommand.h"
#include "controllers/charactercontroller.h"

DeleteCharacterCommand::DeleteCharacterCommand(int col, CharacterController* ctrl, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl), m_index(col)
{
    m_character= m_ctrl->characterSheetFromIndex(m_index);
    setText(QObject::tr("Delete Character #%1").arg(m_index));
}

void DeleteCharacterCommand::undo()
{
    m_ctrl->insertCharacter(m_index, m_character);
}

void DeleteCharacterCommand::redo()
{
    m_ctrl->removeCharacter(m_index);
}
