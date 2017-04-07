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
#include <QDebug>

#include "map/map.h"


#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"


PathItem::PathItem()
    : VisualItem(),m_penMode(false),m_filled(false)
{
    m_closed=false;
    createActions();
}

PathItem::PathItem(QPointF& start,QColor& penColor,int penSize,bool penMode,QGraphicsItem * parent)
    : VisualItem(penColor,parent),m_penMode(penMode),m_filled(false)
{
    m_closed=false;
//    m_path.moveTo(start);
	m_start = start;
    m_end = m_start;
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
    for(QPointF p : m_pointVector)
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
        for(QPointF p : m_pointVector)
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
        for(QPointF p: m_pointVector)
        {
            path.lineTo(p);
        }
    }
    if(!m_penMode)
    {
        if(!m_end.isNull())
        {
            path.lineTo(m_end);
        }
    }
    painter->save();
    if(m_filled)
    {
        path.setFillRule(Qt::OddEvenFill);
        painter->setBrush(m_pen.brush());
    }
    painter->setPen(m_pen);
	painter->drawPath(path);
    painter->restore();
}
void PathItem::setNewEnd(QPointF& p)
{
    m_end = p;
    if(m_penMode)
    {
        m_pointVector.append(p);
        update();
        initChildPointItem();
        emit itemGeometryChanged(this);
    }

}
void PathItem::release()
{
    if(!m_penMode)
    {
        if(m_end != m_start)
        {
            m_pointVector.append(m_end);
            update();
            initChildPointItem();
            emit itemGeometryChanged(this);
        }
        m_end = QPointF();
    }
}

void PathItem::writeData(QDataStream& out) const
{
    out << m_start;
    out << m_pointVector;
    out << m_path;
    out << opacity();
    out << m_pen;
    out << m_closed;
    out << m_filled;
    out << scale();
    out << rotation();
    out << m_penMode;
    out << (VisualItem::Layer)m_layer;


}

void PathItem::readData(QDataStream& in)
{
    in >> m_start;
    in >> m_pointVector;
    in >> m_path;
    qreal opa=0;
    in >> opa;
    setOpacity(opa);
    in >> m_pen;
    in >> m_closed;
    m_closeAct->setChecked(m_closed);
    in >> m_filled;
    m_fillAct->setChecked(m_filled);
    qreal scale;
    in >> scale;
    setScale(scale);

    qreal rotation;
    in >> rotation;
    setRotation(rotation);
    in >> m_penMode;
    int i;
    in >> i;
    m_layer = (VisualItem::Layer)i;

    //m_end = m_start;
}
VisualItem::ItemType PathItem::getType() const
{
    return VisualItem::PATH;
}
void PathItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    msg->uint8(m_closed);
    msg->uint8(m_filled);
    msg->uint8(m_penMode);
    msg->uint8((VisualItem::Layer)m_layer);
    msg->real(zValue());
    msg->real(opacity());

    msg->real(pos().x());
    msg->real(pos().y());

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
    m_filled = (bool)msg->uint8();
    m_fillAct->setChecked(m_filled);
    m_penMode = (bool)msg->uint8();
    m_layer = (VisualItem::Layer)msg->uint8();
    setZValue(msg->real());
    setOpacity(msg->real());

    qreal posx = msg->real();
    qreal posy = msg->real();


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


    setPos(posx,posy);
}
void PathItem::createActions()
{
    m_closeAct = new QAction(tr("Close Path"),this);
    m_closeAct->setCheckable(true);
    connect(m_closeAct,SIGNAL(triggered()),this,SLOT(closePath()));

    m_fillAct = new QAction(tr("Fill Path"),this);
    m_fillAct->setCheckable(true);
    connect(m_fillAct,SIGNAL(triggered()),this,SLOT(fillPath()));
}

void PathItem::addActionContextMenu(QMenu* menu)
{
    menu->addAction(m_closeAct);
    menu->addAction(m_fillAct);
}
void  PathItem::closePath()
{
    m_closed = m_closeAct->isChecked();
    emit itemGeometryChanged(this);
    update();
}
void  PathItem::fillPath()
{
    m_filled = m_fillAct->isChecked();
    emit itemGeometryChanged(this);
    update();
}
void PathItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    if(pointId==-1)
    {
        m_start = pos;
        m_resizing = true;
        m_changedPointId = pointId;
        m_changedPointPos = pos;
    }
    else
    {
        m_resizing = true;
        m_changedPointId = pointId;
        m_changedPointPos = pos;
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


    if(!m_penMode)
    {
        for(int i = m_child->size()-1; i< m_pointVector.size() ; ++i)
        {
            ChildPointItem* tmp = new ChildPointItem(i,this);
            tmp->setMotion(ChildPointItem::ALL);
            m_child->append(tmp);
            tmp->setPos(m_pointVector.at(i));
            tmp->setPlacement(ChildPointItem::Center);
        }
    }

}
VisualItem* PathItem::getItemCopy()
{
	PathItem* path = new PathItem();
    path->setPath(m_pointVector);
    path->setStartPoint(m_start);
    path->setPen(m_pen);
    path->setClosed(m_closed);
    path->setFilled(m_filled);
    path->setPos(pos());
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
void PathItem::readMovePointMsg(NetworkMessageReader* msg)
{
    qreal pointid = msg->real();
    qreal x = msg->real();
    qreal y = msg->real();
    if(-1==pointid)
    {
        m_start = QPointF(x,y);
    }
    else
    {
        m_pointVector[(int)pointid]=QPointF(x,y);
    }
    update();
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
void PathItem::setFilled(bool b)
{
    m_filled = b;
    update();
}

void PathItem::endOfGeometryChange()
{
    if(m_resizing)
    {
        sendPointPosition();
        m_resizing =false;
    }
    else
    {
        VisualItem::endOfGeometryChange();
    }
}
void PathItem::sendPointPosition()
{
    if((getOption(VisualItem::LocalIsGM).toBool()) ||
            (getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL) ||
            ((getOption(VisualItem::PermissionMode).toInt() == Map::PC_MOVE)&&
             (getType() == VisualItem::CHARACTER)&&
             (isLocal())))//getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::MovePoint);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(m_changedPointId);
        msg.real(m_changedPointPos.x());
        msg.real(m_changedPointPos.y());
        msg.sendAll();
    }
}
