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
#include "linknode.h"

#define PenWidth 4

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
void LinkNode::update(const QPointF& p1, const QPointF& p2)
{
    qreal arrowLenght= 10.0;
    qreal arrowWidth= 8.0;
    qreal radius= 0.;
    qreal diameter= 0.;
    m_geometry.setLineWidth(PenWidth);
    QLineF line(p1, p2);

    auto pArrow= line.pointAt(1 - radius / line.length());
    auto startArrow= line.pointAt(1 - radius / line.length() - arrowLenght / line.length());

    QLineF arrowLine(startArrow, pArrow);
    QLineF arrowBase= arrowLine.normalVector();

    auto pointArrow= arrowBase.pointAt(arrowWidth / arrowBase.length());
    auto pointArrow2= arrowBase.pointAt(-arrowWidth / arrowBase.length());
    auto vertices= m_geometry.vertexDataAsPoint2D();
    {
        vertices[0].set(static_cast<float>(p1.x() + diameter), static_cast<float>(p1.y() + diameter));
        vertices[1].set(static_cast<float>(startArrow.x() + diameter), static_cast<float>(startArrow.y() + diameter));
        vertices[2].set(static_cast<float>(pointArrow.x() + diameter), static_cast<float>(pointArrow.y() + diameter));
        vertices[3].set(static_cast<float>(pArrow.x() + diameter), static_cast<float>(pArrow.y() + diameter));
        vertices[4].set(static_cast<float>(pointArrow2.x() + diameter), static_cast<float>(pointArrow2.y() + diameter));
        vertices[5].set(static_cast<float>(startArrow.x() + diameter), static_cast<float>(startArrow.y() + diameter));
    }
    markDirty(QSGNode::DirtyGeometry);
}
} // namespace mindmap
