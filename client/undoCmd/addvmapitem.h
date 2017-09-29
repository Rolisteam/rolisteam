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
#ifndef ADDVMAPITEMCOMMAND_H
#define ADDVMAPITEMCOMMAND_H

#include <QUndoCommand>
#include "vmap.h"

class AddVmapItemCommand : public QUndoCommand
{
public:
  AddVMapItemCommand(Canvas::Tool tool, VMap* vmap,QList<VisualItem*>* list,QPointF pos,
                  QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

  VisualItem* getItem() const;

private:
  VisualItem* m_item;
  VMap* m_map;
  QPointF m_pos;
  QList<VisualItem*>* m_list;
};

#endif // ADDFIELDCOMMAND_H
