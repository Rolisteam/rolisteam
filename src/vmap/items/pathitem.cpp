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
#include "pathitem.h"
#include <QPainterPath>
#include <QPainter>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"


PathItem::PathItem()
    : VisualItem()
{
    
}

PathItem::PathItem(QPointF& start,QColor& penColor,int penSize,QGraphicsItem * parent)
    : VisualItem(penColor,parent)
{
    m_path.moveTo(start);
    m_pen.setColor(penColor);
    m_pen.setWidth(penSize);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);
    
}
QRectF PathItem::boundingRect() const
{
    //Adjust the bounding according the pen size.
    QRectF rect = m_path.boundingRect();
    rect.adjust(-m_pen.width()/2,-m_pen.width()/2,m_pen.width()/2,m_pen.width()/2);
    return rect;
}
QPainterPath PathItem::shape () const
{
    
    return m_path;
    
}
void PathItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    painter->setPen(m_pen);
    painter->drawPath(m_path);
    painter->restore();
}
void PathItem::setNewEnd(QPointF& p)
{
    //QRectF tmp= m_rect;
    m_path.lineTo(p);
    //update(tmp);
}
void PathItem::writeData(QDataStream& out) const
{
    out << m_path;
    out << m_pen;
}

void PathItem::readData(QDataStream& in)
{
    in >> m_path;
    in >> m_pen;
}
VisualItem::ItemType PathItem::getType()
{
    return VisualItem::PATH;
}
void PathItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    //pen
    msg->uint16(m_pen.width());
    msg->rgb(m_pen.color());

    //path
    QByteArray data;
    QDataStream in(&data,QIODevice::WriteOnly);
    in << m_path;
    msg->byteArray32(data);
}
void PathItem::readItem(NetworkMessageReader* msg)
{
     m_id = msg->string16();
    m_pen.setWidth(msg->int16());
    m_pen.setColor(msg->rgb());

    //center
    QByteArray data;
    data = msg->byteArray32();

    QDataStream out(&data,QIODevice::ReadOnly);
    out >> m_path;

}
