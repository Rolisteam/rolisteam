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
#ifndef SETFIELDPROPERTYCOMMAND_H
#define SETFIELDPROPERTYCOMMAND_H

#include <QList>
#include <QModelIndexList>
#include <QUndoCommand>

#include "charactersheet/controllers/fieldcontroller.h"
#include "fieldmodel.h"

class SetFieldPropertyCommand : public QUndoCommand
{
public:
    SetFieldPropertyCommand(QList<TreeSheetItem*> selection, QVariant value, int col,
                            QUndoCommand* parent= nullptr);

    void undo() override;
    void redo() override;

private:
    QVariant m_newValue;
    QList<QVariant> m_oldValues;
    QList<TreeSheetItem*> m_selection;
    int m_col;
};

#endif // SETFIELDPROPERTYCOMMAND_H
