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
#include "rectcontroller.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include <QDebug>
namespace vmap
{
RectController::RectController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(params, ctrl, parent)
{
    if(params.end() != params.find("tool"))
    {
        m_tool= params.at(QStringLiteral("tool")).value<Core::SelectableTool>();
        m_filled= (m_tool == Core::SelectableTool::FILLRECT);
    }
    else if(params.end() != params.find("filled"))
    {
        m_filled= params.at(QStringLiteral("filled")).toBool();
        m_tool= m_filled ? Core::SelectableTool::FILLRECT : Core::SelectableTool::EMPTYRECT;
    }

    if(params.end() != params.find("penWidth"))
        m_penWidth= static_cast<quint16>(params.at(QStringLiteral("penWidth")).toInt());

    if(params.end() != params.find("rect"))
        setRect(params.at(QStringLiteral("rect")).toRectF());
}

bool RectController::filled() const
{
    return m_filled;
}

QRectF RectController::rect() const
{
    return m_rect;
}

void RectController::setRect(QRectF rect)
{
    if(rect == m_rect)
        return;
    m_rect= rect;
    emit rectChanged();
    m_rectEdited= true;
}
VisualItemController::ItemType RectController::itemType() const
{
    return VisualItemController::RECT;
}
void RectController::setCorner(const QPointF& move, int corner)
{
    if(move.isNull())
        return;

    auto rect= m_rect;
    qreal x2= rect.right();
    qreal y2= rect.bottom();
    qreal x= rect.x();
    qreal y= rect.y();
    switch(corner)
    {
    case TopLeft:
        x+= move.x();
        y+= move.y();
        break;
    case TopRight:
        x2+= move.x();
        y+= move.y();
        break;
    case BottomRight:
        x2+= move.x();
        y2+= move.y();
        break;
    case BottomLeft:
        x+= move.x();
        y2+= move.y();
        break;
    }
    rect.setCoords(x, y, x2, y2);
    if(!rect.isValid())
        rect= rect.normalized();
    setRect(rect);
}

void RectController::aboutToBeRemoved()
{
    emit removeItem();
}

void RectController::endGeometryChange()
{
    VisualItemController::endGeometryChange();
    if(m_rectEdited)
    {
        emit rectEditFinished();
        m_rectEdited= false;
    }
}

quint16 RectController::penWidth() const
{
    return m_penWidth;
}
} // namespace vmap
