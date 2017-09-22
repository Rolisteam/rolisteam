#ifndef DELETEFIELDCOMMAND_H
#define DELETEFIELDCOMMAND_H

#include <QUndoCommand>
#include "canvas.h"

class DeleteFieldCommand : public QUndoCommand
{
public:
  DeleteFieldCommand(Field* field, Canvas* graphicsScene, FieldModel* model,int currentPage,QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:
  Field* m_field;
  Canvas* m_canvas;
  QPointF m_point;
  FieldModel* m_model;
  int m_currentPage;
  int m_posInModel;
  CharacterSheetItem* m_parent;
};

#endif // DELETEFIELDCOMMAND_H
