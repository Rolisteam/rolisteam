/***************************************************************************
  *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
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
#include "submdiwindows.h"
#include <QDebug>
#include "preferencesmanager.h"
SubMdiWindows::SubMdiWindows(QWidget* parent)
    : QMdiSubWindow(parent)
{

    m_currentTool = ToolsBar::HANDLER;
    m_options = PreferencesManager::getInstance();
}

void SubMdiWindows::changedStatus(Qt::WindowStates oldState,Qt::WindowStates newState)
{
    Q_UNUSED(oldState);
    switch(newState)
    {
        case Qt::WindowActive :
                m_active=true;
                break;
        default:
               m_active=false;
    }
}

void SubMdiWindows::currentToolChanged(ToolsBar::SelectableTool selectedtool)
{
    m_currentTool = selectedtool;

}
void SubMdiWindows::currentCursorChanged(QCursor* cursor)
{
    m_currentCursor = cursor;

}
void SubMdiWindows::currentColorChanged(QColor& penColor)
{
m_penColor = penColor;

}
void SubMdiWindows::currentPenSizeChanged(int pensize)
{
    m_penSize =pensize;
}
void SubMdiWindows::currentNPCSizeChanged(int npc)
{
    m_npcSize = npc;
}
SubMdiWindows::SubWindowType SubMdiWindows::getType()
{
    return m_type;
}
