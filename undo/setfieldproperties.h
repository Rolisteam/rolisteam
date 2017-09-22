#ifndef SETFIELDPROPERTYCOMMAND_H
#define SETFIELDPROPERTYCOMMAND_H

#include <QUndoCommand>
#include <QList>

#include "field.h"

class SetFieldPropertyCommand : public QUndoCommand
{
public:
  SetFieldPropertyCommand(QModelIndexList selection, QVariant value, Column col, QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:
  QVariant m_newValue;
  QList<QVariant> m_oldValues
  QModelIndexList m_list;
  CharacterSheetItem::ColumnId m_col;
};

#endif // SETFIELDPROPERTYCOMMAND_H
