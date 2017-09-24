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
#include "setfieldproperties.h"

SetFieldPropertyCommand::SetFieldPropertyCommand(FieldModel* model,QList<CharacterSheetItem*> selection, QVariant value, int col, QUndoCommand *parent)
  : QUndoCommand(parent),m_newValue(value),m_selection(selection),m_col(col),m_model(model)
{
    for(auto index : m_selection)
    {
        m_oldValues.append(index->getValueFrom(static_cast<CharacterSheetItem::ColumnId>(m_col),Qt::EditRole));
    }
    setText(QObject::tr("Set Property %1").arg(m_newValue.toString()));
}
void SetFieldPropertyCommand::undo()
{
    int i = 0;
    for(auto index : m_selection)
    {
        index->setValueFrom(static_cast<CharacterSheetItem::ColumnId>(m_col),m_oldValues.at(i));
        ++i;
    }
}
void SetFieldPropertyCommand::redo()
{
    for(auto index : m_selection)
    {
        index->setValueFrom(static_cast<CharacterSheetItem::ColumnId>(m_col),m_newValue);
    }
}
