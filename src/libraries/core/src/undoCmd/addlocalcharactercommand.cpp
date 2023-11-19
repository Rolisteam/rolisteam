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
#include "undoCmd/addlocalcharactercommand.h"

#include "data/character.h"
#include "model/playermodel.h"

AddLocalCharacterCommand::AddLocalCharacterCommand(PlayerModel* model, const QString& stateId, const QModelIndex& index)
    : m_model(model), m_index(index),m_stateId(stateId)
{
    setText(QObject::tr("Add character to %1").arg(index.data().toString()));
}

void AddLocalCharacterCommand::redo()
{
    if(!m_character)
    {
        m_character=new Character();
        m_character->setName(QObject::tr("Unknown Character"));
        m_character->setStateId(m_stateId);
    }
    m_model->addCharacter(m_index, m_character.data());
}

void AddLocalCharacterCommand::undo()
{
    m_model->removeCharacter(m_character.data());
}
