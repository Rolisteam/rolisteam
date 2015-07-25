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

#define SQUARE_SIZE 8

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
    return QRectF(-SQUARE_SIZE, -SQUARE_SIZE, 2*SQUARE_SIZE, 2*SQUARE_SIZE);
}
void ChildPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawRect(-SQUARE_SIZE, -SQUARE_SIZE, 2*SQUARE_SIZE, 2*SQUARE_SIZE);
}
void ChildPointItem::setMotion(ChildPointItem::MOTION m)
{
    m_currentMotion = m;
}
