/***************************************************************************
 *	 Copyright (C) 2018 by Renaud Guezennec                                *
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
#include "changesizevmapitem.h"

#include "vmap/items/visualitem.h"

ChangeSizeVmapItemCommand::ChangeSizeVmapItemCommand(
    VMap* vmap, RGraphicsView::Method method, QPointF mousePos, QUndoCommand* parent)
    : QUndoCommand(parent), m_vmap(vmap)
{
    if(nullptr != m_vmap)
    {
        auto list= m_vmap->selectedItems();
        QSizeF finalRect;
        if(RGraphicsView::UnderMouse == method)
        {
            auto items= m_vmap->items(mousePos);
            for(auto item : items)
            {
                auto vItem= dynamic_cast<VisualItem*>(item);
                if(vItem != nullptr)
                {
                    if(vItem->isVisible() && vItem->getType() != VisualItem::GRID
                        && vItem->getType() != VisualItem::SIGHT)
                    {
                        finalRect= vItem->boundingRect().size();
                        break;
                    }
                }
            }
            m_newSize= finalRect;
        }
        for(auto item : list)
        {
            auto vItem= dynamic_cast<VisualItem*>(item);
            if(vItem != nullptr)
            {
                if(!m_targetItem.contains(vItem) && vItem->isVisible() && vItem->getType() != VisualItem::GRID
                    && vItem->getType() != VisualItem::SIGHT)
                {
                    auto size= vItem->boundingRect().size();
                    m_targetItem.append(vItem);
                    m_originalSize.append(size);
                    if(method == RGraphicsView::Bigger)
                    {
                        finalRect= std::max(finalRect, size, [](const QSizeF& a, const QSizeF& b) {
                            return a.width() * a.height() < b.width() * b.height();
                        });
                    }
                    else if(method == RGraphicsView::Smaller)
                    {
                        if(!finalRect.isValid())
                        {
                            finalRect= size;
                        }
                        finalRect= std::min(finalRect, size, [](const QSizeF& a, const QSizeF& b) {
                            return a.width() * a.height() < b.width() * b.height();
                        });
                    }
                    else if(method == RGraphicsView::Average)
                    {
                        finalRect= finalRect.isValid() ? (size + finalRect) / 2 : size;
                    }
                }
            }
            m_newSize= finalRect;
        }
        setText(QObject::tr("Change size of %1 item(s)").arg(m_targetItem.size()));
    }
}

void ChangeSizeVmapItemCommand::undo()
{
    qInfo() << QStringLiteral("undo command ChangeSizeVmapItemCommand: %1 ").arg(text());
    Q_ASSERT(m_targetItem.size() == m_originalSize.size());
    int i= 0;
    for(auto& item : m_targetItem)
    {
        if(item == nullptr)
            continue;

        auto size= m_originalSize[i];
        item->setSize(size);
        item->sendRectGeometryMsg();
        ++i;
    }
}

void ChangeSizeVmapItemCommand::redo()
{
    qInfo() << QStringLiteral("redo command ChangeSizeVmapItemCommand: %1 ").arg(text());
    if(!m_newSize.isValid())
        return;

    for(auto& item : m_targetItem)
    {
        item->setSize(m_newSize);
        item->sendRectGeometryMsg();
    }
}
