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
#include <QStyleOptionGraphicsItem>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"


#include <math.h>
#define PI 3.14159265


LineItem::LineItem()
    : VisualItem()
{
    
}

LineItem::LineItem(const QPointF& p,const QColor& penColor,int penSize,QGraphicsItem * parent)
    :  VisualItem(penColor,penSize,parent)
{
    m_startPoint = p;
	m_endPoint = p;
    m_rect.setTopLeft(p);
}

void LineItem::setNewEnd(QPointF& nend)
{
    m_endPoint = nend;
    m_rect.setBottomRight(nend);
}
QRectF LineItem::boundingRect() const
{
    return  m_rect.normalized();
}
QPainterPath LineItem::shape() const
{
    QLineF line(m_startPoint,m_endPoint);
    line.setLength(line.length()+m_penWidth/2.0);
    QLineF line2(line.p2(),line.p1());
    line2.setLength(line2.length()+m_penWidth/2.0);
    line.setPoints(line2.p2(),line2.p1());

    QLineF normal = line.normalVector();
    normal.setLength(m_penWidth/2.0);
    auto start = normal.p2();
    auto end = normal.pointAt(-1);
    QLineF normal2 = line2.normalVector();
    normal2.setLength(m_penWidth/2.0);
    auto p2 = normal2.p2();
    auto p3 = normal2.pointAt(-1);

    QPainterPath path;
    path.moveTo(start);
    path.lineTo(p3);
    path.lineTo(p2);
    path.lineTo(end);
    path.lineTo(start);
	return path;
}
void LineItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)
    painter->save();
    auto pen = painter->pen();
    pen.setColor(m_color);
    pen.setWidth(m_penWidth);
    painter->setPen(pen);
    painter->drawLine(m_startPoint,m_endPoint);
    setChildrenVisible(hasFocusOrChild());
    painter->restore();

    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        auto shapePath = shape();
        QPen pen = painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawPath(shapePath);
        painter->restore();
    }
    
}
void LineItem::writeData(QDataStream& out) const
{
    out << m_rect;
    out << m_startPoint;
    out << m_endPoint;
    out << opacity();
    out << m_penWidth;
    out << m_color;
    out << (int)m_layer;
}

void LineItem::readData(QDataStream& in)
{
    in >> m_rect;
    in >> m_startPoint;
    in >> m_endPoint;
    qreal opa=0;
    in >> opa;
    setOpacity(opa);
    in >> m_penWidth;
    in >> m_color;
    int i;
    in >> i;
    m_layer = (VisualItem::Layer)i;
}
VisualItem::ItemType LineItem::getType() const
{
    return VisualItem::LINE;
}
void LineItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    msg->uint8((int)m_layer);
    msg->real(zValue());
    msg->real(opacity());

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
    msg->int16(m_penWidth);
    msg->rgb(m_color.rgb());


    msg->real(pos().x());
    msg->real(pos().y());
}
void LineItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
    m_layer = (VisualItem::Layer)msg->uint8();
    setZValue(msg->real());
    setOpacity(msg->real());
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
    m_penWidth = msg->int16();
    m_color = msg->rgb();

    qreal posx = msg->real();
    qreal posy = msg->real();

    setPos(posx,posy);

}
void LineItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    if(pointId == 0)
    {
        m_resizing = true;
        m_startPoint = pos;
        m_rect.setTopLeft(m_startPoint);
    }
    else if(pointId == 1)
    {

        m_resizing = true;
        m_endPoint = pos;
        m_rect.setBottomRight(m_endPoint);
    }
}
void LineItem::setRectSize(qreal x,qreal y,qreal w,qreal h)
{
    m_rect.setX(x);
    m_rect.setY(y);
    m_rect.setWidth(w);
    m_rect.setHeight(h);

    m_startPoint = m_rect.topLeft();
    m_endPoint = m_rect.bottomRight();
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
    LineItem* line = new LineItem(m_startPoint,m_color,m_penWidth);
	line->setNewEnd(m_endPoint);
    line->setOpacity(opacity());
    line->setScale(scale());
    line->setRotation(rotation());
    line->setZValue(zValue());
    line->setLayer(getLayer());

	return line;
}
