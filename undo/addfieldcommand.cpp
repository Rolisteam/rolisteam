#include "addfieldcommand.h"
#include "field.h"
#include "tablecanvasfield.h"

AddFieldCommand::AddFieldCommand(Canvas::Tool tool, Canvas* canvas,FieldModel* model,int currentPage,QPointF pos,QUndoCommand *parent)
  : QUndoCommand(parent),m_canvas(canvas),m_model(model),m_currentPage(currentPage)
{

  m_field = new Field(pos);
  if(Canvas::ADDTABLE == tool)
  {
      m_field->setCanvasField(new TableCanvasField(m_field));
  }

  m_canvas->addItem(m_field->getCanvasField());
  if(nullptr != m_model)
  {
      m_model->appendField(m_field);
  }
  m_field->setValueFrom(CharacterSheetItem::X,pos.x());
  m_field->setValueFrom(CharacterSheetItem::Y,pos.y());

  //m_currentItem->setFocus();
  switch(tool)//TEXTINPUT,TEXTFIELD,TEXTAREA,SELECT,CHECKBOX,IMAGE,BUTTON
  {
  case Canvas::ADDCHECKBOX:
      m_field->setCurrentType(Field::CHECKBOX);
      break;
  case Canvas::ADDINPUT:
      m_field->setCurrentType(Field::TEXTINPUT);
      break;
  case Canvas::ADDTEXTAREA:
      m_field->setCurrentType(Field::TEXTAREA);
      break;
  case Canvas::ADDTEXTFIELD:
      m_field->setCurrentType(Field::TEXTFIELD);
      break;
  case Canvas::ADDTABLE:
  {
      m_field->setCurrentType(Field::TABLE);
  }
      break;
  case Canvas::ADDIMAGE:
      m_field->setCurrentType(Field::IMAGE);
      break;
  case Canvas::ADDFUNCBUTTON:
      m_field->setCurrentType(Field::FUNCBUTTON);
      break;
  case Canvas::BUTTON:
      m_field->setCurrentType(Field::BUTTON);
      m_field->setBgColor(Qt::red);
      break;
  case Canvas::MOVE:
  case Canvas::DELETETOOL:
  case Canvas::NONE:
      break;
  }
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
  m_model->appendField(m_field);

}

Field *AddFieldCommand::getField() const
{
  return m_field;
}
