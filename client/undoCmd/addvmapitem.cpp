/***************************************************************************
    *	 Copyright (C) 2017 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
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
#include "addvmapitem.h"

#include "vmap/items/rectitem.h"
#include "vmap/items/ellipsitem.h"
#include "vmap/items/pathitem.h"
#include "vmap/items/lineitem.h"
#include "vmap/items/textitem.h"
#include "vmap/items/characteritem.h"
#include "vmap/items/ruleitem.h"
#include "vmap/items/imageitem.h"
#include "vmap/items/anchoritem.h"

#include "data/character.h"


AddVmapItemCommand::AddVmapItemCommand(VToolsBar::SelectableTool tool,
                                       VMap* vmap,
                                       QPointF& pos,
                                       QColor& color,
                                       int penSize,
                                       QUndoCommand *parent)
    : QUndoCommand(parent),
      m_vmap(vmap),
      m_pos(pos),
      m_color(color),
      m_penSize(penSize),
      m_tool(tool)
{

    switch(m_tool)
    {
    case VToolsBar::PEN:
        m_currentItem=new PathItem(m_pos,m_color,m_penSize,true);
        break;
    case VToolsBar::PATH:
    {
        PathItem* pathItem=new PathItem(m_pos,m_color,m_penSize);
        m_currentItem = pathItem;
        m_currentPath = pathItem;
    }
        break;
    case VToolsBar::LINE:
        m_currentItem= new LineItem(m_pos,m_color,m_penSize);
        break;
    case VToolsBar::EMPTYRECT:
        m_currentItem = new RectItem(m_pos,m_pos,false,m_penSize,m_color);
        break;
    case VToolsBar::FILLRECT:
        m_currentItem = new RectItem(m_pos,m_pos,true,m_penSize,m_color);
        break;
    case VToolsBar::EMPTYELLIPSE:
        m_currentItem = new EllipsItem(m_pos,false,m_penSize,m_color);
        break;
    case VToolsBar::FILLEDELLIPSE:
        m_currentItem = new EllipsItem(m_pos,true,m_penSize,m_color);
        break;
    case VToolsBar::TEXT:
    {
        QGraphicsItem* item = m_vmap->itemAt(m_pos,QTransform());
        TextItem* tmp = dynamic_cast<TextItem*>(item);
        QGraphicsTextItem* tmpGraph = dynamic_cast<QGraphicsTextItem*>(item);
        if((nullptr==tmp)&&(nullptr==tmpGraph))
        {
            TextItem* temptext = new TextItem(m_pos,m_penSize,m_color);
            m_currentItem = temptext;
        }
        else
        {
            m_error = true;
        }

    }
        break;
    case VToolsBar::TEXTBORDER:
    {
        QGraphicsItem* item = m_vmap->itemAt(m_pos,QTransform());
        TextItem* tmp = dynamic_cast<TextItem*>(item);
        QGraphicsTextItem* tmpGraph = dynamic_cast<QGraphicsTextItem*>(item);
        if((nullptr==tmp)&&(nullptr==tmpGraph))
        {
            TextItem* temptext = new TextItem(m_pos,m_penSize,m_color);
            temptext->setBorderVisible(true);
            m_currentItem = temptext;
        }
        else
        {
            m_error = true;
        }
    }
        break;
    case VToolsBar::HANDLER:
        break;
    case VToolsBar::ADDNPC:
    {
        CharacterItem* itemCharar= new CharacterItem(new Character(m_vmap->getCurrentNpcName(),m_color,true,m_vmap->getCurrentNpcNumber()),m_pos);
        m_currentItem = itemCharar;

    }
        break;
    case VToolsBar::RULE:
    {
        RuleItem* itemRule = new RuleItem(m_pos);
        itemRule->setUnit(static_cast<VMap::SCALE_UNIT>(m_vmap->getOption(VisualItem::Unit).toInt()));
        itemRule->setPixelToUnit(m_vmap->getOption(VisualItem::GridSize).toInt()/m_vmap->getOption(VisualItem::Scale).toReal());
        m_currentItem = itemRule;
    }
        break;
    case VToolsBar::ANCHOR:
    {
        AnchorItem* anchorItem = new AnchorItem(m_pos);
        m_currentItem = anchorItem;
    }
        break;
    }

    QObject::connect(m_currentItem,SIGNAL(itemGeometryChanged(VisualItem*)),m_vmap,SLOT(sendItemToAll(VisualItem*)));
    QObject::connect(m_currentItem,SIGNAL(itemRemoved(QString)),m_vmap,SLOT(removeItemFromScene(QString)));
    QObject::connect(m_currentItem,SIGNAL(duplicateItem(VisualItem*)),m_vmap,SLOT(duplicateItem(VisualItem*)));
    QObject::connect(m_currentItem,SIGNAL(itemLayerChanged(VisualItem*)),m_vmap,SLOT(checkItemLayer(VisualItem*)));
    //QObject::connect(m_currentItem,SIGNAL(itemPositionHasChanged()),m_vmap,SLOT(selectionPositionHasChanged()));
    //QObject::connect(m_currentItem,SIGNAL(itemPositionAboutToChange()),m_vmap,SLOT(selectionPositionAboutToChange()));

    QObject::connect(m_currentItem,SIGNAL(promoteItemTo(VisualItem*,VisualItem::ItemType)),m_vmap,SLOT(promoteItemInType(VisualItem*,VisualItem::ItemType)));
    QObject::connect(m_currentItem,SIGNAL(changeStackPosition(VisualItem*,VisualItem::StackOrder)),m_vmap,SLOT(changeStackOrder(VisualItem*,VisualItem::StackOrder)));

    initItem();

    setText(QObject::tr("Add vmap item"));
}
AddVmapItemCommand::AddVmapItemCommand(VisualItem* item,VMap* map,QUndoCommand *parent)
    : QUndoCommand(parent),m_currentItem(item),m_vmap(map)
{
    initItem();
}

void AddVmapItemCommand::initItem()
{
    m_first = true;
    m_currentItem->setPropertiesHash(m_vmap->getPropertiesHash());
    m_currentItem->setLayer(m_vmap->getCurrentLayer());
    m_currentItem->setMapId(m_vmap->getId());
    m_vmap->QGraphicsScene::addItem(m_currentItem);
}

bool AddVmapItemCommand::isUndoable() const
{
    return m_undoable;
}

void AddVmapItemCommand::setUndoable(bool undoable)
{
    m_undoable = undoable;
}

bool AddVmapItemCommand::isEditable()
{
    bool editable = false;
    if((m_vmap->getOption(VisualItem::LocalIsGM).toBool()))//GM
    {
        editable = true;
    }
    else
    {
        if(m_vmap->getPermissionMode() == Map::PC_ALL)
        {
            editable = true;
        }
        else if(m_vmap->getPermissionMode() == Map::PC_MOVE)
        {
            if(m_currentItem->getType()==VisualItem::CHARACTER)
            {
                CharacterItem* charItem = dynamic_cast<CharacterItem*>(m_currentItem);
                if(nullptr != charItem )
                {
                    if(charItem->getParentId() == m_vmap->getLocalUserId())
                    {
                        editable = true;
                        charItem->setCharacterIsMovable(true);
                    }
                }
            }
        }
    }
    return editable;
}
bool AddVmapItemCommand::isVisible()
{
    bool visible = false;
    if((m_vmap->getOption(VisualItem::LocalIsGM).toBool())||(m_vmap->getOption(VisualItem::VisibilityMode) == VMap::ALL))
    {
        visible = true;
    }
    else if(static_cast<VMap::VisibilityMode>(m_vmap->getOption(VisualItem::VisibilityMode).toInt()) == VMap::FOGOFWAR)
    {
        if(m_currentItem->getType()==VisualItem::CHARACTER)
        {
            CharacterItem* charItem = dynamic_cast<CharacterItem*>(m_currentItem);
            if(nullptr != charItem )
            {
                if(charItem->getParentId() == m_vmap->getLocalUserId())
                {
                    visible = true;
                }
            }
        }
    }
    return visible;
}
void AddVmapItemCommand::undo()
{
    m_vmap->removeItem(m_currentItem);
    m_vmap->update();
    m_vmap->removeItemFromData(m_currentItem);

    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::DelItem);
    msg.string8(m_vmap->getId());//id map
    msg.string16(m_currentItem->getId());//id item
    msg.sendAll();
}
void AddVmapItemCommand::redo()
{
    m_vmap->setFocusItem(m_currentItem);
    m_currentItem->setVisible(isVisible());
    m_currentItem->setEditableItem(isEditable());
    m_vmap->addItemFromData(m_currentItem);
    if(m_first)
    {
        m_currentItem->initChildPointItem();
        m_first = false;
    }
    else
    {
        m_vmap->QGraphicsScene::addItem(m_currentItem);
    }

    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addItem);
    msg.string8(m_vmap->getId());
    msg.uint8(m_currentItem->getType());
    m_currentItem->fillMessage(&msg);
    msg.sendAll();

}

VisualItem* AddVmapItemCommand::getItem() const
{
    return m_currentItem;
}

VisualItem *AddVmapItemCommand::getPath() const
{
    return m_currentPath;
}

bool AddVmapItemCommand::hasError() const
{
    return m_error;
}

void AddVmapItemCommand::setError(bool error)
{
    m_error = error;
}

bool AddVmapItemCommand::isNpc() const
{
    return (m_tool == VToolsBar::ADDNPC);
}
template <class T>
T* AddVmapItemCommand::getItem() const
{
    return dynamic_cast<T*>(m_currentItem);
}
