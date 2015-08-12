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
    : VisualItem()
{

}

RectItem::RectItem(QPointF& topleft,QPointF& buttomright,bool filled,quint16 penSize,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent),m_penWidth(penSize)
{
    m_rect.setBottomRight(buttomright);
    m_rect.setTopLeft(topleft);
    m_filled= filled;
}

QRectF RectItem::boundingRect() const
{
    return m_rect;
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
    if(hasFocusOrChild())
    {
        if(NULL!=m_child)
        {
            foreach(ChildPointItem* item, *m_child)
            {
                item->setVisible(true);
            }
        }
    }
    else
    {
        if(NULL!=m_child)
        {
            foreach(ChildPointItem* item, *m_child)
            {
                item->setVisible(false);
            }
        }
    }
    painter->restore();
    
}
void RectItem::setNewEnd(QPointF& p)
{
    m_rect.setBottomRight(p);
}

VisualItem::ItemType RectItem::getType()
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
}

void RectItem::readData(QDataStream& in)
{
    in >> m_rect;
    in >> m_filled;
    in >> m_color;
    in >> m_id;
    in >> m_penWidth;
}
void RectItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);

    //rect
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());

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
    m_filled = msg->int8();
    m_color = msg->rgb();
	m_penWidth = msg->uint16();
    setTransformOriginPoint(m_rect.center());
    setScale(msg->real());
    setRotation(msg->real());
}
void RectItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    switch ((int)pointId)
    {
    case 0:
        m_rect.setTopLeft(pos);
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 1:
        m_rect.setTopRight(pos);
         m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 2:
        m_rect.setBottomRight(pos);
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 3:
        m_rect.setBottomLeft(pos);
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        break;
    default:
        break;
    }

    setTransformOriginPoint(m_rect.center());
    //updateChildPosition();
}
void RectItem::initChildPointItem()
{
    setPos(m_rect.center());
    m_rect.setCoords(-m_rect.width()/2,-m_rect.height()/2,m_rect.width()/2,m_rect.height()/2);
    setTransformOriginPoint(m_rect.center());

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

    setTransformOriginPoint(m_rect.center());

    update();
}
VisualItem* RectItem::getItemCopy()
{
	QPointF topLeft = m_rect.topLeft();
	QPointF bottomRight = m_rect.bottomRight();
	RectItem* rectItem = new RectItem(topLeft,bottomRight,m_filled,m_penWidth,m_color);
	return rectItem;
}
