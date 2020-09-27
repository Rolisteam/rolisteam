#include "dragnodecommand.h"

#include "commandhelper.h"
/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "data/mindnode.h"

DragNodeCommand::DragNodeCommand(const QPointF& motion, const std::vector<QPointer<MindNode>>& selection)
    : m_motion(motion), m_mindNodes(selection)
{
}

void DragNodeCommand::undo()
{
    std::for_each(m_mindNodes.begin(), m_mindNodes.end(), [this](const QPointer<MindNode>& node) {
        if(node.isNull())
            return;
        node->translate(m_motion * -1);
    });
}

void DragNodeCommand::redo()
{
    std::for_each(m_mindNodes.begin(), m_mindNodes.end(), [this](const QPointer<MindNode>& node) {
        if(node.isNull())
            return;
        if(node->isDragged())
            return;
        node->translate(m_motion);
    });
}

int DragNodeCommand::id() const
{
    return static_cast<int>(RMindMap::CommandType::Drag);
}

bool DragNodeCommand::mergeWith(const QUndoCommand* other)
{
    if(other->id() != id())
        return false;
    auto command= dynamic_cast<const DragNodeCommand*>(other);

    if(nullptr == command)
        return false;

    if(command->getSelection() != getSelection())
        return false;

    m_motion+= command->getMotion();
    return true;
}

const QPointF& DragNodeCommand::getMotion() const
{
    return m_motion;
}
const std::vector<QPointer<MindNode>> DragNodeCommand::getSelection() const
{
    return m_mindNodes;
}
