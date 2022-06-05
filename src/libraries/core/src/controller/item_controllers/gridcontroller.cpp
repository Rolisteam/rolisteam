/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "controller/item_controllers/gridcontroller.h"
#include "media/mediatype.h"

#include "controller/view_controller/vectorialmapcontroller.h"

#include <QDebug>
#include <QPainter>
#include <cmath>
#include <math.h>

namespace vmap
{
vmap::GridController::GridController(VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::GRID, std::map<QString, QVariant>(), ctrl, parent)
{
    setLayer(Core::Layer::GRIDLAYER);
    setVisible(false);

    connect(ctrl, &VectorialMapController::gridPatternChanged, this, &vmap::GridController::computePattern);
    connect(ctrl, &VectorialMapController::gridColorChanged, this, &vmap::GridController::computePattern);
    connect(ctrl, &VectorialMapController::gridVisibilityChanged, this, &vmap::GridController::computePattern);
    connect(ctrl, &VectorialMapController::gridSizeChanged, this, &vmap::GridController::computePattern);
    connect(ctrl, &VectorialMapController::gridScaleChanged, this, &vmap::GridController::computePattern);
    connect(ctrl, &VectorialMapController::gridAboveChanged, this, &vmap::GridController::computePattern);
    connect(ctrl, &VectorialMapController::scaleUnitChanged, this, &vmap::GridController::computePattern);

    connect(m_ctrl, &VectorialMapController::visualRectChanged, this, &vmap::GridController::rectChanged);

    computePattern();

    connect(this, &vmap::GridController::rectChanged, this, [this] { setModified(); });
    connect(this, &vmap::GridController::gridPatternChanged, this, [this] { setModified(); });
}
bool GridController::gm() const
{
    return m_gm;
}

QImage GridController::gridPattern() const
{
    return m_gridPattern;
}

void GridController::aboutToBeRemoved()
{
    emit removeItem();
}

void GridController::setCorner(const QPointF& move, int corner) {}

void GridController::endGeometryChange() {}

void GridController::setGm(bool gm)
{
    if(m_gm == gm)
        return;

    m_gm= gm;
    emit gmChanged(m_gm);
}

void GridController::setGridPattern(QImage gridPattern)
{
    if(m_gridPattern == gridPattern)
        return;

    m_gridPattern= gridPattern;
    emit gridPatternChanged(m_gridPattern);
}

void GridController::setRect(QRectF rect)
{
    if(m_rect == rect)
        return;

    m_rect= rect;
    emit rectChanged(m_rect);
}

QRectF GridController::rect() const
{
    auto rect= m_ctrl->visualRect();
    rect= rect.united(m_rect);
    return rect;
}

void GridController::computePattern()
{
    if(!m_ctrl)
        return;

    if(m_ctrl->gridPattern() == Core::GridPattern::NONE || !m_ctrl->gridVisibility() || !m_ctrl->gridAbove())
        setVisible(false);
    else
        setVisible(true);

    QImage pattern;
    QPolygonF polygon;

    QColor background= m_ctrl->gridAbove() ? Qt::transparent : m_ctrl->backgroundColor();

    if(m_ctrl->gridPattern() == Core::GridPattern::HEXAGON)
    {
        qreal radius= m_ctrl->gridSize() / 2;
        qreal hlimit= radius * std::sin(M_PI / 3);
        qreal offset= radius - hlimit;
        QPointF A(2 * radius, radius - offset);
        QPointF B(radius * 1.5, radius - hlimit - offset);
        QPointF C(radius * 0.5, radius - hlimit - offset);
        QPointF D(0, radius - offset);
        QPointF E(radius * 0.5, radius + hlimit - offset - 1);
        QPointF F(radius * 1.5, radius + hlimit - offset - 1);

        QPointF G(2 * radius + radius, radius - offset);
        polygon << C << D << E << F << A << B << A << G;

        pattern
            = QImage(static_cast<int>(m_ctrl->gridSize() * 1.5), static_cast<int>(2 * hlimit), QImage::Format_ARGB32);
        pattern.fill(background);
    }
    else if(m_ctrl->gridPattern() == Core::GridPattern::SQUARE)
    {
        pattern= QImage(m_ctrl->gridSize(), m_ctrl->gridSize(), QImage::Format_ARGB32);
        pattern.fill(background);
        int sizeP= m_ctrl->gridSize();
        QPointF A(0, 0);
        QPointF B(0, sizeP - 1);
        QPointF C(sizeP - 1, sizeP - 1);

        polygon << A << B << C;
    }
    if(!pattern.isNull())
    {
        QPainter painter(&pattern);
        QPen pen;
        pen.setColor(m_ctrl->gridColor());
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawPolyline(polygon);
        painter.end();
    }
    setGridPattern(pattern);
}
} // namespace vmap
