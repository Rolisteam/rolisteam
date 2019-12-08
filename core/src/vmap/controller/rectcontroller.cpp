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

RectController::RectController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(ctrl, parent)
{
    m_color= params.at(QStringLiteral("color")).value<QColor>();
    m_filled= (params.at(QStringLiteral("tool")).value<Core::SelectableTool>() == Core::SelectableTool::FILLRECT);
    m_penWidth= static_cast<quint16>(params.at(QStringLiteral("penWidth")).toInt());
    m_pos= params.at(QStringLiteral("position")).toPointF();
    // m_rect.setTopLeft(pos);
    // m_rect.setBottomRight(pos);
}

bool RectController::filled() const
{
    return m_filled;
}

QColor RectController::color() const
{
    return m_color;
}

QRectF RectController::rect() const
{
    return m_rect;
}

void RectController::setColor(QColor color)
{
    if(color == m_color)
        return;
    m_color= color;
    emit colorChanged();
}

void RectController::setRect(QRectF rect)
{
    if(rect == m_rect)
        return;
    m_rect= rect;
    emit rectChanged();
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
    /* auto rect= m_rect;
     auto pos= m_pos;
     qreal w= rect.width();
     qreal h= rect.height();
     rect.setCoords(-w / 2, -h / 2, w, h);
     pos+= QPointF(w / 2, h / 2);
     setRect(rect);
     setPos(pos);*/
}

quint16 RectController::penWidth() const
{
    return m_penWidth;
}
