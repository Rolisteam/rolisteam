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
#include "lineitem.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include "controller/item_controllers/linecontroller.h"
#include <QDebug>

#include <math.h>
#define PI 3.14159265

LineItem::LineItem(vmap::LineController* ctrl) : VisualItem(ctrl), m_lineCtrl(ctrl)
{
    auto func= [this]()
    {
        updateChildPosition();
        update();
    };
    connect(m_lineCtrl, &vmap::LineController::endPointChanged, this, func);
    connect(m_lineCtrl, &vmap::LineController::startPointChanged, this, func);

    for(int i= 0; i <= vmap::LineController::End; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_lineCtrl, i, this);
        tmp->setMotion(ChildPointItem::MOVE);
        m_children.append(tmp);
    }
    updateChildPosition();
}

void LineItem::setNewEnd(const QPointF& nend)
{
    m_lineCtrl->setCorner(nend, 1);
    // m_rect.setBottomRight(nend);
}
QRectF LineItem::boundingRect() const
{
    return m_lineCtrl->rect();
}
QPainterPath LineItem::shape() const
{
    QLineF line(m_lineCtrl->startPoint(), m_lineCtrl->endPoint());
    line.setLength(line.length() + m_lineCtrl->penWidth() / 2.0);
    QLineF line2(line.p2(), line.p1());
    line2.setLength(line2.length() + m_lineCtrl->penWidth() / 2.0);
    line.setPoints(line2.p2(), line2.p1());

    QLineF normal= line.normalVector();
    normal.setLength(m_lineCtrl->penWidth() / 2.0);
    auto start= normal.p2();
    auto end= normal.pointAt(-1);
    QLineF normal2= line2.normalVector();
    normal2.setLength(m_lineCtrl->penWidth() / 2.0);
    auto p2= normal2.p2();
    auto p3= normal2.pointAt(-1);

    QPainterPath path;
    path.moveTo(start);
    path.lineTo(p3);
    path.lineTo(p2);
    path.lineTo(end);
    path.lineTo(start);
    return path;
}
void LineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    painter->save();
    auto pen= painter->pen();
    pen.setColor(m_lineCtrl->color());
    pen.setWidth(m_lineCtrl->penWidth());
    painter->setPen(pen);
    painter->drawLine(m_lineCtrl->startPoint(), m_lineCtrl->endPoint());
    setChildrenVisible(hasFocusOrChild());
    painter->restore();

    if(canBeMoved() && (option->state & QStyle::State_MouseOver || isSelected()))
    {
        painter->save();
        auto shapePath= shape();
        QPen pen= painter->pen();
        pen.setColor(isSelected() ? m_selectedColor : m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawPath(shapePath);
        painter->restore();
    }
}

void LineItem::setRectSize(qreal x, qreal y, qreal w, qreal h)
{
    /*  m_rect.setX(x);
      m_rect.setY(y);
      m_rect.setWidth(w);
      m_rect.setHeight(h);

      m_startPoint= m_rect.topLeft();
      m_endPoint= m_rect.bottomRight();*/
}

void LineItem::updateChildPosition()
{
    if(!m_lineCtrl)
        return;

    m_children.value(0)->setPos(m_lineCtrl->startPoint());
    m_children.value(0)->setPlacement(ChildPointItem::Center);
    m_children.value(1)->setPos(m_lineCtrl->endPoint());
    m_children.value(1)->setPlacement(ChildPointItem::Center);
}
VisualItem* LineItem::getItemCopy()
{
    return nullptr;
    /* LineItem* line= new LineItem(m_startPoint, m_color, m_penWidth);
     line->setNewEnd(m_endPoint);
     line->setOpacity(opacity());
     line->setScale(scale());
     line->setRotation(rotation());
     line->setZValue(zValue());
     line->setLayer(getLayer());

     return line;*/
}

void LineItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change == ChildPointItem::Resizing)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(boundingRect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_lineCtrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                                   oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    VisualItem::endOfGeometryChange(change);
}
