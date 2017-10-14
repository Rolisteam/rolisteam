#ifndef MOVEITEMCOMMAND_H
#define MOVEITEMCOMMAND_H
#include <QPointF>
class VisualItem;

class MoveItemCommand : public QUndoCommand
{

public:
    MoveItemCommand(VisualItem* item,QPointF newPos);

	void redo() override;
	void undo() override;

private:
    VisualItem* m_item;
	QPointF m_newPos;
	QPointF m_oldPos;
};

#endif //MOVEITEMCOMMAND_H
