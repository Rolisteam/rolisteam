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
#include <QDebug>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

QPainterPath vectorToFullPath(const std::vector<QPointF>& points, qreal penWidth= 10., bool closeUp= false)
{
    QPainterPath path;
    if(points.size() == 1)
        return path;

    bool first= true;
    std::vector<QPointF> topPoints;
    std::vector<QPointF> bottomPoints;

    int i= 0;
    for(auto current : points)
    {
        // QPointF current= points[i];
        QPointF lastPoint;
        QPointF nextPoint;
        if(points.size() > i + 1)
            nextPoint= points[i + 1];
        if(i > 0)
            lastPoint= points[i - 1];

        QLineF line(current, lastPoint);
        QLineF line2(current, nextPoint);

        if(nextPoint.isNull())
        {
            auto normal1= line.normalVector();
            normal1.setLength(-normal1.length());
            topPoints.push_back(normal1.pointAt(penWidth / (normal1.length() * 2)));
            bottomPoints.push_back(normal1.pointAt(-penWidth / (normal1.length() * 2)));
        }
        else if(lastPoint.isNull())
        {
            auto normal2= line2.normalVector();

            topPoints.push_back(normal2.pointAt(penWidth / (normal2.length() * 2)));
            bottomPoints.push_back(normal2.pointAt(-penWidth / (normal2.length() * 2)));
        }
        else
        {
            auto normal1= line.normalVector();
            normal1.setLength(-normal1.length());
            auto normal2= line2.normalVector();

            topPoints.push_back((normal1.pointAt(penWidth / (normal1.length() * 2))
                                 + normal2.pointAt(penWidth / (normal2.length() * 2)))
                                / 2);
            bottomPoints.push_back((normal1.pointAt(-penWidth / (normal1.length() * 2))
                                    + normal2.pointAt(-penWidth / (normal2.length() * 2)))
                                   / 2);
        }
    }

    for(auto i= bottomPoints.rbegin(); i != bottomPoints.rend(); ++i)
    {
        topPoints.push_back(*i);
    }

    if(closeUp)
        topPoints.push_back(topPoints.at(0));

    for(const auto& point : topPoints)
    {
        if(first)
        {
            path.moveTo(point);
            first= false;
        }
        else
            path.lineTo(point);
    }

    return path;
}

PathItem::PathItem(vmap::PathController* ctrl) : VisualItem(ctrl), m_pathCtrl(ctrl)
{
    if(!m_pathCtrl)
        return;

    connect(m_pathCtrl, &vmap::PathController::positionChanged, this, [this](int corner, QPointF pos) {
        if(m_children.empty())
            return;

        if(corner == qBound(0, corner, m_children.size() - 1))
            m_children[corner]->setPos(pos);
        update();
    });
    connect(m_pathCtrl, &vmap::PathController::pointAdded, this, &PathItem::addChild);

    int i= 0;
    if(!m_pathCtrl->penLine())
    {
        for(auto point : m_pathCtrl->points())
            addChild(point, i);
    }

    createActions();
    update();
}

QRectF PathItem::boundingRect() const
{
    if(m_pathCtrl)
        return m_pathCtrl->rect();
    else
        return {};
}

QPainterPath PathItem::shape() const
{
    return vectorToFullPath(m_pathCtrl->points(), 10);
}

void PathItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    setChildrenVisible(hasFocusOrChild());

    QPainterPath path= m_pathCtrl->path(); // vectorToPath(m_pathCtrl->points(), m_pathCtrl->closed());

    painter->save();
    auto pen= painter->pen();
    pen.setColor(m_pathCtrl->color());
    pen.setWidth(m_pathCtrl->penWidth());
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    if(m_pathCtrl->filled())
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
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        QTransform scale= QTransform().scale(1.01, 1.01);
        painter->drawPath(scale.map(path));
        painter->restore();
    }
}

void PathItem::setNewEnd(const QPointF& p)
{
    if(m_pathCtrl->penLine())
    {
        auto p0= m_pathCtrl->pointAt(m_pathCtrl->pointCount() - 1);
        m_pathCtrl->addPoint(p0 + p);
    }
    else
        m_pathCtrl->setCorner(p, m_pathCtrl->pointCount());
}

void PathItem::createActions()
{
    m_closeAct= new QAction(tr("Close Path"), this);
    m_closeAct->setCheckable(true);
    connect(m_closeAct, &QAction::triggered, m_pathCtrl, &vmap::PathController::setClosed);

    m_fillAct= new QAction(tr("Fill Path"), this);
    m_fillAct->setCheckable(true);
    connect(m_fillAct, &QAction::triggered, m_pathCtrl, &vmap::PathController::setFilled);
}

void PathItem::addActionContextMenu(QMenu& menu)
{
    menu.addAction(m_closeAct);
    menu.addAction(m_fillAct);
    VisualItem::addActionContextMenu(menu);
}
void PathItem::addPoint(const QPointF& point)
{
    m_pathCtrl->addPoint(point);
}

void PathItem::addChild(const QPointF& point, int i)
{
    if(m_pathCtrl->penLine())
        return;

    ChildPointItem* tmp= new ChildPointItem(m_ctrl, i, this);
    tmp->setMotion(ChildPointItem::MOUSE);
    m_children.push_back(tmp);
    tmp->setPos(point);
    tmp->setPlacement(ChildPointItem::Center);
}

VisualItem* PathItem::getItemCopy()
{
    /*PathItem* path= new PathItem();
    path->setPath(m_pointVector);
    path->setStartPoint(m_start);
    path->setPenColor(m_color);
    path->setPenWidth(m_penWidth);
    path->setClosed(m_closed);
    path->setFilled(m_filled);
    path->setPos(pos());*/
    return nullptr; // path
}
