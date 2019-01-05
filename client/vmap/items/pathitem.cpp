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
#include <QDebug>
#include <QMenu>
#include <QStyleOptionGraphicsItem>

#include "map/map.h"


#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

QPainterPath vectorToPath(const QVector<QPointF>& points, bool closeUp = false)
{
    QPainterPath path;
    bool first = true;
    QPointF start;
    for(QPointF p : points)
    {
        if(first)
        {
            first = false;
            start = p;
            path.moveTo(p);
        }
        else
            path.lineTo(p);
    }

    if(closeUp)
        path.lineTo(start);
    return path;
}


PathItem::PathItem()
    : VisualItem(),m_penMode(false),m_filled(false)
{
    m_closed=false;
    createActions();
}

PathItem::PathItem(const QPointF &start, const QColor &penColor, int penSize, bool penMode, QGraphicsItem * parent)
    : VisualItem(penColor,penSize,parent),m_penMode(penMode),m_filled(false)
{
    m_closed=false;
    setPos(start);
    m_start = start;
    m_end = m_start;
    createActions();
    
}
QRectF PathItem::boundingRect() const
{
    QPainterPath path = vectorToPath(m_pointVectorBary);
	QRectF rect = path.boundingRect();
    rect.adjust(-m_penWidth/2,-m_penWidth/2,m_penWidth/2,m_penWidth/2);
    return rect;
}

QPainterPath PathItem::shape () const
{
    return vectorToPath(m_pointVectorBary);
}
void PathItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    setChildrenVisible(hasFocusOrChild());

    QPainterPath path = vectorToPath(m_pointVectorBary,m_closed);
    if(!m_penMode)
    {
        if(!m_end.isNull())
        {
            path.lineTo(m_end-pos());
        }
    }
    painter->save();
    auto pen = painter->pen();
    pen.setColor(m_color);
    pen.setWidth(m_penWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    if(m_filled)
    {
        path.setFillRule(Qt::OddEvenFill);
        painter->setBrush(pen.brush());
    }
    painter->setPen(pen);
	painter->drawPath(path);
    painter->restore();


    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        QPen pen = painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        QTransform scale = QTransform().scale(1.01,1.01);
        painter->drawPath(scale.map(path));
        painter->restore();
    }
}
void PathItem::setNewEnd(QPointF& p)
{
    m_end = p;
    if(m_penMode)
    {
        m_pointVector.append(p);
        initChildPointItem();
        update();
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
            initChildPointItem();
            update();
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
    out << m_penWidth;
    out << m_color;
    out << m_closed;
    out << m_filled;
    out << scale();
    out << rotation();
    out << m_penMode;
    out << static_cast<int>(m_layer);
}

void PathItem::readData(QDataStream& in)
{
    in >> m_start;
    in >> m_pointVector;
    in >> m_path;
    qreal opa=0;
    in >> opa;
    setOpacity(opa);
    in >> m_penWidth;
    in >> m_color;
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
    m_layer = static_cast<VisualItem::Layer>(i);

    //m_end = m_start;
    initRealPoints();
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
    msg->uint8(static_cast<quint8>(m_layer));
    msg->real(zValue());
    msg->real(opacity());

    msg->real(pos().x());
    msg->real(pos().y());

    //pen
    msg->uint16(m_penWidth);
    msg->rgb(m_color.rgb());

    msg->real(m_start.x());
    msg->real(m_start.y());


    //path
    msg->uint32(m_pointVector.size());
    for(QPointF& pos:m_pointVector)
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
    m_closed = static_cast<bool>(msg->uint8());
    m_closeAct->setChecked(m_closed);
    m_filled = static_cast<bool>(msg->uint8());
    m_fillAct->setChecked(m_filled);
    m_penMode = static_cast<bool>(msg->uint8());
    m_layer = static_cast<VisualItem::Layer>(msg->uint8());
    setZValue(msg->real());
    setOpacity(msg->real());

    qreal posx = msg->real();
    qreal posy = msg->real();


    //pen
    m_penWidth =msg->int16();
    m_color = msg->rgb();

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
    initRealPoints();
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
    auto idx = static_cast<int>(pointId);

    if(m_pointVectorBary.isEmpty() || m_pointVectorBary.size() <= idx)
        return;

    m_resizing = true;
    m_changedPointId = pointId;
    m_changedPointPos = pos;
    m_pointVectorBary[idx]=pos;
}

void PathItem::initRealPoints()
{
    QPointF median = m_start;
    for(auto& point : m_pointVector)
    {
        median = (median+point)/2;
    }
    m_pointVectorBary.clear();
    m_pointVectorBary.append(m_start-median);
    for(auto& point : m_pointVector)
    {
        m_pointVectorBary.append(point-median);
    }
    setPos(median);
}
void PathItem::initChildPointItem()
{
    initRealPoints();

    if(nullptr == m_child)
	{
		m_child = new QVector<ChildPointItem*>();
    }
    else
    {
       qDeleteAll(m_child->begin(),m_child->end());
       m_child->clear();
    }
    if(!m_penMode)
    {
        int i = 0;
        for(auto& p : m_pointVectorBary)
        {
            ChildPointItem* tmp = new ChildPointItem(i,this);
            tmp->setMotion(ChildPointItem::ALL);
            m_child->append(tmp);
            tmp->setPos(p);
            tmp->setPlacement(ChildPointItem::Center);
            ++i;
        }
    }

}
VisualItem* PathItem::getItemCopy()
{
	PathItem* path = new PathItem();
    path->setPath(m_pointVector);
    path->setStartPoint(m_start);
    path->setPenColor(m_color);
    path->setPenWidth(m_penWidth);
    path->setClosed(m_closed);
    path->setFilled(m_filled);
    path->setPos(pos());
	return path;
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
    m_pointVectorBary[static_cast<int>(pointid)]=QPointF(x,y);
    update();
}


void PathItem::setPath(QVector<QPointF> points)
{
    m_pointVectorBary = points;
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
        msg.sendToServer();
    }
}
