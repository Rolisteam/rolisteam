#include "movefieldcommand.h"

MoveFieldCommand::MoveFieldCommand(QList<QGraphicsItem*> list, QList<QPointF> oldPos, QUndoCommand* parent)
    : QUndoCommand(parent),m_list(list),m_oldPoints(oldPos)
{
    if(m_list.size() == m_oldPoints.size())
    {
        for(auto item : m_list)
        {
            m_newPoints.append(item->pos());
        }
    }
    setText(QObject::tr("Move %n Field(s)","",m_list.size()));
}

void MoveFieldCommand::undo()
{
    int i = 0;
    for(auto item : m_list)
    {
        item->setPos(m_oldPoints.at(i));
        ++i;
    }
}

void MoveFieldCommand::redo()
{
    int i = 0;
    for(auto item : m_list)
    {
        item->setPos(m_newPoints.at(i));
        ++i;
    }
}
