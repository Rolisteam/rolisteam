/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include "vmap/items/anchoritem.h"
#include "vmap/items/characteritem.h"
#include "vmap/items/ellipsitem.h"
#include "vmap/items/highlighteritem.h"
#include "vmap/items/imageitem.h"
#include "vmap/items/lineitem.h"
#include "vmap/items/pathitem.h"
#include "vmap/items/rectitem.h"
#include "vmap/items/ruleitem.h"
#include "vmap/items/textitem.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/character.h"
#include "network/networkmessagewriter.h"

AddVmapItemCommand::AddVmapItemCommand(Core::SelectableTool tool, VectorialMapController* ctrl,
                                       const std::map<Core::Properties, QVariant>& properties, QPointF& pos,
                                       QColor& color, quint16 penSize, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl), m_pos(pos), m_color(color), m_penSize(penSize), m_tool(tool)
{
    switch(m_tool)
    {
    case Core::PEN:
        m_currentItem= new PathItem(properties);
        // , m_pos, m_color, m_penSize, true
        break;
    case Core::PATH:
    {
        PathItem* pathItem= new PathItem(properties);
        // , m_pos, m_color, m_penSize, true
        m_currentItem= pathItem;
        m_currentPath= pathItem;
    }
    break;
    case Core::LINE:
        m_currentItem= new LineItem(properties);
        // , m_pos, m_color, m_penSize
        break;
    case Core::EMPTYRECT:
        m_currentItem= new RectItem(properties);
        //, m_pos, m_pos, false, m_penSize, m_color
        break;
    case Core::FILLRECT:
        m_currentItem= new RectItem(properties);
        // , m_pos, m_pos, true, m_penSize, m_color
        break;
    case Core::EMPTYELLIPSE:
        m_currentItem= new EllipsItem(properties);
        // , m_pos, false, m_penSize, m_color
        break;
    case Core::FILLEDELLIPSE:
        m_currentItem= new EllipsItem(properties);
        // , m_pos, true, m_penSize, m_color
        break;
    case Core::TEXT:
    {
        /*QGraphicsItem* item= m_vmap->itemAt(m_pos, QTransform());
        TextItem* tmp= dynamic_cast<TextItem*>(item);
        QGraphicsTextItem* tmpGraph= dynamic_cast<QGraphicsTextItem*>(item);
        if((nullptr == tmp) && (nullptr == tmpGraph))
        {
            TextItem* temptext= new TextItem(m_pos, m_penSize, m_color);
            m_currentItem= temptext;
        }
        else
        {
            m_error= true;
        }*/
    }
    break;
    case Core::TEXTBORDER:
    {
        /*QGraphicsItem* item= m_vmap->itemAt(m_pos, QTransform());
        TextItem* tmp= dynamic_cast<TextItem*>(item);
        QGraphicsTextItem* tmpGraph= dynamic_cast<QGraphicsTextItem*>(item);
        if((nullptr == tmp) && (nullptr == tmpGraph))
        {
            TextItem* temptext= new TextItem(m_pos, m_penSize, m_color);
            temptext->setBorderVisible(true);
            m_currentItem= temptext;
        }
        else
        {
            m_error= true;
        }*/
    }
    break;
    case Core::HANDLER:
        break;
    case Core::ADDNPC:
    {
        /* CharacterItem* itemCharar= new CharacterItem(
             new Character(m_vmap->getCurrentNpcName(), m_color, true, m_vmap->getCurrentNpcNumber()), m_pos);
         m_currentItem= itemCharar;*/
    }
    break;
    case Core::RULE:
    {
        RuleItem* itemRule= new RuleItem(properties); //, m_pos
        itemRule->setUnit(static_cast<Core::ScaleUnit>(m_ctrl->getOption(Core::Unit).toInt()));
        itemRule->setPixelToUnit(m_ctrl->getOption(Core::GridSize).toInt() / m_ctrl->getOption(Core::Scale).toReal());
        m_currentItem= itemRule;
    }
    break;
    case Core::ANCHOR:
    {
        AnchorItem* anchorItem= new AnchorItem(properties, m_pos); //, m_pos
        m_currentItem= anchorItem;
    }
    break;
    case Core::HIGHLIGHTER:
    {
        HighlighterItem* highlighter= new HighlighterItem(properties); //, m_pos, m_penSize, m_color
        m_currentItem= highlighter;
    }
    break;
    default:
        break;
    }

    if(nullptr != m_currentItem)
    {
        /* QObject::connect(m_currentItem, SIGNAL(itemGeometryChanged(VisualItem*)), m_vmap,
                          SLOT(sendItemToAll(VisualItem*)));
         QObject::connect(m_currentItem, &VisualItem::itemRemoved, m_vmap, &VMap::removeItemFromScene);
         QObject::connect(m_currentItem, SIGNAL(duplicateItem(VisualItem*)), m_vmap, SLOT(duplicateItem(VisualItem*)));
         QObject::connect(m_currentItem, SIGNAL(itemLayerChanged(VisualItem*)), m_vmap,
                          SLOT(checkItemLayer(VisualItem*)));
         QObject::connect(m_currentItem, SIGNAL(promoteItemTo(VisualItem*, VisualItem::ItemType)), m_vmap,
                          SLOT(promoteItemInType(VisualItem*, VisualItem::ItemType)));
         QObject::connect(m_currentItem, SIGNAL(changeStackPosition(VisualItem*, VisualItem::StackOrder)), m_vmap,
                          SLOT(changeStackOrder(VisualItem*, VisualItem::StackOrder)));*///
        initItem(true);

        setText(QObject::tr("Add vmap item"));
    }
}
AddVmapItemCommand::AddVmapItemCommand(VisualItem* item, bool addMapLayer, VectorialMapController* ctrl,
                                       QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl), m_currentItem(item)
{
    initItem(addMapLayer);
}

