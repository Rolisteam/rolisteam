/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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


RuleItem::RuleItem()
    : m_pen(QColor(Qt::red))
{
}
RuleItem::RuleItem(QPointF& p)
    :  VisualItem()
{
    m_startPoint = p;
    m_endPoint = m_startPoint;
    m_rect.setTopLeft(p);
    m_rect.setBottomRight(m_endPoint);

}

RuleItem::~RuleItem()
{

}
QRectF RuleItem::boundingRect() const
{
    return  m_rect;
}
VisualItem::ItemType RuleItem::getType()
{
    return VisualItem::RULE;
}
void RuleItem::setNewEnd(QPointF& nend)
{
    if(m_mod & Qt::ControlModifier)
    {
        QLineF line(m_startPoint,nend);
        if(abs(line.dx()) > abs(line.dy()))
        {
            nend.setY(m_startPoint.y());
        }
        else
        {
            nend.setX(m_startPoint.x());
        }
    }
    m_endPoint = nend;
    m_rect.setBottomRight(nend);
}
void RuleItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);

    m_pen.setWidth(3);
    m_pen.setColor(Qt::red);
    painter->setPen(m_pen);
    QLineF line(m_startPoint,m_endPoint);
    qDebug() << line.length() << m_pixelToUnit;

    painter->drawLine(line);
    painter->restore();

   // QPointF middle = line.pointAt(0.5);

    qreal len = line.length();
    qreal unitCount = len/m_pixelToUnit;

    QFont f = painter->font();
    f.setPixelSize(15);
    painter->setFont(f);

    painter->drawText(m_endPoint,QString("%1 %2").arg(QString::number(unitCount,'f',2)).arg(m_unitText));

}
void RuleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << event->modifiers() << event->pos();


    VisualItem::mouseMoveEvent(event);
}
void RuleItem::setModifiers(Qt::KeyboardModifiers mod)
{
    m_mod = mod;
}

void RuleItem::writeData(QDataStream& out) const
{

}

void RuleItem::readData(QDataStream& in)
{

}
void RuleItem::fillMessage(NetworkMessageWriter* msg)
{

}
void RuleItem::readItem(NetworkMessageReader* msg)
{

}
void RuleItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    if(pointId == 0)
    {
        m_startPoint = pos;
        m_rect.setTopLeft(m_startPoint);
    }
    else if(pointId == 1)
    {
        m_endPoint = pos;
        m_rect.setBottomRight(m_endPoint);
    }
}
void RuleItem::initChildPointItem()
{

}
void RuleItem::setUnit(VMap::SCALE_UNIT unit)
{
    switch(unit)
    {
    case VMap::M:
        m_unitText = tr("m");
                break;
    case VMap::CM:
        m_unitText = tr("cm");
                break;
    case VMap::INCH:
        m_unitText = tr("″");
                break;
    case VMap::FEET:
        m_unitText = tr("′");
                break;
    case VMap::YARD:
        m_unitText = tr("yd");
                break;
    case VMap::MILE:
        m_unitText = tr("mi");
                break;
    case VMap::KM:
        m_unitText = tr("km");
                break;
    case VMap::PX:
        m_unitText = tr("px");
        break;
    }
}
void RuleItem::setPixelToUnit(qreal pixels)
{
    m_pixelToUnit = pixels;
}
