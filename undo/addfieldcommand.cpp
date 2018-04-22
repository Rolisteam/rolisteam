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
#include "addfieldcommand.h"
#include "field.h"
#include "tablefield.h"
#include "tablecanvasfield.h"

AddFieldCommand::AddFieldCommand(Canvas::Tool tool, Canvas* canvas,FieldModel* model,int currentPage,QPointF pos,QUndoCommand *parent)
  : QUndoCommand(parent),m_canvas(canvas),m_model(model),m_currentPage(currentPage)
{


  if(Canvas::ADDTABLE == tool)
  {
      m_field = new TableField();
      m_field->setCanvasField(new TableCanvasField(m_field));
  }
  else
  {
      m_field = new Field(pos);
  }
  m_field->setPage(m_currentPage);

  //m_canvas->addItem(m_field->getCanvasField());

  m_field->setValueFrom(CharacterSheetItem::X,pos.x());
  m_field->setValueFrom(CharacterSheetItem::Y,pos.y());

  QString type;

  //m_currentItem->setFocus();
  switch(tool)//TEXTINPUT,TEXTFIELD,TEXTAREA,SELECT,CHECKBOX,IMAGE,BUTTON
  {
  case Canvas::ADDCHECKBOX:
      m_field->setCurrentType(Field::CHECKBOX);
      type = QObject::tr("checkbox");
      break;
  case Canvas::ADDINPUT:
      m_field->setCurrentType(Field::TEXTINPUT);
      type = QObject::tr("TextInput");
      break;
  case Canvas::ADDTEXTAREA:
      m_field->setCurrentType(Field::TEXTAREA);
      type = QObject::tr("TextArea");
      break;
  case Canvas::ADDTEXTFIELD:
      m_field->setCurrentType(Field::TEXTFIELD);
      type = QObject::tr("TextField");
      break;
  case Canvas::ADDTABLE:
  {
      m_field->setCurrentType(Field::TABLE);
      type = QObject::tr("Table");
  }
      break;
  case Canvas::ADDIMAGE:
      m_field->setCurrentType(Field::IMAGE);
      type = QObject::tr("Image");
      break;
  case Canvas::ADDFUNCBUTTON:
      m_field->setCurrentType(Field::FUNCBUTTON);
      type = QObject::tr("function");
      break;
  case Canvas::BUTTON:
      m_field->setCurrentType(Field::DICEBUTTON);
      m_field->setBgColor(Qt::red);
      type = QObject::tr("Dice Button");
      break;
  case Canvas::ADDWEBPAGE:
      m_field->setCurrentType(Field::WEBPAGE);
      type = QObject::tr("Web Page");
      break;
  case Canvas::MOVE:
  case Canvas::DELETETOOL:
  case Canvas::NONE:
  default:
      type = QObject::tr("Unknown");
      break;
  }

  setText(QObject::tr("Add %1 Field").arg(type));
}

void AddFieldCommand::undo()
{
  m_canvas->removeItem(m_field->getCanvasField());
  m_canvas->update();
  m_model->removeField(m_field);

}

void AddFieldCommand::redo()
{
  m_canvas->addItem(m_field->getCanvasField());
  if(nullptr != m_model)
  {
      m_model->appendField(m_field);
  }

}

Field *AddFieldCommand::getField() const
{
  return m_field;
}
