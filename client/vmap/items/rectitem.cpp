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
#include <QStyle>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QStylePainter>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

RectItem::RectItem()
    : VisualItem(),m_penWidth(1),m_initialized(false),m_filled(false)
{

}

RectItem::RectItem(QPointF& topleft,QPointF& buttomright,bool filled,quint16 penSize,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent),m_penWidth(penSize),m_initialized(false),m_filled(filled)
{
    m_rect.setX(0);
    m_rect.setY(0);
    setPos(topleft);
    m_rect.setBottomRight(buttomright-topleft);
    m_filled= filled;
}

QRectF RectItem::boundingRect() const
{
    return m_rect;
}
QPainterPath RectItem::shape() const
{
    if(!m_filled)
    {
        QPainterPath path;
        qreal halfPenSize = m_penWidth/2.0;
        qreal off = 0.5 * halfPenSize;

        path.moveTo(m_rect.topLeft().x()-off,m_rect.topLeft().y()-off);
        path.lineTo(m_rect.topRight().x()+off,m_rect.topRight().y()-off);
        path.lineTo(m_rect.bottomRight().x()+off,m_rect.bottomRight().y()+off);
        path.lineTo(m_rect.bottomLeft().x()-off,m_rect.bottomLeft().y()+off);
        path.lineTo(m_rect.topLeft().x()-off,m_rect.topLeft().y()-off);

        path.lineTo(m_rect.topLeft().x()+off,m_rect.topLeft().y()+off);
        path.lineTo(m_rect.topRight().x()-off,m_rect.topRight().y()+off);
        path.lineTo(m_rect.bottomRight().x()-off,m_rect.bottomRight().y()-off);
        path.lineTo(m_rect.bottomLeft().x()+off,m_rect.bottomLeft().y()-off);
        path.lineTo(m_rect.topLeft().x()+off,m_rect.topLeft().y()+off);
        return path;

    }
    else
    {
        return VisualItem::shape();
    }
}
void RectItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    if(!m_filled)
    {
        QPen pen = painter->pen();
        pen.setColor(m_color);
        pen.setWidth(m_penWidth);
        painter->setPen(pen);
        painter->drawRect(m_rect);
    }
    else
    {
        painter->setBrush(QBrush(m_color));
        painter->fillRect(m_rect, m_color);
    }
    setChildrenVisible(hasFocusOrChild());
    painter->restore();
}
void RectItem::setNewEnd(QPointF& p)
{
    QPointF posItem = pos();
    m_rect.setBottomRight(p-posItem);
}

VisualItem::ItemType RectItem::getType() const
{
    return VisualItem::RECT;
}
void RectItem::writeData(QDataStream& out) const
{
    out << m_rect;
    out << m_filled;
    out << m_color;
    out << m_id;
    out << m_penWidth;
    out << opacity();
    out << m_initialized;
    out << scale();
    out << rotation();
    out << pos();
    //out << zValue();
    out << (int)m_layer;
}

void RectItem::readData(QDataStream& in)
{
    in >> m_rect;
    in >> m_filled;
    in >> m_color;
    in >> m_id;
    in >> m_penWidth;
    qreal opa=0;
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
    m_layer = (VisualItem::Layer)i;
}
void RectItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);

    //rect
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());

    msg->uint8((int)m_layer);
    msg->real(zValue());
    msg->real(opacity());

    //pos
    msg->real(pos().x());
    msg->real(pos().y());

    //others
    msg->int8(m_filled);
    msg->rgb(m_color);
	msg->uint16(m_penWidth);


    msg->real(scale());
    msg->real(rotation());

}
void RectItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    //rect
    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());

    m_layer = (VisualItem::Layer)msg->uint8();
    setZValue(msg->real());
    setOpacity(msg->real());


    //pos
    qreal x  = msg->real();
    qreal y = msg->real();
    setPos(x,y);


    m_filled = msg->int8();
    m_color = msg->rgb();
	m_penWidth = msg->uint16();
    setScale(msg->real());
    setRotation(msg->real());
}
void RectItem::setGeometryPoint(qreal pointId, QPointF &position)
{

    QPointF posItem = pos();
    switch ((int)pointId)
    {
    case 0:
        m_rect.setTopLeft(position);
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 1:
        m_rect.setTopRight(position);
         m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 2:
        m_rect.setBottomRight(position-posItem);
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 3:
        m_rect.setBottomLeft(position);
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        break;
    default:
        break;
    }
    setTransformOriginPoint(m_rect.center());
}
void RectItem::initChildPointItem()
{
    m_rect = m_rect.normalized();
    setTransformOriginPoint(m_rect.center());
    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 4 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this);
        tmp->setMotion(ChildPointItem::MOUSE);
        m_child->append(tmp);
    }
   updateChildPosition();
}
void RectItem::updateChildPosition()
{
    m_child->value(0)->setPos(m_rect.topLeft());
    m_child->value(0)->setPlacement(ChildPointItem::TopLeft);
    m_child->value(1)->setPos(m_rect.topRight());
    m_child->value(1)->setPlacement(ChildPointItem::TopRight);
    m_child->value(2)->setPos(m_rect.bottomRight());
    m_child->value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_child->value(3)->setPos(m_rect.bottomLeft());
    m_child->value(3)->setPlacement(ChildPointItem::ButtomLeft);

    //setTransformOriginPoint(m_rect.center());

    update();
}
VisualItem* RectItem::getItemCopy()
{
	QPointF topLeft = m_rect.topLeft();
	QPointF bottomRight = m_rect.bottomRight();
	RectItem* rectItem = new RectItem(topLeft,bottomRight,m_filled,m_penWidth,m_color);
    rectItem->setPos(pos());
	return rectItem;
}
