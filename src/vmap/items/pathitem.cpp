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
//    m_path.moveTo(start);
	m_start = start;
    m_pen.setColor(penColor);
    m_pen.setWidth(penSize);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);
    
}
QRectF PathItem::boundingRect() const
{
	QPainterPath path;
	path.moveTo(m_start);
	foreach(QPointF p,m_pointVector)
	{
		path.lineTo(p);
	}


	QRectF rect = path.boundingRect();
    rect.adjust(-m_pen.width()/2,-m_pen.width()/2,m_pen.width()/2,m_pen.width()/2);
    return rect;
}
QPainterPath PathItem::shape () const
{
	QPainterPath path;
	path.moveTo(m_start);
	foreach(QPointF p,m_pointVector)
	{
		path.lineTo(p);
	}
	return path;
}
void PathItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	if(hasFocusOrChild())
	{
		foreach(ChildPointItem* item, *m_child)
		{
			item->setVisible(true);
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

	QPainterPath path;
	path.moveTo(m_start);
	foreach(QPointF p,m_pointVector)
	{
		path.lineTo(p);
	}
    painter->save();
    painter->setPen(m_pen);
	painter->drawPath(path);
    painter->restore();
}
void PathItem::setNewEnd(QPointF& p)
{
	m_pointVector.append(p);
	update();
	initChildPointItem();
    itemGeometryChanged(this);
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
    msg->real(scale());
    msg->real(rotation());
    //pen
    msg->uint16(m_pen.width());
    msg->rgb(m_pen.color());

    msg->real(m_start.x());
    msg->real(m_start.y());

    //path
    msg->uint32(m_pointVector.size());
    foreach(QPointF pos,m_pointVector)
    {
        msg->real(pos.x());
        msg->real(pos.y());
    }
}
void PathItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());

    //pen
    m_pen.setWidth(msg->int16());
    m_pen.setColor(msg->rgb());
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);

    m_start.setX(msg->real());
    m_start.setY(msg->real());

    //path
    quint16 size = msg->uint32();
    m_pointVector.clear();
    for(int i = 0; i < size ; ++i)
    {
        qreal x = msg->real();
        qreal y = msg->real();
        m_pointVector.append(QPointF(x,y));
    }
}
void PathItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
	m_pointVector[(int)pointId]=pos;
}
void PathItem::initChildPointItem()
{
	if(NULL == m_child)
	{
		m_child = new QVector<ChildPointItem*>();
	}


	for(int i = m_child->size(); i< m_pointVector.size() ; ++i)
	{
		ChildPointItem* tmp = new ChildPointItem(i,this);
		tmp->setMotion(ChildPointItem::ALL);
		m_child->append(tmp);
		tmp->setPos(m_pointVector.at(i));
		tmp->setPlacement(ChildPointItem::Center);
	}

}
VisualItem* PathItem::getItemCopy()
{
	PathItem* path = new PathItem();

///@todo implement copy

	return path;
}
void PathItem::setPen()
{

}

void PathItem::setPath()
{

}
