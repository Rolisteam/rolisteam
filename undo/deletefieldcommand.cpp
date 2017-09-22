#include "deletefieldcommand.h"

DeleteFieldCommand::DeleteFieldCommand(Field* field, Canvas* graphicsScene,FieldModel* model,int currentPage,QUndoCommand *parent)
    : QUndoCommand(parent),m_field(field),m_canvas(graphicsScene),m_model(model),m_currentPage(currentPage)
{
    if(nullptr != m_field->getCanvasField())
    {
       m_parent = m_field->getParent();
       m_point = m_field->getCanvasField()->pos();
       m_posInModel = m_parent->indexOfChild(m_field);

       setText(QObject::tr("Delete %1 %2 Field").arg(field->getId()).arg(field->getLabel()));
    }
}

void DeleteFieldCommand::undo()
{
    m_canvas->addItem(m_field->getCanvasField());
    m_field->getCanvasField()->setPos(m_point);
    m_model->insertField(m_field,m_parent,m_posInModel);
}

void DeleteFieldCommand::redo()
{
    m_canvas->removeItem(m_field->getCanvasField());
    m_model->removeField(m_field);
}
