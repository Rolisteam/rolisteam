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
#include "highlighteritem.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QDebug>

#include <math.h>
#include <cmath>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"
#include "preferences/preferencesmanager.h"

HighlighterItem::HighlighterItem()
    : VisualItem()
{
    
}

HighlighterItem::HighlighterItem(QPointF& center,int penSize,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent),m_penWidth(penSize)
{
    m_center = center;
    setPos(m_center);
    m_center.setX(0);
    m_center.setY(0);
    m_radius = 0;

    initAnimation();
}
void HighlighterItem::initAnimation()
{
    auto const preferences = PreferencesManager::getInstance();
    m_animation = new QPropertyAnimation(this, "radius");
    m_animation->setDuration(preferences->value("Map_Highlighter_time",1000).toInt());
    m_animation->setStartValue(0);
    m_animation->setEndValue(preferences->value("Map_Highlighter_radius",100).toInt());
    m_animation->setEasingCurve(QEasingCurve::Linear);
    m_animation->setLoopCount(preferences->value("Map_Highlighter_loop",3).toInt());
    m_animation->start();

    connect(m_animation,&QPropertyAnimation::finished,this,[this](){
            emit itemRemoved(m_id);
    });
}
QRectF HighlighterItem::boundingRect() const
{
    return m_rect;
}
QPainterPath HighlighterItem::shape() const
{
	QPainterPath path;
	path.addEllipse(boundingRect());
	return path;
}
void HighlighterItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    QPen pen = painter->pen();
    pen.setColor(m_color);
    pen.setWidth(m_penWidth);
    painter->setPen(pen);

    painter->drawEllipse(m_center,m_radius,m_radius);

    painter->restore();
}
void HighlighterItem::setRadius(qreal radius)
{
    m_radius = radius;
    update();
}
qreal HighlighterItem::getRadius() const
{
    return m_radius;
}
void HighlighterItem::setNewEnd(QPointF& p)
{
    Q_UNUSED(p)
  /*  m_radius = std::fabs(p.x()-pos().x())*sqrt(2);

    m_rect.setRect(-m_radius,-m_radius,m_radius*2,m_radius*2);*/
}
VisualItem::ItemType HighlighterItem::getType() const
{
    return VisualItem::HIGHLIGHTER;
}
void HighlighterItem::writeData(QDataStream& out) const
{
    Q_UNUSED(out)
}

void HighlighterItem::readData(QDataStream& in)
{
    Q_UNUSED(in)
}
void HighlighterItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    msg->uint8((int)m_layer);
    msg->real(zValue());
    msg->real(opacity());
    msg->real(pos().x());
    msg->real(pos().y());
    //radius
    msg->real(m_radius);
    //center
    msg->real(m_center.x());
    msg->real(m_center.y());
    msg->rgb(m_color.rgb());
    msg->int16(m_penWidth);
}
void HighlighterItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
    m_layer = (VisualItem::Layer)msg->uint8();
    setZValue(msg->real());
    setOpacity(msg->real());

    //x , y
    qreal posx = msg->real();
    qreal posy = msg->real();

    //radius
    m_radius = msg->real();

    //center
    m_center.setX(msg->real());
    m_center.setY(msg->real());

    m_color = msg->rgb();
    m_penWidth = msg->int16();


    setPos(posx,posy);

    initAnimation();
}
void HighlighterItem::setGeometryPoint(qreal,QPointF&)
{

}
void HighlighterItem::initChildPointItem()
{

}
VisualItem* HighlighterItem::getItemCopy()
{
    return nullptr;
}
