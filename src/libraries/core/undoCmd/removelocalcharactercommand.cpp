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
#include "removelocalcharactercommand.h"

#include "data/character.h"
#include "model/playermodel.h"

RemoveLocalCharacterCommand::RemoveLocalCharacterCommand(PlayerModel* model, const QModelIndex& index)
    : m_model(model)
    , m_parent(index.parent())
    , m_character(dynamic_cast<Character*>(index.data(PlayerModel::PersonPtrRole).value<Person*>()))
    , m_pos(index.row())
{
    setText(QObject::tr("Remove character %1").arg(index.data().toString()));
}

void RemoveLocalCharacterCommand::redo()
{
    m_model->removeCharacter(m_character);
}

void RemoveLocalCharacterCommand::undo()
{
    m_model->addCharacter(m_parent, m_character, m_pos);
}
