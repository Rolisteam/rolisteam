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
#include "deletepagecommand.h"
#include "controllers/editorcontroller.h"

DeletePageCommand::DeletePageCommand(EditorController* ctrl, FieldModel* model, QUndoCommand* parent)
    : QUndoCommand(parent), m_pageId(ctrl->currentPage()), m_ctrl(ctrl), m_model(model)
{
    // m_canvas = m_list[m_currentPage];

    m_model->getFieldFromPage(m_pageId, m_subField);
    for(auto item : m_subField)
    {
        auto field= dynamic_cast<Field*>(item);
        m_subCanvas.append(dynamic_cast<Canvas*>(field->getCanvasField()->scene()));
        m_subCurrentPage.append(field->getPage());
    }
    m_deleteField= new DeleteFieldCommand(m_subField, m_subCanvas, m_model, m_subCurrentPage, this);

    setText(QObject::tr("Delete Page #%1").arg(m_pageId + 1));
}
void DeletePageCommand::undo()
{
    m_ctrl->insertPage(m_pageId, m_canvas.release());
    /*m_list.insert(m_currentPage,m_canvas);
    QStringList str = m_pagesModel->stringList();
    str.insert(m_currentPage,QObject::tr("Page %1").arg(m_currentPage+1));
    m_pagesModel->setStringList(str);*/

    QUndoCommand::undo();
}

void DeletePageCommand::redo()
{
    Q_ASSERT(!m_canvas);
    m_canvas.reset(m_ctrl->removePage(m_pageId));
    /*m_list.removeOne(m_canvas);
    QStringList str = m_pagesModel->stringList();
    str.removeAt(m_currentPage);
    m_pagesModel->setStringList(str);*/

    QUndoCommand::redo();
}

/*Canvas* DeletePageCommand::canvas() const
{
    return m_canvas;
}

QStringListModel* DeletePageCommand::getPagesModel()
{
    return m_pagesModel;
}

void DeletePageCommand::setPagesModel(QStringListModel* pagesModel)
{
    m_pagesModel = pagesModel;

}*/
