/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "mindmap/geometry/linknode.h"

#define PenWidth 1

#include <QDebug>
#include <QLineF>
namespace mindmap
{
LinkNode::LinkNode() : m_geometry(QSGGeometry::defaultAttributes_Point2D(), 0)
{
    setGeometry(&m_geometry);
    m_geometry.setDrawingMode(QSGGeometry::DrawLineStrip);
    m_geometry.allocate(6);
    setMaterial(&m_material);
}
void LinkNode::setColor(const QColor& color)
{
    m_material.setColor(color);
    markDirty(QSGNode::DirtyMaterial);
}
void LinkNode::update(const QRectF& rect, LinkController::Orientation orient, const QRectF& startBox,
                      const QRectF& endBox)
{
    qreal arrowLenght= 10.0;
    qreal arrowWidth= 8.0;
    qreal radius= 0.;
    qreal diameter= 0.;
    m_geometry.setLineWidth(PenWidth);

    QPointF p1, p2;
    QRectF rect1= startBox;
    rect1.moveTo(-startBox.width() / 2, -startBox.height() / 2);
    QRectF rect2= endBox;
    rect2.moveTo(-endBox.width() / 2, -endBox.height() / 2);

    switch(orient)
    {
    case LinkController::RightBottom:
    {
        p1= rect.topLeft();
        p2= rect.bottomRight();
        rect2= rect2.translated(p2.x(), p2.y());
    }
    break;
    case LinkController::LeftBottom:
    {
        p1= rect.topRight();
        p2= rect.bottomLeft();
        rect2= rect2.translated(p2.x(), p2.y());
        rect1= rect1.translated(p1.x(), p1.y());
    }
    break;
    case LinkController::RightTop:
    {
        p1= rect.bottomLeft();
        p2= rect.topRight();
        rect2= rect2.translated(p2.x(), p2.y());
        rect1= rect1.translated(p1.x(), p1.y());
    }
    break;
    case LinkController::LeftTop:
    {
        p1= rect.bottomRight();
        p2= rect.topLeft();
        rect1= rect1.translated(p1.x(), p1.y());
    }
    break;
    }

    QLineF line(p1, p2);
    // qDebug() << "center:" << line.center();

    QLineF rect1Bottom(rect1.bottomLeft(), rect1.bottomRight());
    QLineF rect1Top(rect1.topLeft(), rect1.topRight());
    QLineF rect1Left(rect1.topLeft(), rect1.bottomLeft());
    QLineF rect1Right(rect1.topRight(), rect1.bottomRight());

    QVector<QLineF> lines({rect1Bottom, rect1Top, rect1Left, rect1Right});

    QPointF intersection1;
    for(auto const& rectSide : std::as_const(lines))
    {
        QPointF point;
        if(line.intersects(rectSide, &point) == QLineF::BoundedIntersection)
            intersection1= point;
    }

    QLineF rect2Bottom(rect2.bottomLeft(), rect2.bottomRight());
    QLineF rect2Top(rect2.topLeft(), rect2.topRight());
    QLineF rect2Left(rect2.topLeft(), rect2.bottomLeft());
    QLineF rect2Right(rect2.topRight(), rect2.bottomRight());

    QVector<QLineF> lines2({rect2Bottom, rect2Top, rect2Left, rect2Right});

    QPointF intersection2;
    for(auto const& rectSide : std::as_const(lines2))
    {
        QPointF point;
        if(line.intersects(rectSide, &point) == QLineF::BoundedIntersection)
            intersection2= point;
    }

    line= QLineF(intersection1, intersection2);

    auto pArrow= line.pointAt(1 - radius / line.length());
    auto startArrow= line.pointAt(1 - radius / line.length() - arrowLenght / line.length());

    QLineF arrowLine(startArrow, pArrow);
    QLineF arrowBase= arrowLine.normalVector();

    auto pointArrow= arrowBase.pointAt(arrowWidth / arrowBase.length());
    auto pointArrow2= arrowBase.pointAt(-arrowWidth / arrowBase.length());
    auto vertices= m_geometry.vertexDataAsPoint2D();
    {
        vertices[0].set(static_cast<float>(intersection1.x() + diameter),
                        static_cast<float>(intersection1.y() + diameter));
        vertices[1].set(static_cast<float>(startArrow.x() + diameter), static_cast<float>(startArrow.y() + diameter));
        vertices[2].set(static_cast<float>(pointArrow.x() + diameter), static_cast<float>(pointArrow.y() + diameter));
        vertices[3].set(static_cast<float>(pArrow.x() + diameter), static_cast<float>(pArrow.y() + diameter));
        vertices[4].set(static_cast<float>(pointArrow2.x() + diameter), static_cast<float>(pointArrow2.y() + diameter));
        vertices[5].set(static_cast<float>(startArrow.x() + diameter), static_cast<float>(startArrow.y() + diameter));
    }
    markDirty(QSGNode::DirtyGeometry);
}
} // namespace mindmap
