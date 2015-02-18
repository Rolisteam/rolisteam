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
#include "colorselector.h"



MapFrame::MapFrame(Map *map)
    : SubMdiWindows(),m_map(map)
{

    m_widgetLayout = new QWidget;
    m_graphicView = new RGraphicsView(map);
    m_toolsbar = ToolsBar::getInstance();

    connect(m_toolsbar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),this,SLOT(currentToolChanged(ToolsBar::SelectableTool)));
    connect(m_toolsbar,SIGNAL(currentColorChanged(QColor&)),this,SLOT(currentColorChanged(QColor&)));
    connect(m_toolsbar,SIGNAL(currentModeChanged(int)),this,SIGNAL(currentModeChanged(int)));
    connect(m_toolsbar,SIGNAL(currentPenSizeChanged(int)),this,SLOT(currentPenSizeChanged(int)));
    connect(m_toolsbar,SIGNAL(currentPNCSizeChanged(int)),this,SLOT(currentNPCSizeChanged(int)));

    m_vlayout= new QVBoxLayout();
    m_hlayout = new QHBoxLayout();
    m_type = SubMdiWindows::MAP;

    m_vlayout->addStretch(1);
    m_vlayout->addWidget(m_graphicView);
    m_vlayout->addStretch(1);
    m_vlayout->setContentsMargins(0,0,0,0);

    m_hlayout->addStretch(1);
    m_hlayout->addLayout(m_vlayout);
    m_hlayout->addStretch(1);
    m_hlayout->setContentsMargins(0,0,0,0);

    if(m_map->mapTitle().size()>0)
        setWindowTitle(m_map->mapTitle());
    else
        setWindowTitle(tr("Untitled Map"));

    m_graphicView->setGeometry(0,0,m_map->mapWidth(),map->mapHeight());
    m_widgetLayout->setLayout(m_hlayout);

    setWidget(m_widgetLayout);
    setWindowIcon(QIcon(":/resources/icons/map.png"));

    m_maskPixmap = new QPixmap(m_graphicView->size());

    m_currentEditingMode=ColorSelector::NORMAL;

}


MapFrame::~MapFrame()
{
    delete m_widgetLayout;
    delete m_graphicView;
    delete m_hlayout;
    delete m_vlayout;
    delete m_maskPixmap;

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
int MapFrame::editingMode()
{
    return m_currentEditingMode;
}

void MapFrame::startMoving(QPoint position)
{
    /// @todo check if this method is still used some where
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
void MapFrame::mousePressEvent(QMouseEvent* event)
{
/**
  @todo : stop the event when we are not in normal editing mode and make appropriate actions.
  */


    if(m_currentEditingMode != ColorSelector::NORMAL)
    {
           event->ignore();
    }
    else
        SubMdiWindows::mousePressEvent(event);
}

void MapFrame::paintEvent(QPaintEvent* event)
{
    /*if(m_currentEditingMode != ColorSelector::NORMAL)
    {
        event->accept();
        
    }
    else*/
        SubMdiWindows::paintEvent(event);
}

void MapFrame::currentPenSizeChanged(int ps)
{
    if(m_map != NULL)
        m_map->setPenSize(ps);
}
void MapFrame::currentNPCSizeChanged(int ps)
{
    if(m_map != NULL)
        m_map->setNPCSize(ps);
}

void MapFrame::currentColorChanged(QColor& penColor)
{
    m_penColor = penColor;

    if(m_map !=NULL)
        m_map->setCurrentChosenColor(m_penColor);
}
void MapFrame::setEditingMode(int mode)
{
    m_currentEditingMode = mode;
}
bool MapFrame::defineMenu(QMenu* /*menu*/)
{
    return false;
}
void MapFrame::saveFile(QString & file)
{
    if(!file.isEmpty())
    {

    }
}

void MapFrame::openFile(QString& file)
{
    if(!file.isEmpty())
    {

    }
}

bool MapFrame::hasDockWidget() const
{
    return true;
}
QDockWidget* MapFrame::getDockWidget()
{
    return m_toolsbar;
}
