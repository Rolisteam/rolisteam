/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
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
#include "ellipsitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <cmath>
#include <math.h>

#include "controller/item_controllers/ellipsecontroller.h"

EllipsItem::EllipsItem(vmap::EllipseController* ctrl) : VisualItem(ctrl), m_ellipseCtrl(ctrl)
{
    connect(m_ellipseCtrl, &vmap::EllipseController::rxChanged, this,
            [this]()
            {
                setTransformOriginPoint(boundingRect().center());
                updateChildPosition();
            });
    connect(m_ellipseCtrl, &vmap::EllipseController::ryChanged, this,
            [this]()
            {
                setTransformOriginPoint(boundingRect().center());
                updateChildPosition();
            });
    connect(m_ellipseCtrl, &vmap::EllipseController::filledChanged, this, [this]() { update(); });

    initChildPointItem();
}

QRectF EllipsItem::boundingRect() const
{
    return m_ellipseCtrl ? m_ellipseCtrl->rect() : QRectF{};
}

QPainterPath EllipsItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());

    if(!m_ellipseCtrl || m_ellipseCtrl->filled())
        return path;

    QPainterPath subpath;

    QRectF rect= boundingRect();
    auto penW= m_ellipseCtrl->penWidth();
    rect.adjust(penW, penW, -penW, -penW);
    subpath.addEllipse(rect);
    path-= subpath;

    return path;
}
void EllipsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    if(!m_ellipseCtrl->filled())
    {
        QPen pen= painter->pen();
        pen.setColor(m_ellipseCtrl->color());
        pen.setWidth(m_ellipseCtrl->penWidth());
        painter->setPen(pen);
    }
    else
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(m_ellipseCtrl->color(), Qt::SolidPattern));
    }

    setChildrenVisible(hasFocusOrChild());

    painter->drawEllipse(QPointF(0, 0), m_ellipseCtrl->rx(), m_ellipseCtrl->ry());

    painter->restore();

    if(canBeMoved() && (option->state & QStyle::State_MouseOver || isSelected()))
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(isSelected() ? m_selectedColor : m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawEllipse(QPointF(0, 0), m_ellipseCtrl->rx(), m_ellipseCtrl->ry());
        painter->restore();
    }
}
void EllipsItem::setNewEnd(const QPointF& p)
{
    m_ellipseCtrl->setCorner(p, 2);
}

void EllipsItem::updateChildPosition()
{
    auto rect= boundingRect();
    m_children.value(0)->setPos(QPointF(rect.right(), rect.center().y()));
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    m_children.value(1)->setPos(QPointF(rect.center().x(), rect.bottom()));
    m_children.value(1)->setPlacement(ChildPointItem::ButtomCenter);
    update();
}

void EllipsItem::initChildPointItem()
{
    if(!m_ellipseCtrl)
        return;
    setTransformOriginPoint(boundingRect().center());

    for(int i= 0; i < 2; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_ctrl, i, this);
        m_children.append(tmp);
    }
    m_children.value(0)->setPos(m_ellipseCtrl->rx(), 0);
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    m_children.value(0)->setMotion(ChildPointItem::X_AXIS | ChildPointItem::ROTATION);
    m_children.value(1)->setPos(0, m_ellipseCtrl->ry());
    m_children.value(1)->setMotion(ChildPointItem::Y_AXIS | ChildPointItem::ROTATION);
    m_children.value(1)->setPlacement(ChildPointItem::ButtomCenter);
}
