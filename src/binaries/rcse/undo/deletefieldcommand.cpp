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

DeleteFieldCommand::DeleteFieldCommand(FieldController* field, Canvas* canvas, FieldModel* model, int currentPage,
                                       QUndoCommand* parent)
    : QUndoCommand(parent), m_model(model)
{
    m_fields.append(field);
    m_canvas.append(canvas);
    m_currentPage.append(currentPage);

    init();
}
DeleteFieldCommand::DeleteFieldCommand(QList<CSItem*> fields, QList<Canvas*> canvas, FieldModel* model,
                                       QList<int> currentPage, QUndoCommand* parent)
    : QUndoCommand(parent), m_canvas(canvas), m_model(model), m_currentPage(currentPage)
{
    for(auto item : fields)
    {
        m_fields.append(dynamic_cast<FieldController*>(item));
    }
    init();
}

void DeleteFieldCommand::init()
{
    setText(QObject::tr("Delete %n Field(s)", "", m_fields.size()));
}

void DeleteFieldCommand::undo()
{
    for(auto field : m_fields)
    {
        field->setVisible(true);
        m_model->insertField(field, field->parentTreeItem(), 0);
    }
}

void DeleteFieldCommand::redo()
{
    for(auto field : m_fields)
    {
        field->setVisible(false);
        m_model->removeField(field);
    }
}
