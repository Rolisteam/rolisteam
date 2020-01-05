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

#include "vmap/controller/visualitemcontroller.h"
#include <QDebug>

ChangeSizeVmapItemCommand::ChangeSizeVmapItemCommand(const QList<vmap::VisualItemController*>& list,
                                                     VectorialMapController::Method method, const QPointF& mousePos,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent)
{
    m_data.reserve(static_cast<std::size_t>(list.size()));
    QRectF ref;
    if(method == VectorialMapController::Bigger)
    {
        auto it= std::max_element(std::begin(list), std::end(list),
                                  [](vmap::VisualItemController* a, vmap::VisualItemController* b) {
                                      auto areaA= a->rect().height() * a->rect().width();
                                      auto areaB= b->rect().height() * b->rect().width();
                                      return areaA < areaB;
                                  });
        ref= (*it)->rect();
    }
    else if(method == VectorialMapController::Smaller)
    {
        auto it= std::min_element(std::begin(list), std::end(list),
                                  [](vmap::VisualItemController* a, vmap::VisualItemController* b) {
                                      auto areaA= a->rect().height() * a->rect().width();
                                      auto areaB= b->rect().height() * b->rect().width();
                                      return areaA < areaB;
                                  });
        ref= (*it)->rect();
    }
    else if(method == VectorialMapController::Average)
    {
        qreal averageHeight= 0.0;
        qreal averageWidth= 0.0;
        std::for_each(std::begin(list), std::end(list), [&averageHeight, &averageWidth](vmap::VisualItemController* a) {
            averageHeight+= a->rect().height();
            averageWidth+= a->rect().width();
        });
        averageWidth/= list.size();
        averageHeight/= list.size();
        ref= QRectF(0.0, 0.0, averageWidth, averageHeight);
    }
    else if(method == VectorialMapController::UnderMouse)
    {
        auto it= std::find_if(std::begin(list), std::end(list), [mousePos](vmap::VisualItemController* a) {
            auto rect= a->rect();
            rect.translate(a->pos());
            return rect.contains(mousePos);
        });

        if(it == std::end(list))
            qCritical() << "ChangeSizeVmapItemCommand - no item under the mouse";
        else
            ref= (*it)->rect();
    }

    if(!ref.isEmpty())
    {
        std::transform(std::begin(list), std::end(list), std::back_inserter(m_data),
                       [ref](vmap::VisualItemController* a) -> ChangeSizeData {
                           ChangeSizeData data;
                           data.m_ctrl= a;
                           auto rect= a->rect();
                           data.m_move= QPointF(ref.width() - rect.width(), ref.height() - rect.height());
                           data.m_resetMove= QPointF(-data.m_move.x(), -data.m_move.y());
                           return data;
                       });
    }

    setText(QObject::tr("Change size of %1 item(s)").arg(list.size()));
}

void ChangeSizeVmapItemCommand::undo()
{
    qInfo() << QStringLiteral("undo command ChangeSizeVmapItemCommand: %1 ").arg(text());
    std::for_each(std::begin(m_data), std::end(m_data),
                  [](const ChangeSizeData& data) { data.m_ctrl->setCorner(data.m_resetMove, 2); });
}

void ChangeSizeVmapItemCommand::redo()
{
    qInfo() << QStringLiteral("redo command ChangeSizeVmapItemCommand: %1 ").arg(text());
    std::for_each(std::begin(m_data), std::end(m_data), [](const ChangeSizeData& data) {
        qDebug() << data.m_move;
        data.m_ctrl->setCorner(data.m_move, 2);
    });
}
