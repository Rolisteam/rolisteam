/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include "griditem.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <math.h>

#include "controller/item_controllers/visualitemcontroller.h"
#include "media/mediatype.h"

// constexpr float PI{3.14159265};

/////////////////////////////////
/// Code SightItem
/////////////////////////////////

GridItem::GridItem(vmap::GridController* ctrl) : VisualItem(ctrl), m_gridCtrl(ctrl)
{
    setZValue(std::numeric_limits<qreal>::max());
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    if(m_gridCtrl)
        m_ctrl->setLayer(Core::Layer::GRIDLAYER);
    setFlags(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsSelectable, false);

    connect(m_gridCtrl, &vmap::GridController::gridPatternChanged, this, [this]() { update(); });
    connect(m_gridCtrl, &vmap::GridController::rectChanged, this, [this]() { update(); });

    connect(this, &QGraphicsObject::parentChanged, this,
            [this]()
            {
                connect(scene(), &QGraphicsScene::sceneRectChanged, m_gridCtrl, &vmap::GridController::setRect);
                m_gridCtrl->setRect(scene()->sceneRect());
            });
}

GridItem::~GridItem() {}

void GridItem::setNewEnd(const QPointF &nend)
{
    Q_UNUSED(nend);
}

void GridItem::updateItemFlags()
{
    VisualItem::updateItemFlags();
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}
QRectF GridItem::boundingRect() const
{
    return m_gridCtrl ? m_gridCtrl->rect() : QRectF{};
}

void GridItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(!m_gridCtrl->visible())
        return;

    painter->fillRect(boundingRect(), QBrush(m_gridCtrl->gridPattern()));
}
