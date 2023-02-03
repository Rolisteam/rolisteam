/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
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

#include "rectitem.h"
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QStylePainter>
#include <QTimer>

#include "controller/item_controllers/rectcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"

#include <QDebug>

RectItem::RectItem(vmap::RectController* ctrl) : VisualItem(ctrl), m_rectCtrl(ctrl)
{
    connect(m_rectCtrl, &vmap::RectController::rectChanged, this, [this]() { updateChildPosition(); });

    for(int i= 0; i <= vmap::RectController::BottomLeft; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_rectCtrl, i, this);
        tmp->setMotion(ChildPointItem::MOUSE);
        m_children.append(tmp);
    }

    updateChildPosition();

    if(m_ctrl)
    {
        setTransformOriginPoint(m_ctrl->rotationOriginPoint());
        setRotation(m_ctrl->rotation());
    }
}

QRectF RectItem::boundingRect() const
{
    return m_rectCtrl ? m_rectCtrl->rect() : QRectF();
}
QPainterPath RectItem::shape() const
{
    if(m_rectCtrl->filled())
        return VisualItem::shape();

    QPainterPath path;
    qreal halfPenSize= m_rectCtrl->penWidth() / 2.0;
    auto rect= m_rectCtrl->rect();
    qreal off= 0.5 * halfPenSize;

    path.moveTo(rect.topLeft().x() - off, rect.topLeft().y() - off);

    path.lineTo(rect.topRight().x() + off, rect.topRight().y() - off);
    path.lineTo(rect.bottomRight().x() + off, rect.bottomRight().y() + off);
    path.lineTo(rect.bottomLeft().x() - off, rect.bottomLeft().y() + off);
    path.lineTo(rect.topLeft().x() - off, rect.topLeft().y() - off);

    path.lineTo(rect.topLeft().x() + off, rect.topLeft().y() + off);
    path.lineTo(rect.topRight().x() - off, rect.topRight().y() + off);
    path.lineTo(rect.bottomRight().x() - off, rect.bottomRight().y() - off);
    path.lineTo(rect.bottomLeft().x() + off, rect.bottomLeft().y() - off);
    path.lineTo(rect.topLeft().x() + off, rect.topLeft().y() + off);
    return path;
}
void RectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    if(!m_rectCtrl->filled())
    {
        QPen pen= painter->pen();
        pen.setColor(m_rectCtrl->color());
        pen.setWidth(m_rectCtrl->penWidth());
        painter->setPen(pen);
        painter->drawRect(m_rectCtrl->rect());
    }
    else
    {
        painter->setBrush(QBrush(m_rectCtrl->color()));
        painter->fillRect(m_rectCtrl->rect(), m_rectCtrl->color());
    }
    setChildrenVisible(hasFocusOrChild());
    painter->restore();

    auto val= static_cast<bool>(option->state & QStyle::State_MouseOver);

    if(canBeMoved() && (isSelected() || val)) // option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(isSelected() ? m_selectedColor : m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawRect(m_rectCtrl->rect());
        painter->restore();
    }
}
void RectItem::setNewEnd(const QPointF& p)
{
    m_ctrl->setCorner(p, 2);
}

void RectItem::updateChildPosition()
{
    if(!m_rectCtrl)
        return;

    auto rect= m_rectCtrl->rect();
    m_children.value(0)->setPos(rect.topLeft());
    m_children.value(0)->setPlacement(ChildPointItem::TopLeft);
    m_children.value(1)->setPos(rect.topRight());
    m_children.value(1)->setPlacement(ChildPointItem::TopRight);
    m_children.value(2)->setPos(rect.bottomRight());
    m_children.value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_children.value(3)->setPos(rect.bottomLeft());
    m_children.value(3)->setPlacement(ChildPointItem::ButtomLeft);

    if(!m_ctrl->localIsGM())
    {
        setTransformOriginPoint(m_rectCtrl->rect().center());
    }

    update();
}

/*void RectItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change == ChildPointItem::Resizing)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(m_rectCtrl->rect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_rectCtrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                                   oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    VisualItem::endOfGeometryChange(change);
}*/