void AddVmapItemCommand::initItem(bool addMapLayer)
{
    QObject::connect(m_currentItem, SIGNAL(itemGeometryChanged(VisualItem*)), m_vmap, SLOT(sendItemToAll(VisualItem*)));
    QObject::connect(m_currentItem, &VisualItem::itemRemoved, m_vmap, &VMap::removeItemFromScene);
    QObject::connect(m_currentItem, SIGNAL(duplicateItem(VisualItem*)), m_vmap, SLOT(duplicateItem(VisualItem*)));
    QObject::connect(m_currentItem, SIGNAL(itemLayerChanged(VisualItem*)), m_vmap, SLOT(checkItemLayer(VisualItem*)));
    QObject::connect(m_currentItem, SIGNAL(promoteItemTo(VisualItem*, VisualItem::ItemType)), m_vmap,
                     SLOT(promoteItemInType(VisualItem*, VisualItem::ItemType)));
    QObject::connect(m_currentItem, SIGNAL(changeStackPosition(VisualItem*, VisualItem::StackOrder)), m_vmap,
                     SLOT(changeStackOrder(VisualItem*, VisualItem::StackOrder)));

    m_first= true;
    // m_currentItem->setMapId(m_vmap->getId());
    // m_currentItem->setPropertiesHash(m_vmap->getPropertiesHash());
    m_currentItem->updateItemFlags();
    // if(addMapLayer)
    //    m_currentItem->setLayer(m_vmap->currentLayer());
    m_currentItem->setVisible(isVisible());
    // m_vmap->QGraphicsScene::addItem(m_currentItem);
    m_currentItem->update();
}

bool AddVmapItemCommand::getInitPoint() const
{
    return m_initPoint;
}

void AddVmapItemCommand::setInitPoint(bool initPoint)
{
    m_initPoint= initPoint;
}

bool AddVmapItemCommand::isUndoable() const
{
    return m_undoable;
}

void AddVmapItemCommand::setUndoable(bool undoable)
{
    m_undoable= undoable;
}

bool AddVmapItemCommand::isVisible()
{
    bool visible= false;
    if((m_currentItem->getType() == VisualItem::SIGHT)
       && (m_ctrl->getOption(Core::VisibilityModeProperty) != Core::FOGOFWAR))
    {
        visible= false;
    }
    else if((m_ctrl->getOption(Core::LocalIsGM).toBool())
            || (m_ctrl->getOption(Core::VisibilityModeProperty) == Core::ALL))
    {
        visible= true;
    }
    else if(static_cast<Core::VisibilityMode>(m_ctrl->getOption(Core::VisibilityModeProperty).toInt())
            == Core::FOGOFWAR)
    {
        if(m_currentItem->getType() == VisualItem::CHARACTER)
        {
            CharacterItem* charItem= dynamic_cast<CharacterItem*>(m_currentItem);
            if(nullptr != charItem)
            {
                // if(charItem->getParentId() == m_vmap->getLocalUserId())
                {
                    visible= true;
                }
            }
        }
        else
        {
            visible= true;
        }
    }
    return visible;
}
void AddVmapItemCommand::undo()
{
    if(!isUndoable())
        return;

    qInfo() << QStringLiteral("Undo command AddVmapItemCommand: %1 ").arg(text());
    // m_vmap->removeItem(m_currentItem);
    // m_vmap->update();
    // m_vmap->removeItemFromData(m_currentItem);

    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::DelItem);
    // msg.string8(m_vmap->getId());         // id map
    msg.string16(m_currentItem->getId()); // id item
    msg.sendToServer();
}
void AddVmapItemCommand::redo()
{
#ifndef UNIT_TEST
    qInfo() << QStringLiteral("Redo command AddVmapItemCommand: %1 ").arg(text());
#endif
    m_currentItem->setVisible(isVisible());
    m_currentItem->updateItemFlags();
    // m_vmap->addItemFromData(m_currentItem);
    if(m_first)
    {
        m_currentItem->initChildPointItem();
        m_first= false;
    }
    else
    {
        //   m_vmap->QGraphicsScene::addItem(m_currentItem);
    }

    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    // msg.string8(m_vmap->getId());
    msg.uint8(m_currentItem->getType());
    m_currentItem->fillMessage(&msg);
    msg.sendToServer();
}

VisualItem* AddVmapItemCommand::getItem() const
{
    return m_currentItem;
}

VisualItem* AddVmapItemCommand::getPath() const
{
    return m_currentPath;
}

bool AddVmapItemCommand::hasError() const
{
    return m_error;
}

void AddVmapItemCommand::setError(bool error)
{
    m_error= error;
}

bool AddVmapItemCommand::isNpc() const
{
    return (m_tool == Core::ADDNPC);
}

bool AddVmapItemCommand::hasToBeDeleted() const
{
    if(nullptr == m_currentItem)
        return true;
    bool result= true;

    if(m_currentItem->getType() == VisualItem::HIGHLIGHTER)
        result= false;
    return result;
}

template <class T>
T* AddVmapItemCommand::getItem() const
{
    return dynamic_cast<T*>(m_currentItem);
}
