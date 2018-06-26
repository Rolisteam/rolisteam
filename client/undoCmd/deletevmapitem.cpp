/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "deletevmapitem.h"
#include "network/networkmessagewriter.h"
#include <QGraphicsScene>

DeleteVmapItemCommand::DeleteVmapItemCommand(VMap* map, VisualItem* item, bool sendToAll, QUndoCommand *parent)
    : QUndoCommand(parent), m_vmap(map), m_currentItem(item), m_sendToAll(sendToAll)
{
    m_visible = m_currentItem->isVisible();
    m_editable = m_currentItem->isEditable();

    setText(QObject::tr("Delete Item From Map %1").arg(m_vmap->getMapTitle()));
}

void DeleteVmapItemCommand::redo()
{
    m_vmap->removeItem(m_currentItem);
    m_vmap->removeItemFromData(m_currentItem);
    m_vmap->update();

    if(m_sendToAll)
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::DelItem);
        msg.string8(m_vmap->getId());//id map
        msg.string16(m_currentItem->getId());//id item
        msg.sendToServer();
    }
}

void DeleteVmapItemCommand::undo()
{
    if(nullptr != m_currentItem)
    {
        m_vmap->setFocusItem(m_currentItem);
        m_vmap->QGraphicsScene::addItem(m_currentItem);
        m_currentItem->setVisible(m_visible);
        m_currentItem->setEditableItem(m_editable);
        m_vmap->addItemFromData(m_currentItem);
        m_vmap->update();


        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addItem);
        msg.string8(m_vmap->getId());
        msg.uint8(m_currentItem->type());
        m_currentItem->fillMessage(&msg);
        msg.sendToServer();
    }
}
