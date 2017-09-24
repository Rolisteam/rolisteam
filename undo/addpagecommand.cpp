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
#include "addpagecommand.h"
QStringListModel* AddPageCommand::m_pagesModel = new QStringListModel();

AddPageCommand::AddPageCommand(int currentPage, QList<Canvas*>& list,Canvas::Tool tool,QUndoCommand *parent)
  : QUndoCommand(parent),m_currentPage(currentPage),m_list(list),m_currentTool(tool)
{
    m_canvas = new Canvas();
    m_canvas->setCurrentTool(m_currentTool);
    m_canvas->setCurrentPage(m_currentPage);
    setText(QObject::tr("Add Page #%1").arg(m_currentPage));
}

void AddPageCommand::undo()
{
    m_list.removeAt(m_currentPage);
    QStringList str = m_pagesModel->stringList();
    str.removeAt(m_currentPage);
    m_pagesModel->setStringList(str);
}

void AddPageCommand::redo()
{
    m_list.insert(m_currentPage,m_canvas);
    QStringList str = m_pagesModel->stringList();
    str.insert(m_currentPage,QObject::tr("Page %1").arg(m_currentPage+1));
    m_pagesModel->setStringList(str);
}

Canvas *AddPageCommand::canvas() const
{
    return m_canvas;
}

QStringListModel* AddPageCommand::getPagesModel()
{
    return m_pagesModel;
}

void AddPageCommand::setPagesModel(QStringListModel* pagesModel)
{
    m_pagesModel = pagesModel;

}
