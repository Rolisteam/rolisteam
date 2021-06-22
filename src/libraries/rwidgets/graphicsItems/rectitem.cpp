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

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
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
}

QRectF RectItem::boundingRect() const
{
    return m_rectCtrl->rect();
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

    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
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

void RectItem::writeData(QDataStream& out) const
{
    /*out << m_rect;
    out << m_rectCtrl->filled();
    out << m_color;
    out << m_id;
    out << m_penWidth;
    out << opacity();
    out << m_initialized;
    out << scale();
    out << rotation();
    out << pos();
    // out << zValue();
    out << static_cast<int>(m_layer);*/
}

void RectItem::readData(QDataStream& in)
{
    /* in >> m_rect;
     in >> m_filled;
     in >> m_color;
     in >> m_id;
     in >> m_penWidth;
     qreal opa= 0;
     in >> opa;
     setOpacity(opa);
     in >> m_initialized;
     qreal scale;
     in >> scale;
     setScale(scale);

     qreal rotation;
     in >> rotation;
     setRotation(rotation);

     QPointF p;
     in >> p;
     setPos(p);

     int i;
     in >> i;
     m_layer= static_cast<Core::Layer>(i);*/
}
void RectItem::fillMessage(NetworkMessageWriter* msg)
{
    /* msg->string16(m_id);

     // rect
     msg->real(m_rect.x());
     msg->real(m_rect.y());
     msg->real(m_rect.width());
     msg->real(m_rect.height());

     msg->uint8((int)m_layer);
     msg->real(zValue());
     msg->real(opacity());

     // pos
     msg->real(pos().x());
     msg->real(pos().y());

     // others
     msg->int8(m_filled);
     msg->rgb(m_color.rgb());
     msg->uint16(m_penWidth);

     msg->real(scale());
     msg->real(rotation());*/
}
void RectItem::readItem(NetworkMessageReader* msg)
{
    /* m_id= msg->string16();
     // rect
     m_rect.setX(msg->real());
     m_rect.setY(msg->real());
     m_rect.setWidth(msg->real());
     m_rect.setHeight(msg->real());

     m_layer= static_cast<Core::Layer>(msg->uint8());
     setZValue(msg->real());
     setOpacity(msg->real());

     // pos
     qreal x= msg->real();
     qreal y= msg->real();
     setPos(x, y);

     m_filled= msg->int8();
     m_color= msg->rgb();
     m_penWidth= msg->uint16();

     // setTransformOriginPoint(m_rect.center());
     setScale(msg->real());
     setRotation(msg->real());*/
}
void RectItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    /*auto rect= m_rectCtrl->rect();
    switch(static_cast<int>(pointId))
    {
    case 0:
        rect.setTopLeft(pos);
        m_children.value(1)->setPos(rect.topRight());
        m_children.value(2)->setPos(rect.bottomRight());
        m_children.value(3)->setPos(rect.bottomLeft());
        break;
    case 1:
        rect.setTopRight(pos);
        m_children.value(0)->setPos(rect.topLeft());
        m_children.value(2)->setPos(rect.bottomRight());
        m_children.value(3)->setPos(rect.bottomLeft());
        break;
    case 2:
        rect.setBottomRight(pos);
        m_children.value(0)->setPos(rect.topLeft());
        m_children.value(1)->setPos(rect.topRight());
        m_children.value(3)->setPos(rect.bottomLeft());
        break;
    case 3:
        rect.setBottomLeft(pos);
        m_children.value(0)->setPos(rect.topLeft());
        m_children.value(1)->setPos(rect.topRight());
        m_children.value(2)->setPos(rect.bottomRight());
        break;
    default:
        break;
    }
    setTransformOriginPoint(rect.center());*/
    // updateChildPosition();
}

void RectItem::initChildPointItem() {}

void RectItem::updateChildPosition()
{
    auto rect= m_rectCtrl->rect();
    m_children.value(0)->setPos(rect.topLeft());
    m_children.value(0)->setPlacement(ChildPointItem::TopLeft);
    m_children.value(1)->setPos(rect.topRight());
    m_children.value(1)->setPlacement(ChildPointItem::TopRight);
    m_children.value(2)->setPos(rect.bottomRight());
    m_children.value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_children.value(3)->setPos(rect.bottomLeft());
    m_children.value(3)->setPlacement(ChildPointItem::ButtomLeft);

    update();
}
VisualItem* RectItem::getItemCopy()
{
    /*QPointF topLeft= m_rect.topLeft();
    QPointF bottomRight= m_rect.bottomRight();*/
    /*RectItem* rectItem= new RectItem(topLeft, bottomRight, m_filled, m_penWidth, m_color);
    rectItem->setPos(pos());*/
    return nullptr;
    //  return rectItem;
}

void RectItem::endOfGeometryChange(ChildPointItem::Change change)
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
}
