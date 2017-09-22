#ifndef SETFIELDPROPERTYCOMMAND_H
#define SETFIELDPROPERTYCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QModelIndexList>

#include "field.h"
#include "fieldmodel.h"

class SetFieldPropertyCommand : public QUndoCommand
{
public:
  SetFieldPropertyCommand(FieldModel* model, QModelIndexList selection, QVariant value, int col, QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:
  QVariant m_newValue;
  QList<QVariant> m_oldValues;
  QModelIndexList m_selection;
  int m_col;
  FieldModel* m_model;
};

#endif // SETFIELDPROPERTYCOMMAND_H
