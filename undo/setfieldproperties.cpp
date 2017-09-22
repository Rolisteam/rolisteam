#include "setfieldproperties.h"

SetFieldPropertyCommand::SetFieldPropertyCommand(FieldModel* model,QModelIndexList selection, QVariant value, int col, QUndoCommand *parent)
  : QUndoCommand(parent),m_newValue(value),m_selection(selection),m_col(col),m_model(model)
{
    for(QModelIndex index : m_selection)
    {
        if(m_col == index.column())
        {
          m_oldValues.append(m_model->data(index,Qt::EditRole));
        }
    }
    setText(QObject::tr("Set Property %1").arg(m_newValue.toString()));
}
void SetFieldPropertyCommand::undo()
{
    int i = 0;
    for(QModelIndex index : m_selection)
    {
      if(index.column() == m_col)
      {
        m_model->setData(index,m_oldValues.at(i),Qt::EditRole);
        ++i;
      }
    }
}
void SetFieldPropertyCommand::redo()
{
    for(QModelIndex index : m_selection)
    {
      if(index.column() == m_col)
      {
        m_model->setData(index,m_newValue,Qt::EditRole);
      }
    }
}
