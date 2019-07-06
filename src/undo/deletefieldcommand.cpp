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
#include "deletefieldcommand.h"

DeleteFieldCommand::DeleteFieldCommand(Field* field, Canvas* canvas,FieldModel* model,int currentPage,QUndoCommand *parent)
    : QUndoCommand(parent),m_model(model)
{
    m_fields.append(field);
    m_canvas.append(canvas);
    m_currentPage.append(currentPage);

    init();

}
DeleteFieldCommand::DeleteFieldCommand(QList<CharacterSheetItem*> fields, QList<Canvas*> canvas, FieldModel* model,QList<int> currentPage,QUndoCommand *parent)
    : QUndoCommand(parent),m_canvas(canvas),m_model(model),m_currentPage(currentPage)
{
    for(auto item : fields)
    {
        m_fields.append(dynamic_cast<Field*>(item));
    }
    init();
}


void DeleteFieldCommand::init()
{
    for(auto field : m_fields)
    {
        if(nullptr != field->getCanvasField())
        {
            auto parent = field->getParent();
            m_parent.append(field->getParent());
            m_points.append(field->getCanvasField()->pos());
            if(nullptr != parent)
                m_posInModel.append(parent->indexOfChild(field));
        }
    }

    setText(QObject::tr("Delete %n Field(s)","",m_fields.size()));
}

void DeleteFieldCommand::undo()
{
    for(int i = 0; i < m_fields.size(); ++i)
    {
        m_canvas[i]->addItem(m_fields[i]->getCanvasField());
        m_fields[i]->getCanvasField()->setPos(m_points[i]);
        m_model->insertField(m_fields[i],m_parent[i],m_posInModel[i]);
    }
}

void DeleteFieldCommand::redo()
{
    for(int i = 0; i < m_fields.size(); ++i)
    {
        m_canvas[i]->removeItem(m_fields[i]->getCanvasField());
        m_model->removeField(m_fields[i]);
    }
}
