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
#include "lineitem.h"
#include <QPainterPath>
#include <QPainter>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

LineItem::LineItem()
    : VisualItem()
{
    
}

LineItem::LineItem(QPointF& p,QColor& penColor,int penSize,QGraphicsItem * parent)
    :  VisualItem(penColor,parent)
{
    m_startPoint = p;
	m_endPoint = p;
    m_rect.setTopLeft(p);
    m_pen.setColor(penColor);
    m_pen.setWidth(penSize);
}

void LineItem::setNewEnd(QPointF& nend)
{
    m_endPoint = nend;
    m_rect.setBottomRight(nend);
}
QRectF LineItem::boundingRect() const
{
    return  m_rect;
}
QPainterPath LineItem::shape() const
{
	/// @todo may be useful to set polygon instead of line. To manage the width of the line.
	QPainterPath path;
	path.moveTo(m_startPoint);
	path.lineTo(m_endPoint);
	return path;
}
void LineItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    painter->setPen(m_pen);
    painter->drawLine(m_startPoint,m_endPoint);
    setChildrenVisible(hasFocusOrChild());
    painter->restore();
    
}
void LineItem::writeData(QDataStream& out) const
{
    out << m_rect;
    out << m_startPoint;
    out << m_endPoint;
    out << m_pen;
    out << m_color;
}

void LineItem::readData(QDataStream& in)
{
    in >> m_rect;
    in >> m_startPoint;
    in >> m_endPoint;
    in >> m_pen;
    in >> m_color;
}
VisualItem::ItemType LineItem::getType()
{
    return VisualItem::LINE;
}
void LineItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());

    //rect
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());
    //m_startPoint
    msg->real(m_startPoint.x());
    msg->real(m_startPoint.y());
    //m_endPoint
    msg->real(m_endPoint.x());
    msg->real(m_endPoint.y());
    //pen
    msg->int16(m_pen.width());
    msg->rgb(m_color);
}
void LineItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
    //rect
    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());
    //center
    m_startPoint.setX(msg->real());
    m_startPoint.setY(msg->real());
    //m_endPoint
    m_endPoint.setX(msg->real());
    m_endPoint.setY(msg->real());
    //pen
    m_pen.setWidth(msg->int16());
    m_color = msg->rgb();
    m_pen.setColor(m_color);
}
void LineItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    if(pointId == 0)
    {
        m_startPoint = pos;
        m_rect.setTopLeft(m_startPoint);
    }
    else if(pointId == 1)
    {
        m_endPoint = pos;
        m_rect.setBottomRight(m_endPoint);
    }
}
void LineItem::initChildPointItem()
{
    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 2 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this);
        tmp->setMotion(ChildPointItem::ALL);
        m_child->append(tmp);

    }
    m_child->value(0)->setPos(m_startPoint);
    m_child->value(0)->setPlacement(ChildPointItem::Center);
    m_child->value(1)->setPos(m_endPoint);
    m_child->value(1)->setPlacement(ChildPointItem::Center);
}
VisualItem* LineItem::getItemCopy()
{
	LineItem* line = new LineItem(m_startPoint,m_color,m_pen.width());
	line->setNewEnd(m_endPoint);

///@todo implement copy

	return line;
}
