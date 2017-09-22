#ifndef MOVEFIELDCOMMAND_H
#define MOVEFIELDCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QGraphicsItem>

class MoveFieldCommand : public QUndoCommand
{
public:
  MoveFieldCommand(QList<QGraphicsItem*> list, QList<QPointF> oldPos, QUndoCommand* parent = 0);

  void undo() override;
  void redo() override;

private:
  QList<QGraphicsItem*> m_list;
  QList<QPointF> m_oldPoints;
  QList<QPointF> m_newPoints;

};

#endif // MOVEFIELDCOMMAND_H
