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
#include "linecontroller.h"

#include <QVariant>

#include "controller/view_controller/vectorialmapcontroller.h"
namespace vmap
{
LineController::LineController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::LINE, params, ctrl, parent)
{
    m_tool= Core::SelectableTool::LINE;

    if(params.end() != params.find("color"))
        m_color= params.at(QStringLiteral("color")).value<QColor>();

    if(params.end() != params.find("penWidth"))
        m_penWidth= static_cast<quint16>(params.at(QStringLiteral("penWidth")).toInt());

    if(params.end() != params.find("position"))
        m_pos= params.at(QStringLiteral("position")).toPointF();

    connect(this, &LineController::startPointChanged, this, [this] { setModified(); });
    connect(this, &LineController::endPointChanged, this, [this] { setModified(); });
    connect(this, &LineController::penWidthChanged, this, [this] { setModified(); });
}

quint16 LineController::penWidth() const
{
    return m_penWidth;
}

QPointF LineController::endPoint() const
{
    return m_end;
}

QPointF LineController::startPoint() const
{
    return m_start;
}

void LineController::aboutToBeRemoved()
{
    emit removeItem();
}

void LineController::endGeometryChange()
{
    VisualItemController::endGeometryChange();

    if(m_editEndPoint)
    {
        emit endPointEditFinished();
        m_editEndPoint= false;
    }

    if(m_editStartPoint)
    {
        emit startPointEditFinished();
        m_editStartPoint= false;
    }
}

QRectF LineController::rect() const
{
    return QRectF(startPoint(), endPoint()).normalized();
}

void LineController::setCorner(const QPointF& move, int corner)
{
    switch(corner)
    {
    case Start:
        setStartPoint(m_start + move);
        break;
    case End:
        setEndPoint(m_end + move);
        break;
    }
}

void LineController::setStartPoint(const QPointF& p)
{
    if(p == m_start)
        return;
    m_start= p;
    emit startPointChanged();
    m_editStartPoint= true;
}

void LineController::setEndPoint(const QPointF& p)
{
    if(p == m_end)
        return;
    m_end= p;
    emit endPointChanged();
    m_editEndPoint= true;
}
} // namespace vmap
