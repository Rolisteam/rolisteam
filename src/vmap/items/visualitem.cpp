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
#include "visualitem.h"

#include <QGraphicsSceneHoverEvent>
#include <math.h>
#include <QCursor>
#include <QDebug>
#include <QUuid>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

VisualItem::VisualItem()
    : QGraphicsObject()
{
    m_id = QUuid::createUuid().toString();
    init();


}

VisualItem::VisualItem(QColor& penColor,QGraphicsItem * parent )
    : QGraphicsObject(parent),m_color(penColor)
{
   init();
}
void VisualItem::init()
{
   setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
   connect(this,SIGNAL(xChanged()),this,SLOT(sendPositionMsg()));
   connect(this,SIGNAL(yChanged()),this,SLOT(sendPositionMsg()));
   connect(this,SIGNAL(zChanged()),this,SLOT(sendPositionMsg()));
}

void VisualItem::setPenColor(QColor& penColor)
{
    m_color = penColor;
}
void VisualItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mouseMoveEvent(event);
}
void VisualItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
void VisualItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
void VisualItem::setId(QString id)
{
    m_id = id;
}

QString VisualItem::getId()
{
    return m_id;
}

void VisualItem::sendPositionMsg()
{
    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::MoveItem);
    msg.string8(m_mapId);
    msg.string16(m_id);
    msg.real(pos().x());
    msg.real(pos().y());
    msg.real(zValue());
    msg.sendAll();
}
void VisualItem::readPositionMsg(NetworkMessageReader* msg)
{
    qreal x = msg->real();
    qreal y = msg->real();
    qreal z = msg->real();

    setPos(x,y);
    setZValue(z);
}

void VisualItem::setMapId(QString id)
{
    m_mapId = id;
}

QString VisualItem::getMapId()
{
    return m_mapId;
}








//friend functions
QDataStream& operator<<(QDataStream& os,const VisualItem& c)
{
    
    c.writeData(os);
    
    return os;
}
QDataStream& operator>>(QDataStream& is,VisualItem& c)
{
    c.readData(is);
    return is;
}

