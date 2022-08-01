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
#include "additemcommand.h"

#include "mindmap/data/linkcontroller.h"
#include "mindmap/model/minditemmodel.h"

#include <QDebug>

namespace mindmap
{
AddItemCommand::AddItemCommand(mindmap::MindItemModel* nodeModel, MindItem::Type type, const QString& idParent,
                               QPointF pos)
    : m_nodeModel(nodeModel), m_idParent(idParent), m_type(type), m_pos(pos)
{
    setText(tr("Add item of type: %1 to mindmap").arg(type));
}

void AddItemCommand::undo()
{
    m_nodeModel->removeItem(m_mindItem);
    m_nodeModel->removeItem(m_link);
}

void AddItemCommand::redo()
{
    if(m_mindItem.isNull())
    {
        auto pair= m_nodeModel->addItem(m_idParent, m_type);
        if(!m_pos.isNull())
        {
            auto p= dynamic_cast<PositionedItem*>(pair.first);
            if(p)
                p->setPosition(m_pos);
        }
        m_mindItem= pair.first;
        m_link= pair.second;
    }
    else
    {
        m_nodeModel->appendItem({m_mindItem.data(), m_link});
    }
}

AddLinkCommand::AddLinkCommand(MindItemModel* nodeModel, const QString& idStart, const QString& idEnd)
    : m_nodeModel(nodeModel), m_idStart(idStart), m_idEnd(idEnd)
{
    setText(tr("Add link"));
}

void AddLinkCommand::undo()
{
    m_nodeModel->removeItem(m_link);
}

void AddLinkCommand::redo()
{
    if(m_link.isNull())
    {
        auto [item, link]= m_nodeModel->addItem({}, MindItem::LinkType);
        m_link= link;
        m_link->setStart(m_nodeModel->positionItem(m_idStart));
        m_link->setEnd(m_nodeModel->positionItem(m_idEnd));
    }
    else
    {
        m_nodeModel->appendItem({m_link});
    }
}

} // namespace mindmap
