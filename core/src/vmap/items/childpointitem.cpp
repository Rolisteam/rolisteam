/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include "controller/view_controller/vectorialmapcontroller.h"
#include "vmap/controller/visualitemcontroller.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <cmath>
#include <math.h>
#define PI 3.14159265

#define SQUARE_SIZE 6

ChildPointItem::ChildPointItem(vmap::VisualItemController* ctrl, int point, VisualItem* parent, bool isVision)
    : QGraphicsObject(parent)
    , m_ctrl(ctrl)
    , m_pointId(point)
    , m_parent(parent)
    , m_allowRotation(false)
    , m_vision(isVision)
{
    m_currentMotion= ALL;
    m_editable= true;
    // connect(m_ctrl, &vmap::VisualItemController::rotationChanged, this, [this]() { setRotation(-m_ctrl->rotation());
    // });
    // setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    // setAcceptHoverEvents(true);
    // setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
    // setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsFocusable);
}

ChildPointItem::~ChildPointItem() {}

QVariant ChildPointItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if(!m_editable)
        return QGraphicsItem::itemChange(change, value);

    if(change == ItemPositionChange /*&& scene()*/ && isSelected() && m_currentMotion != NONE
       && m_currentMotion != MOUSE)
    {
        QPointF newPos= value.toPointF();
        switch(m_currentMotion)
        {
        case X_AXIS:
            newPos.setY(pos().y());
            break;
        case Y_AXIS:
            newPos.setX(pos().x());

            break;
        case MOVE:
            m_parent->setPos(mapToScene(newPos));
            break;
        default:
            break;
        }
        if(newPos != value.toPointF())
        {
            return newPos;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}
QRectF ChildPointItem::boundingRect() const
{
    return QRectF(m_startPoint.x(), m_startPoint.y(), 2 * SQUARE_SIZE, 2 * SQUARE_SIZE);
}
void ChildPointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setPen(Qt::black);
    painter->fillRect(m_startPoint.x(), m_startPoint.y(), 2 * SQUARE_SIZE, 2 * SQUARE_SIZE, Qt::gray);
    painter->drawRect(m_startPoint.x(), m_startPoint.y(), 2 * SQUARE_SIZE, 2 * SQUARE_SIZE);
}
void ChildPointItem::setMotion(ChildPointItem::MOTION m)
{
    m_currentMotion= m;
    if((MOUSE == m_currentMotion) || (NONE == m_currentMotion))
    {
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    }
    else
    {
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges
                 | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    }
}
void ChildPointItem::setPlacement(ChildPointItem::PLACEMENT p)
{
    // MiddelLeft,MiddleRight,Center,ButtomLeft,ButtomRight,ButtomCenter};
    m_placement= p;
    switch(m_placement)
    {
    case TopLeft:
        m_startPoint.setX(0);
        m_startPoint.setY(0);
        break;
    case TopRight:
        m_startPoint.setX(-(2 * SQUARE_SIZE));
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
        m_startPoint.setX(-(2 * SQUARE_SIZE));
        m_startPoint.setY(-(SQUARE_SIZE));
        break;
    case Center:
        m_startPoint.setX(-(SQUARE_SIZE));
        m_startPoint.setY(-(SQUARE_SIZE));
        break;
    case ButtomLeft:
        m_startPoint.setX(0);
        m_startPoint.setY(-(2 * SQUARE_SIZE));
        break;
    case ButtomRight:
        m_startPoint.setX(-(2 * SQUARE_SIZE));
        m_startPoint.setY(-(2 * SQUARE_SIZE));
        break;
    case ButtomCenter:
        m_startPoint.setX(-(SQUARE_SIZE));
        m_startPoint.setY(-(2 * SQUARE_SIZE));
        break;
    }
}

ChildPointItem::PLACEMENT ChildPointItem::placement() const
{
    return m_placement;
}
/*void ChildPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    event->accept();
    QPointF v = pos() + event->pos();//distance vector
    qreal rot = m_parent->rotation();


    if(m_editable)
    {
        if(m_currentMotion == MOUSE)
        {
            if(!(event->modifiers() & Qt::ControlModifier))
            {
                VisualItem::TransformType transformType = VisualItem::NoTransform;

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
void ChildPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    event->accept();
    QPointF v= pos() + event->pos();
    if(!m_editable)
    {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }
    if((m_currentMotion == ChildPointItem::Y_AXIS || m_currentMotion == ChildPointItem::X_AXIS)
       && (m_currentChange == None || m_currentChange == Resizing))
    {
        m_currentChange= Resizing;
        auto move= event->pos() - event->lastPos();
        m_currentMotion == ChildPointItem::Y_AXIS ? move.setX(0) : move.setY(0);
        m_ctrl->setCorner(move, m_pointId);
    }
    if(!(event->modifiers() & Qt::ControlModifier) && m_currentMotion == MOUSE
       && (m_currentChange == None || m_currentChange == Resizing))
    {
        m_currentChange= Resizing;
        VisualItem::TransformType transformType= VisualItem::NoTransform;

        if((event->modifiers() & Qt::ShiftModifier))
        {
            transformType= VisualItem::KeepRatio;
        }
        auto W= qMax(2 * std::fabs(v.x()), 5.0);
        auto H= qMax(2 * std::fabs(v.y()), 4.0);
        if(event->modifiers() & Qt::AltModifier)
        {
            transformType= VisualItem::Sticky;
            int size= m_ctrl->gridSize();
            W= std::round(W / size) * size;
            H= std::round(H / size) * size;
        }

        auto move= event->pos() - event->lastPos();
        auto pItem= parentItem();
        qDebug() << move << pItem->pos() << pItem->boundingRect() << pItem->transformOriginPoint();
        // move= pTransform.map(move);

        m_ctrl->setCorner(move, m_pointId); // mapToScene(pos()) +
    }

    if(((m_currentMotion == MOUSE) || (m_allowRotation)) && (event->modifiers() & Qt::ControlModifier)
       && (m_currentChange == None || m_currentChange == Rotation))
    {
        if(v.isNull())
            return;
        m_currentChange= Rotation;

        QPointF refPos= pos();

        // set item rotation (set rotation relative to current)
        qreal refAngle= atan2(refPos.y(), refPos.x());
        qreal newAngle= atan2(v.y(), v.x());
        double dZr= 57.29577951308232 * (newAngle - refAngle); // 180 * a / M_PI
        double zr= m_ctrl->rotation() + dZr;

        m_ctrl->setRotation(zr);
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}
void ChildPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_parent->endOfGeometryChange(m_currentChange);
    m_currentChange= None;

    QGraphicsItem::mouseReleaseEvent(event);
}
void ChildPointItem::setRotationEnable(bool allow)
{
    m_allowRotation= allow;
}
void ChildPointItem::setEditableItem(bool b)
{
    m_editable= b;
    if((m_editable) && (((MOUSE == m_currentMotion) || (NONE == m_currentMotion))))
    {
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    }
    else if(m_editable)
    {
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges
                 | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    }
}
void ChildPointItem::setPointID(int a)
{
    m_pointId= a;
}
int ChildPointItem::getPointID() const
{
    return m_pointId;
}
bool ChildPointItem::isVisionHandler()
{
    return m_vision;
}
void ChildPointItem::setVisionHandler(bool b)
{
    m_vision= b;
}
