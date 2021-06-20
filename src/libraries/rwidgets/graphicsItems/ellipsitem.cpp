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
#include <QStyleOptionGraphicsItem>

#include <cmath>
#include <math.h>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "vmap/controller/ellipsecontroller.h"

EllipsItem::EllipsItem(vmap::EllipseController* ctrl) : VisualItem(ctrl), m_ellipseCtrl(ctrl)
{
    connect(m_ellipseCtrl, &vmap::EllipseController::rxChanged, this, [this]() {
        setTransformOriginPoint(boundingRect().center());
        updateChildPosition();
    });
    connect(m_ellipseCtrl, &vmap::EllipseController::ryChanged, this, [this]() {
        setTransformOriginPoint(boundingRect().center());
        updateChildPosition();
    });
    connect(m_ellipseCtrl, &vmap::EllipseController::filledChanged, this, [this]() { update(); });

    initChildPointItem();
}

QRectF EllipsItem::boundingRect() const
{
    return m_ellipseCtrl->rect();
}

QPainterPath EllipsItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());

    if(!m_ellipseCtrl->filled())
    {
        QPainterPath subpath;

        QRectF rect= boundingRect();
        auto penW= m_ellipseCtrl->penWidth();
        rect.adjust(penW, penW, -penW, -penW);
        subpath.addEllipse(rect);
        path-= subpath;
    }

    return path;
}
void EllipsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    if(!m_ellipseCtrl->filled())
    {
        QPen pen= painter->pen();
        pen.setColor(m_ellipseCtrl->color());
        pen.setWidth(m_ellipseCtrl->penWidth());
        painter->setPen(pen);
    }
    else
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(m_ellipseCtrl->color(), Qt::SolidPattern));
    }

    setChildrenVisible(hasFocusOrChild());

    painter->drawEllipse(QPointF(0, 0), m_ellipseCtrl->rx(), m_ellipseCtrl->ry());

    painter->restore();

    if(option->state & QStyle::State_MouseOver || isUnderMouse())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawEllipse(QPointF(0, 0), m_ellipseCtrl->rx(), m_ellipseCtrl->ry());
        painter->restore();
    }
}
void EllipsItem::setNewEnd(const QPointF& p)
{
    m_ellipseCtrl->setCorner(p, 2);
}

void EllipsItem::updateChildPosition()
{
    auto rect= boundingRect();
    m_children.value(0)->setPos(QPointF(rect.right(), rect.center().y()));
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    m_children.value(1)->setPos(QPointF(rect.center().x(), rect.bottom()));
    m_children.value(1)->setPlacement(ChildPointItem::ButtomCenter);
    update();
}

void EllipsItem::initChildPointItem()
{
    setTransformOriginPoint(boundingRect().center());

    for(int i= 0; i < 2; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_ctrl, i, this);
        m_children.append(tmp);
    }
    m_children.value(0)->setPos(m_ellipseCtrl->rx(), 0);
    m_children.value(0)->setPlacement(ChildPointItem::MiddleRight);
    m_children.value(0)->setMotion(ChildPointItem::X_AXIS | ChildPointItem::ROTATION);
    m_children.value(1)->setPos(0, m_ellipseCtrl->ry());
    m_children.value(1)->setMotion(ChildPointItem::Y_AXIS | ChildPointItem::ROTATION);
    m_children.value(1)->setPlacement(ChildPointItem::ButtomCenter);
}

/*void EllipsItem::setHoldSize(bool holdSize)
{
    VisualItem::setHoldSize(holdSize);

    std::for_each(m_children.begin(), m_children.end(), [holdSize](ChildPointItem* tmp) {
        if(holdSize)
            tmp->setMotion(ChildPointItem::NONE);
        else
            tmp->setMotion((tmp->placement() == ChildPointItem::ButtomCenter) ? ChildPointItem::Y_AXIS :
                                                                                ChildPointItem::X_AXIS);
    });
}*/

void EllipsItem::writeData(QDataStream& out) const
{
    /*   out << m_rx;
       out << m_ry;
       out << static_cast<int>(m_layer);
       out << opacity();
       out << m_center;
       out << m_filled;
       out << m_color;
       out << m_penWidth;

       out << pos();
       out << scale();
       out << rotation();*/
}

void EllipsItem::readData(QDataStream& in)
{
    /*  in >> m_rx;
       in >> m_ry;
       int lay;
       in >> lay;
       m_layer= static_cast<Core::Layer>(lay);
       qreal opa= 0;
       in >> opa;
       setOpacity(opa);
       in >> m_center;
       in >> m_filled;
       in >> m_color;
       in >> m_penWidth;

       QPointF position;
       in >> position;
       setPos(position);

       qreal scale;
       in >> scale;
       setScale(scale);

       qreal rotation;
       in >> rotation;
       setRotation(rotation);
       m_rect.setRect(-m_rx, -m_ry, m_rx * 2, m_ry * 2);*/
}
void EllipsItem::fillMessage(NetworkMessageWriter* msg)
{
    /* msg->string16(m_id);
     msg->real(scale());
     msg->real(rotation());
     msg->uint8(static_cast<int>(m_layer));
     msg->real(zValue());
     msg->real(opacity());
     //
     msg->real(pos().x());
     msg->real(pos().y());
     // radius
     msg->real(m_rx);
     msg->real(m_ry);
     // center
     msg->real(m_center.x());
     msg->real(m_center.y());
     msg->int8(m_filled);
     msg->rgb(m_color.rgb());
     msg->uint16(m_penWidth);*/
}
void EllipsItem::readItem(NetworkMessageReader* msg)
{
    /*  m_id= msg->string16();
      setScale(msg->real());
      setRotation(msg->real());
      m_layer= static_cast<Core::Layer>(msg->uint8());
      setZValue(msg->real());
      setOpacity(msg->real());

      // x , y
      qreal posx= msg->real();
      qreal posy= msg->real();

      // radius
      m_rx= msg->real();
      m_ry= msg->real();

      // center
      m_center.setX(msg->real());
      m_center.setY(msg->real());

      m_filled= msg->int8();
      m_color= msg->rgb();
     // m_penWidth= msg->uint16();

      m_rect.setRect(-m_rx, -m_ry, m_rx * 2, m_ry * 2);
      setPos(posx, posy);
      update();*/
}
void EllipsItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    /* if(m_holdSize)
     {
         return;
     }

     switch(static_cast<int>(pointId))
     {
     case 0:
         m_rx= pos.x() - m_center.x();
         break;
     case 1:
         m_ry= pos.y() - m_center.y();
         break;
     default:
         break;
     }
     if(m_ry < 0.1)
     {
         m_ry= 0.1;
         pos.setY(m_center.y() + m_ry);
     }
     if(m_rx < 0.1)
     {
         m_rx= 0.1;
         pos.setX(m_center.x() + m_rx);
     }
     // m_rect.setRect(-m_rx, -m_ry, m_rx * 2, m_ry * 2);
     m_resizing= true;
     update();*/
}

VisualItem* EllipsItem::getItemCopy()
{ // QPointF& center,bool filled,int penSize,QColor& penColor
    // EllipsItem* ellipseItem= new EllipsItem(m_ctrl /*, m_center, m_filled, m_penWidth, m_color*/);
    // QPointF pos(m_rx + m_center.x(), m_ry + m_center.y());
    // ellipseItem->setNewEnd(pos);
    // return ellipseItem;
    return nullptr;
}

void EllipsItem::setRectSize(qreal x, qreal y, qreal w, qreal h)
{
    /*  VisualItem::setRectSize(x, y, w, h);
      m_rx= w / 2;
      m_ry= h / 2;
      update();*/
}
