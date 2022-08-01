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
#include "controller/item_controllers/ellipsecontroller.h"

#include <QVariant>

namespace vmap
{
EllipseController::EllipseController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                                     QObject* parent)
    : VisualItemController(VisualItemController::ELLIPSE, params, ctrl, parent)
{
    namespace cv= Core::vmapkeys;
    if(params.end() != params.find(cv::KEY_TOOL))
    {
        m_tool= params.at(cv::KEY_TOOL).value<Core::SelectableTool>();
        m_filled= (m_tool == Core::SelectableTool::FILLEDELLIPSE);
    }

    if(params.end() != params.find(cv::KEY_FILLED))
    {
        m_filled= params.at(cv::KEY_FILLED).toBool();
        m_tool= m_filled ? Core::SelectableTool::FILLEDELLIPSE : Core::SelectableTool::EMPTYELLIPSE;
    }

    if(params.end() != params.find(cv::KEY_PENWIDTH))
        m_penWidth= static_cast<quint16>(params.at(cv::KEY_PENWIDTH).toInt());

    if(params.end() != params.find(cv::KEY_RX))
        m_rx= params.at(cv::KEY_RX).toReal();

    if(params.end() != params.find(cv::KEY_RY))
        m_ry= params.at(cv::KEY_RY).toReal();

    connect(this, &EllipseController::filledChanged, this, [this] { setModified(); });
    connect(this, &EllipseController::penWidthChanged, this, [this] { setModified(); });
    connect(this, &EllipseController::ryChanged, this, [this] { setModified(); });
    connect(this, &EllipseController::rxChanged, this, [this] { setModified(); });
}

bool EllipseController::filled() const
{
    return m_filled;
}

quint16 EllipseController::penWidth() const
{
    return m_penWidth;
}

qreal EllipseController::rx() const
{
    return m_rx;
}

qreal EllipseController::ry() const
{
    return m_ry;
}

void EllipseController::aboutToBeRemoved()
{
    emit removeItem();
}

void EllipseController::endGeometryChange()
{
    VisualItemController::endGeometryChange();
    if(m_editingRx)
    {
        emit rxEditionChanged();
        m_editingRx= false;
    }

    if(m_editingRy)
    {
        emit ryEditionChanged();
        m_editingRy= false;
    }
}

QRectF EllipseController::rect() const
{
    return QRectF(-rx(), -ry(), rx() * 2, ry() * 2);
}

void EllipseController::setCorner(const QPointF& move, int corner,
                                  Core::TransformType tt)
{
    if(move.isNull())
        return;

    auto rx= m_rx;
    auto ry= m_ry;
    switch(corner)
    {
    case 0:
        rx+= move.x();
        break;
    case 1:
        ry+= move.y();
        break;
    case 2:
        rx+= move.x() / 2;
        ry+= move.y() / 2;
        break;
    }
    setRx(rx);
    setRy(ry);
}

void EllipseController::setRx(qreal rx)
{
    if(qFuzzyCompare(rx, m_rx) || qFuzzyIsNull(rx))
        return;
    m_rx= rx;
    emit rxChanged();
    m_editingRx= true;
}

void EllipseController::setRy(qreal ry)
{
    if(qFuzzyCompare(ry, m_ry) || qFuzzyIsNull(ry))
        return;
    m_ry= ry;
    emit ryChanged();
    m_editingRy= true;
}
} // namespace vmap
