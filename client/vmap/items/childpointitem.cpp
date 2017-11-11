/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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
#include <cmath>
#define PI 3.14159265

#define SQUARE_SIZE 6

ChildPointItem::ChildPointItem(qreal point,VisualItem* parent,bool isVision )
    : QGraphicsObject(parent),m_pointId(point),m_parent(parent),m_allowRotation(false),m_vision(isVision)
{
    m_currentMotion = ALL;
    m_editable = true;
   // setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    // setAcceptHoverEvents(true);
    //setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    //setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsFocusable);
}

ChildPointItem::~ChildPointItem()
{

}

QVariant ChildPointItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(m_editable)
    {
        if (change == ItemPositionChange /*&& scene()*/ && isSelected() && m_currentMotion!=NONE && m_currentMotion!=MOUSE)
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

			if( MOVE == m_currentMotion)
			{
				m_parent->setPos(mapToScene(newPos));
				/*QPointF p=pos() - newPos;
				m_parent->moveBy(p.x(),p.y());*/
			}
			else
			{
				m_parent->setGeometryPoint(m_pointId,newPos);
			}
            if(newPos != value.toPointF())
            {
                return newPos;
            }
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
    if((MOUSE == m_currentMotion)||(NONE == m_currentMotion))
    {
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsFocusable);
    }
    else
    {
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    }
}
void ChildPointItem::setPlacement(ChildPointItem::PLACEMENT p)
{
    //MiddelLeft,MiddleRight,Center,ButtomLeft,ButtomRight,ButtomCenter};
    m_placement = p;
    switch(m_placement)
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
/*void ChildPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{ 
    event->accept();
    qDebug() << pos() << event->pos() << "taritnatenate ";
    QPointF v = pos() + event->pos();//distance vector
    qreal rot = m_parent->rotation();


    if(m_editable)
    {
        if(m_currentMotion == MOUSE)
        {
            if(!(event->modifiers() & Qt::ControlModifier))
            {
                VisualItem::TransformType transformType = VisualItem::NoTransform;

                //qDebug() << "before"<< v  << event->pos() << pos();

                qreal W = 0;//qMax(fabs(v.x()), 5.0);
                qreal H = 0;//qMax(fabs(v.y()), 4.0);
                qreal X = 0;
                qreal Y = 0;
                m_parent->setRotation(0);
                v = event->pos();//distance vector
                switch(m_placement)
                {
                    case TopLeft://topleft
                       // m_parent->setTransformOriginPoint(m_parent->boundingRect().bottomRight());
                        X = v.x();
                        Y = v.y();
                        W = m_parent->boundingRect().width() - X;
                        H = m_parent->boundingRect().height() - Y;
                        break;
                    case TopRight://topright
                       // m_parent->setTransformOriginPoint(m_parent->boundingRect().bottomLeft());
                        Y = v.y();
                        W = m_parent->boundingRect().width() + v.x();
                        H = m_parent->boundingRect().height() - Y;
                        break;
                    case ButtomRight:// bottom right
                       // m_parent->setTransformOriginPoint(m_parent->boundingRect().topLeft());
                        W = m_parent->boundingRect().width() + v.x();
                        H = m_parent->boundingRect().height() + v.y();
                        break;
                    case ButtomLeft://bottom left
                        //m_parent->setTransformOriginPoint(m_parent->boundingRect().topRight());
                        X = v.x();
                        H = m_parent->boundingRect().height() + v.y();
                        W = m_parent->boundingRect().width() - X;
                        break;
                }
                if((event->modifiers() & Qt::ShiftModifier))
                {
                    transformType = VisualItem::KeepRatio;
                }
                else if(event->modifiers() & Qt::AltModifier)
                {
                    transformType = VisualItem::Sticky;
                    int size = m_parent->getOption(VisualItem::GridSize).toInt();
                    W = std::round(W/size)*size;
                    H = std::round(H/size)*size;
                }

                {
                    if(rot == 0)
                    {
                        m_parent->moveBy(X,Y);
                    }
                    m_parent->resizeContents(QRectF(0, 0, W, H),transformType);
                    //m_parent->setTransformOriginPoint(m_parent->boundingRect().center());
                    m_parent->setRotation(rot);
                }
            }
        }
        if(((m_currentMotion == MOUSE)||(m_allowRotation))&&(event->modifiers() & Qt::ControlModifier))
        {
                if (v.isNull())
                    return;

                QPointF refPos = pos();

                // set item rotation (set rotation relative to current)
                qreal refAngle = atan2(refPos.y(), refPos.x());
                qreal newAngle = atan2(v.y(), v.x());
                double dZr = 57.29577951308232 * (newAngle - refAngle); // 180 * a / M_PI
                double zr = m_parent->rotation() + dZr;
                m_parent->setTransformOriginPoint(m_parent->boundingRect().center());
                m_parent->setRotation(zr);
        }
        else
        {
               QGraphicsItem::mouseMoveEvent(event);
        }
    }
    else
    {
            QGraphicsItem::mouseMoveEvent(event);
    }
}*/
void ChildPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    QPointF v = pos() + event->pos();
    if(m_editable)
    {
        if(m_currentMotion == MOUSE)
        {
            if(!(event->modifiers() & Qt::ControlModifier))
            {
                VisualItem::TransformType transformType = VisualItem::NoTransform;

                if((event->modifiers() & Qt::ShiftModifier))
                {
                    transformType = VisualItem::KeepRatio;
                }
                int W = qMax(2 * fabs(v.x()), 5.0);
                int H = qMax(2 * fabs(v.y()), 4.0);
                if(event->modifiers() & Qt::AltModifier)
                {
                    transformType = VisualItem::Sticky;
                    int size = m_parent->getOption(VisualItem::GridSize).toInt();
                    W = std::round(W/size)*size;
                    H = std::round(H/size)*size;
                }

                //if((v.x() >1)&&(v.y()>1))
               // qDebug() << v.y() << v.x();
                {
                    m_parent->resizeContents(QRectF(-W / 2, -H / 2, W, H),transformType);
                }
            }

        }
        if(((m_currentMotion == MOUSE)||(m_allowRotation))&&(event->modifiers() & Qt::ControlModifier))
        {
                if (v.isNull())
                    return;

                QPointF refPos = pos();

                // set item rotation (set rotation relative to current)
                qreal refAngle = atan2(refPos.y(), refPos.x());
                qreal newAngle = atan2(v.y(), v.x());
                double dZr = 57.29577951308232 * (newAngle - refAngle); // 180 * a / M_PI
                double zr = m_parent->rotation() + dZr;

                m_parent->setRotation(zr);
        }
        else
        {
               QGraphicsItem::mouseMoveEvent(event);
        }
    }
    else
    {
            QGraphicsItem::mouseMoveEvent(event);
    }
}
void ChildPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    m_parent->endOfGeometryChange();

    QGraphicsItem::mouseReleaseEvent(event);
}
void ChildPointItem::setRotationEnable(bool allow)
{
    m_allowRotation = allow;
}
void ChildPointItem::setEditableItem(bool b)
{
    m_editable = b;
    if((m_editable)&&(((MOUSE == m_currentMotion)||(NONE == m_currentMotion))))
    {
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsFocusable);
    }
    else if(m_editable)
    {
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    }
}
void ChildPointItem::setPointID(qreal a)
{
    m_pointId = a;
}
qreal ChildPointItem::getPointID() const
{
    return m_pointId;
}
bool ChildPointItem::isVisionHandler()
{
    return m_vision;
}
void ChildPointItem::setVisionHandler(bool b)
{
    m_vision = b;
}
