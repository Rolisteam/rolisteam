/***************************************************************************
*	Copyright (C) 2007 by Romain Campioni                                  *
*	Copyright (C) 2009 by Renaud Guezennec                                 *
*   http://renaudguezennec.homelinux.org/accueil,3.html                    *
*                                                                          *
*   rolisteam is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
*   This program is distributed in the hope that it will be useful,        *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
*                                                                          *
*   You should have received a copy of the GNU General Public License      *
*   along with this program; if not, write to the                          *
*   Free Software Foundation, Inc.,                                        *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
***************************************************************************/


#include <QtGui>

#include "MapFrame.h"




MapFrame::MapFrame(Map *map)
    : SubMdiWindows(),m_map(map)
{

    m_graphicView = new QGraphicsView(map);
    setWindowTitle(m_map->mapTitle());
    setGeometry(0,0,m_map->mapWidth(),map->mapHeight());

    setWidget(m_graphicView);

}


MapFrame::~MapFrame()
{

}


void MapFrame::closeEvent(QCloseEvent *event)
{

    hide();
    event->ignore();
}


Map * MapFrame::map()
{
    return m_map;
}


void MapFrame::startMoving(QPoint position)
{
    startingPoint = position;
    horizontalStart = m_graphicView->horizontalScrollBar()->value();
    verticalStart = m_graphicView->verticalScrollBar()->value();
}


void MapFrame::Moving(QPoint position)
{
    QPoint diff = startingPoint - position;
    m_graphicView->horizontalScrollBar()->setValue(horizontalStart + diff.x());
    m_graphicView->verticalScrollBar()->setValue(verticalStart + diff.y());
}
void MapFrame::currentCursorChanged(QCursor* cursor)
{
    m_currentCursor = cursor;
    m_graphicView->setCursor(*m_currentCursor);
}
void MapFrame::currentToolChanged(ToolsBar::SelectableTool selectedtool)
{
    m_currentTool = selectedtool;
    if(m_map != NULL)
        m_map->setCurrentTool(m_currentTool);

}
