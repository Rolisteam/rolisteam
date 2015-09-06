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
#include "ellipsitem.h"

#include <QPainter>
#include <QDebug>

#include "math.h"

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

EllipsItem::EllipsItem()
    : VisualItem()
{
    
}

EllipsItem::EllipsItem(QPointF& center,bool filled,int penSize,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent),m_penWidth(penSize)
{
    m_center = center;
    m_ry = 0;
    m_rx = 0;
    m_filled = filled;
}
QRectF EllipsItem::boundingRect() const
{
    QRectF rect;
    rect.setTopLeft(QPointF(m_center.x()-m_rx,m_center.y()-m_ry));
    rect.setBottomRight(QPointF(m_center.x()+m_rx,m_center.y()+m_ry));
    return rect;
}
void EllipsItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    if(!m_filled)
    {
        QPen pen = painter->pen();
        pen.setColor(m_color);
        pen.setWidth(m_penWidth);
        painter->setPen(pen);
    }
    else
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(m_color,Qt::SolidPattern));
    }

    setChildrenVisible(hasFocusOrChild());

    painter->drawEllipse(m_center,m_rx,m_ry);
    painter->restore();
    
}
void EllipsItem::setNewEnd(QPointF& p)
{
    //QRectF tmp= m_rect;
    float dx = p.x()-m_center.x();
    float dy = p.y()-m_center.y();

    m_rx = dx;
    m_ry = dy;
    
    //qDebug() << "dy = "<< dy << "dx = " << dx;
    /*  m_rect.setBottomRight(p);
    m_rect.setTopLeft(QPointF(m_center.x()-dx,m_center.y()-dy));*/
    
}
void EllipsItem::writeData(QDataStream& out) const
{
    out << m_rx;
    out << m_ry;
    out << m_center;
    out << m_filled;
    out << m_color;
}
VisualItem::ItemType EllipsItem::getType()
{
    return VisualItem::ELLISPE;
}

void EllipsItem::readData(QDataStream& in)
{
    in >> m_rx;
    in >> m_ry;
    in >> m_center;
    in >> m_filled;
    in >> m_color;
}
void EllipsItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    //radius
    msg->real(m_rx);
    msg->real(m_ry);
    //center
    msg->real(m_center.x());
    msg->real(m_center.y());
    msg->int8(m_filled);
    msg->rgb(m_color);
}
void EllipsItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
    //rect
    m_rx = msg->real();
    m_ry = msg->real();

    //center
    m_center.setX(msg->real());
    m_center.setY(msg->real());

    m_filled = msg->int8();
    m_color = msg->rgb();

}
void EllipsItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    switch ((int)pointId)
    {
    case 0:
        m_rx = pos.x()-m_center.x();
        break;
    case 1:
        m_ry = pos.y()-m_center.y();
        break;
    default:
        break;
    }
    update();

}
void EllipsItem::initChildPointItem()
{
    setTransformOriginPoint(m_center);
    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 2 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this);
        m_child->append(tmp);
    }
    m_child->value(0)->setPos(m_center.x()+m_rx,m_center.y());
    m_child->value(0)->setPlacement(ChildPointItem::MiddleRight);
    m_child->value(0)->setRotationEnable(true);
    m_child->value(0)->setMotion(ChildPointItem::X_AXIS);
    m_child->value(1)->setPos(m_center.x(),m_center.y()+m_ry);
    m_child->value(1)->setMotion(ChildPointItem::Y_AXIS);
    m_child->value(1)->setPlacement(ChildPointItem::ButtomCenter);
    m_child->value(1)->setRotationEnable(true);
}
VisualItem* EllipsItem::getItemCopy()
{//QPointF& center,bool filled,int penSize,QColor& penColor
    EllipsItem* ellipseItem = new EllipsItem(m_center,m_filled,m_penWidth,m_color);
    QPointF pos(m_rx+m_center.x(),m_ry+m_center.y());
    ellipseItem->setNewEnd(pos);
    return ellipseItem;
}
