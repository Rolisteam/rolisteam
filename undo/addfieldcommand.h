#ifndef ADDFIELDCOMMAND_H
#define ADDFIELDCOMMAND_H

#include <QUndoCommand>
#include "canvas.h"

class AddFieldCommand : public QUndoCommand
{
public:
  AddFieldCommand(Canvas::Tool tool, Canvas* graphicsScene,FieldModel* model,int currentPage,QPointF pos,
                  QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

  Field* getField() const;

private:
  Field* m_field;
  Canvas* m_canvas;
  QPointF initialPosition;
  FieldModel* m_model;
  int m_currentPage;
};

#endif // ADDFIELDCOMMAND_H
