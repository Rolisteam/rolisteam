/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include "ruleitem.h"

#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include <QPainter>
#include <cmath>

#define FONT_SIZE 15

QString unitToText(Core::ScaleUnit unit)
{
    QString result;
    switch(unit)
    {
    case Core::M:
        result= QStringLiteral("m");
        break;
    case Core::CM:
        result= QStringLiteral("cm");
        break;
    case Core::INCH:
        result= QStringLiteral("″");
        break;
    case Core::FEET:
        result= QStringLiteral("′");
        break;
    case Core::YARD:
        result= QStringLiteral("yd");
        break;
    case Core::MILE:
        result= QStringLiteral("mi");
        break;
    case Core::KM:
        result= QStringLiteral("km");
        break;
    case Core::PX:
        result= QStringLiteral("px");
        break;
    }
    return result;
}

RuleItem::RuleItem(VectorialMapController* ctrl) : QGraphicsObject(), m_ctrl(ctrl) {}

RuleItem::~RuleItem() {}
QRectF RuleItem::boundingRect() const
{
    return QRectF(m_startPoint, m_endPoint);
}

void RuleItem::setNewEnd(const QPointF& nendConst, bool onAxis)
{
    if(nendConst.isNull())
        return;
    if(!onAxis)
    {
        m_endPoint+= nendConst;
        return;
    }

    QLineF line(m_startPoint, nendConst);
    if(std::fabs(line.dx()) > std::fabs(line.dy()))
    {
        m_endPoint= QPointF(m_endPoint.x() + nendConst.x(), 0);
    }
    else
    {
        m_endPoint= QPointF(0, m_endPoint.y() + nendConst.y());
    }
}
void RuleItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    m_pen.setWidth(m_ctrl->penSize() * 1 / m_ctrl->zoomLevel());
    m_pen.setColor(Qt::red);
    painter->setPen(m_pen);
    QLineF line(m_startPoint, m_endPoint);

    painter->drawLine(line);
    painter->restore();

    qreal len= line.length();

    qreal unitCount= len / m_ctrl->gridSize() * m_ctrl->gridScale();

    QFont f= painter->font();
    f.setPixelSize(FONT_SIZE * 1 / m_ctrl->zoomLevel());
    painter->setFont(f);

    auto text= QString("%1 %2").arg(QString::number(unitCount, 'f', 2), unitToText(m_ctrl->scaleUnit()));

    QFontMetricsF metrics(f);
    auto rect= metrics.boundingRect(text);
    painter->save();
    painter->fillRect(rect.translated(m_endPoint), QBrush("#88FFFFFF"));
    painter->restore();
    painter->drawText(m_endPoint, text);
}
