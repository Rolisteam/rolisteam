/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "undoCmd/changestackordervmapcommand.h"

ChangeStackOrderVMapCommand::ChangeStackOrderVMapCommand(VectorialMapController* vmapCtrl,
                                                         const QList<vmap::VisualItemController*>& list,
                                                         VectorialMapController::StackOrder order)
    : m_vmapCtrl(vmapCtrl), m_order(order)
{
    qDebug() << list << list.size();
    m_data.reserve(list.size());

    /*for(auto item : list)
    {
        m_data.push_back({
            item,
            item->zOrder(),
        });
    }*/
}

void ChangeStackOrderVMapCommand::undo() {}

void ChangeStackOrderVMapCommand::redo() {}
/*void VMap::changeStackOrder(VisualItem* item, VisualItem::StackOrder op)
{
    if(nullptr == item || m_sortedItemList.size() < 2)
        return;
    int index= m_sortedItemList.indexOf(item->getId());
    if(index < 0)
        return;

if(VisualItem::FRONT == op)
{
    m_sortedItemList.append(m_sortedItemList.takeAt(index));
}
else if(VisualItem::BACK == op)
{
    m_sortedItemList.prepend(m_sortedItemList.takeAt(index));
}
else
{
    // find out insertion indexes over the stacked items
    QList<QGraphicsItem*> stackedItems= items(item->sceneBoundingRect(), Qt::IntersectsItemShape);

    int maxIndex= 0;
    int minIndex= m_sortedItemList.size() - 1;
    for(QGraphicsItem* qitem : stackedItems)
    {
        // operate only on different Content and not on sightItem.
        VisualItem* vItem= dynamic_cast<VisualItem*>(qitem);
        if(!vItem || vItem == item || vItem == m_sightItem || vItem == m_gridItem)
            continue;

        auto id= vItem->getId();
        auto tmpIndex= m_sortedItemList.indexOf(id);
        maxIndex= std::max(maxIndex, tmpIndex);
        minIndex= std::min(minIndex, tmpIndex);
    }

    if(VisualItem::RAISE == op)
    {
        m_sortedItemList.insert(maxIndex, m_sortedItemList.takeAt(index));
        // element at  index must be after all element in list
    }
    else if(VisualItem::LOWER == op)
    {
        m_sortedItemList.insert(minIndex, m_sortedItemList.takeAt(index));
        // element at  index must be before all element in list
    }
}

quint64 z= 0;
for(const QString& key : m_sortedItemList)
{
    VisualItem* item= m_itemMap->value(key);
    if(nullptr != item)
    {
        item->setZValue(++z);
    }
}

     // add grid and sight item
m_zIndex= z;
m_sightItem->setZValue(++z);
m_gridItem->setZValue(++z);
}*/
