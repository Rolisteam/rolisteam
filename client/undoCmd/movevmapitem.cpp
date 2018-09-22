/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "movevmapitem.h"
#include "vmap/items/visualitem.h"
#include <QDebug>

MoveItemCommand::MoveItemCommand(QList<VisualItem*> selection,QList<QPointF> oldPosition, QUndoCommand* parent)
    : QUndoCommand (parent), m_selection(selection), m_oldPoints(oldPosition)
{
    if(m_selection.size() == m_oldPoints.size())
    {
        m_valid = true;
        for(auto item : m_selection)
        {
            m_newPoints.append(item->pos());
        }
        setText(QObject::tr("Move %n item(s)","",m_selection.size()));
    }
}

void MoveItemCommand::redo()
{
    qInfo() << QStringLiteral("redo command MoveItemCommand: %1 ").arg(text());
    if(!m_valid)
        return;
    int i = 0;
    for(auto item : m_selection)
    {
        item->setPos(m_newPoints.at(i));
        ++i;
    }
}

void MoveItemCommand::undo()
{
    qInfo() << QStringLiteral("undo command MoveItemCommand: %1 ").arg(text());
    if(!m_valid)
        return;
    int i = 0;
    for(auto item : m_selection)
    {
        item->setPos(m_oldPoints.at(i));
        ++i;
    }
}
