#include "setfieldpropertycommand.h"

SetFieldPropertyCommand::AddPageCommand(QList<Field*> selection, QVariant value, CharacterSheetItem::ColumnId col, QUndoCommand *parent)
  : QUndoCommand(parent),m_newValue(value),m_selection(selection),m_col(col)
{
    for(Field* field : selection)
    {
    	m_oldValues.append(field->getValueFrom(m_col,Qt::EditRole));
	}	
    setText(QObject::tr("Delete Character #%1").arg(index));
}
void SetFieldPropertyCommand::undo()
{
    int i = 0;
    for(Field* field : selection)
    {
        field->setValueFrom(m_col,m_selection.at(i));
        ++i;
	}
}
void SetFieldPropertyCommand::redo()
{
    for(Field* field : selection)
    {
        field->setValueFrom(m_col,m_newValue);
	}
}
