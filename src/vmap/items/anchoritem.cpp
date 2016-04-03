/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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

#include "anchoritem.h"


AnchorItem::AnchorItem()
    : m_pen(QColor(Qt::darkGray))
{
}
AnchorItem::AnchorItem(QPointF& p)
    :  VisualItem()
{
    m_startPoint = p;
    m_endPoint = m_startPoint;
    m_rect.setTopLeft(p);
    m_rect.setBottomRight(m_endPoint);

}

AnchorItem::~AnchorItem()
{

}
QRectF AnchorItem::boundingRect() const
{
    return  m_rect;
}
VisualItem::ItemType AnchorItem::getType()
{
    return VisualItem::ANCHOR;
}
void AnchorItem::setNewEnd(QPointF& nend)
{
    m_endPoint = nend;
    m_rect.setBottomRight(nend);
}
void AnchorItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);

    m_pen.setWidth(3);
    m_pen.setColor(Qt::darkGray);
    painter->setPen(m_pen);
    QLineF line(m_startPoint,m_endPoint);
    painter->drawLine(line);
    painter->restore();

   // QPointF middle = line.pointAt(0.5);

    QFont f = painter->font();
    f.setPixelSize(15);
    painter->setFont(f);
}

void AnchorItem::writeData(QDataStream& out) const
{

}

void AnchorItem::readData(QDataStream& in)
{

}
void AnchorItem::fillMessage(NetworkMessageWriter* msg)
{

}
void AnchorItem::readItem(NetworkMessageReader* msg)
{

}
void AnchorItem::setGeometryPoint(qreal pointId, QPointF &pos)
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
void AnchorItem::initChildPointItem()
{

}

VisualItem* AnchorItem::getItemCopy()
{
	return NULL;
}
const QPointF& AnchorItem::getStart() const
{
    return m_startPoint;
}

const  QPointF& AnchorItem::getEnd() const
{
    return m_endPoint;
}
