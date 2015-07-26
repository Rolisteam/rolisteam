/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "childpointitem.h"
#include "visualitem.h"

#include <QGraphicsScene>
#include <QPainter>

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include <math.h>
#define PI 3.14159265

#define SQUARE_SIZE 6

ChildPointItem::ChildPointItem(qreal point,VisualItem* parent)
    : QGraphicsObject(parent),m_pointId(point),m_parent(parent)
{
    m_currentMotion = ALL;
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
}

ChildPointItem::~ChildPointItem()
{

}

QVariant ChildPointItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene() && hasFocus())
    {
        QPointF newPos = value.toPointF();
        if(m_currentMotion == X_AXIS)
        {
            newPos.setY(pos().y());
        }
        else if( Y_AXIS == m_currentMotion)
        {
            newPos.setX(pos().x());
        }
        m_parent->setGeometryPoint(m_pointId,newPos);
        if(newPos != value.toPointF())
        {
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
QRectF ChildPointItem::boundingRect() const
{
    return QRectF(m_startPoint.x(), m_startPoint.y(), 2*SQUARE_SIZE, 2*SQUARE_SIZE);
}
void ChildPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::black);
    painter->fillRect(m_startPoint.x(), m_startPoint.y(), 2*SQUARE_SIZE, 2*SQUARE_SIZE,Qt::gray);
    painter->drawRect(m_startPoint.x(), m_startPoint.y(), 2*SQUARE_SIZE, 2*SQUARE_SIZE);
}
void ChildPointItem::setMotion(ChildPointItem::MOTION m)
{
    m_currentMotion = m;
}
void ChildPointItem::setPlacement(ChildPointItem::PLACEMENT p)
{
    //MiddelLeft,MiddleRight,Center,ButtomLeft,ButtomRight,ButtomCenter};

    switch(p)
    {
    case TopLeft:
        m_startPoint.setX(0);
        m_startPoint.setY(0);
        break;
    case TopRight:
        m_startPoint.setX(-(2*SQUARE_SIZE));
        m_startPoint.setY(0);
        break;
    case TopCenter:
        m_startPoint.setX(-(SQUARE_SIZE));
        m_startPoint.setY(0);
        break;
    case MiddelLeft:
        m_startPoint.setX(0);
        m_startPoint.setY(-(SQUARE_SIZE));
        break;
    case MiddleRight:
        m_startPoint.setX(-(2*SQUARE_SIZE));
        m_startPoint.setY(-(SQUARE_SIZE));
        break;
    case Center:
        m_startPoint.setX(-(SQUARE_SIZE));
        m_startPoint.setY(-(SQUARE_SIZE));
        break;
    case ButtomLeft:
        m_startPoint.setX(0);
        m_startPoint.setY(-(2*SQUARE_SIZE));
        break;
    case ButtomRight:
        m_startPoint.setX(-(2*SQUARE_SIZE));
        m_startPoint.setY(-(2*SQUARE_SIZE));
        break;
    case ButtomCenter:
        m_startPoint.setX(-(SQUARE_SIZE));
        m_startPoint.setY(-(2*SQUARE_SIZE));
        break;
    }


}
void ChildPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
   // if( ROTATION == m_currentMotion)
    if(event->modifiers() & Qt::ControlModifier)
    {
        event->accept();
        QPointF v = pos() + event->pos();
        if (v.isNull())
            return;

        QPointF refPos = pos();

        // set item rotation (set rotation relative to current)
        qreal refAngle = atan2(refPos.y(), refPos.x());
        qreal newAngle = atan2(v.y(), v.x());
        double dZr = 57.29577951308232 * (newAngle - refAngle); // 180 * a / M_PI
        double zr = m_parent->rotation() + dZr;

        // apply rotation
        m_parent->setRotation(zr);
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}
