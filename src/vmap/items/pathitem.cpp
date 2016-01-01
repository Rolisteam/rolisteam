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
#include <QMenu>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"


PathItem::PathItem()
    : VisualItem()
{
    m_closed=false;
    createActions();
}

PathItem::PathItem(QPointF& start,QColor& penColor,int penSize,bool penMode,QGraphicsItem * parent)
    : VisualItem(penColor,parent),m_penMode(penMode)
{
    m_closed=false;
//    m_path.moveTo(start);
	m_start = start;
    m_pen.setColor(penColor);
    m_pen.setWidth(penSize);
    m_pen.setCapStyle(Qt::RoundCap);
    m_pen.setJoinStyle(Qt::RoundJoin);
    createActions();
    
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
    setChildrenVisible(hasFocusOrChild());

	QPainterPath path;
    if(!m_penMode)
    {
        path.moveTo(m_start);
        foreach(QPointF p,m_pointVector)
        {
            path.lineTo(p);
        }
        if(m_closed)
        {
            path.lineTo(m_start);
        }
    }
    else
    {
        path.moveTo(m_start);
        for(int i = 0; i< m_pointVector.size();++i)
        {
            if(i==0)
            {
                path.quadTo(m_start,m_pointVector[i]);
            }
            else if(i==1)
            {
                path.cubicTo(m_start,m_pointVector[0],m_pointVector[i]);
            }
            else
            {
                path.cubicTo(m_pointVector[i-2],m_pointVector[i-1],m_pointVector[i]);
            }
        }
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
    emit itemGeometryChanged(this);
}

void PathItem::writeData(QDataStream& out) const
{
    out << m_start;
    out << m_pointVector;
    out << m_path;
    out << m_pen;
}

void PathItem::readData(QDataStream& in)
{
    in >> m_start;
    in >> m_pointVector;
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
    msg->uint8(m_closed);
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
    m_closed = (bool)msg->uint8();
    m_closeAct->setChecked(m_closed);

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
void PathItem::createActions()
{
    m_closeAct = new QAction(tr("Close Path"),this);
    m_closeAct->setCheckable(true);
    connect(m_closeAct,SIGNAL(triggered()),this,SLOT(closePath()));
}

void PathItem::addActionContextMenu(QMenu* menu)
{
    menu->addAction(m_closeAct);
}
void  PathItem::closePath()
{
    m_closed = m_closeAct->isChecked();
    emit itemGeometryChanged(this);
    update();
}

void PathItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    if(pointId==-1)
    {
        m_start = pos;
    }
    else
    {
        m_pointVector[(int)pointId]=pos;
    }
}
void PathItem::initChildPointItem()
{
	if(NULL == m_child)
	{
		m_child = new QVector<ChildPointItem*>();
        ChildPointItem* tmp = new ChildPointItem(-1,this);
        tmp->setMotion(ChildPointItem::ALL);
        m_child->append(tmp);
        tmp->setPos(m_start);
        tmp->setPlacement(ChildPointItem::Center);
	}


    for(int i = m_child->size()-1; i< m_pointVector.size() ; ++i)
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

    path->setPath(m_pointVector);
    path->setStartPoint(m_start);
    path->setPen(m_pen);
    path->setClosed(m_closed);
    path->setPos(pos());

///@todo implement copy

	return path;
}
void PathItem::setPen(QPen pen)
{
    m_pen = pen;
    update();
}
void PathItem::setStartPoint(QPointF start)
{
    m_start = start;
}

void PathItem::setPath(QVector<QPointF> points)
{
    m_pointVector = points;
    update();
}
void PathItem::setClosed(bool b)
{
    m_closed = b;
    update();
}
