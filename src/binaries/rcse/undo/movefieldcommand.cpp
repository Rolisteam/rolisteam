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
#include "movefieldcommand.h"

MoveFieldCommand::MoveFieldCommand(QList<QGraphicsItem*> list, QList<QPointF> oldPos, QUndoCommand* parent)
    : QUndoCommand(parent),m_list(list),m_oldPoints(oldPos)
{
    if(m_list.size() == m_oldPoints.size())
    {
        for(auto item : m_list)
        {
            m_newPoints.append(item->pos());
        }
    }
    setText(QObject::tr("Move %n Field(s)","",m_list.size()));
}

void MoveFieldCommand::undo()
{
    int i = 0;
    for(auto item : m_list)
    {
        item->setPos(m_oldPoints.at(i));
        ++i;
    }
}

void MoveFieldCommand::redo()
{
    int i = 0;
    for(auto item : m_list)
    {
        item->setPos(m_newPoints.at(i));
        ++i;
    }
}
