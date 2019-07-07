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
#ifndef DELETEFIELDCOMMAND_H
#define DELETEFIELDCOMMAND_H

#include <QUndoCommand>
#include "canvas.h"
#include <QList>

class DeleteFieldCommand : public QUndoCommand
{
public:
  DeleteFieldCommand(Field* fields, Canvas* canvas, FieldModel* model,int currentPage,QUndoCommand *parent = 0);
  DeleteFieldCommand(QList<CharacterSheetItem*> fields, QList<Canvas*> canvas, FieldModel* model,QList<int> currentPage,QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:
  void init();
private:
  QList<Field*> m_fields;
  QList<Canvas*> m_canvas;
  QList<QPointF> m_points;
  FieldModel* m_model;
  QList<int> m_currentPage;
  QList<int> m_posInModel;
  QList<CharacterSheetItem*> m_parent;
};

#endif // DELETEFIELDCOMMAND_H
