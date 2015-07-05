/***************************************************************************
    *      Copyright (C) 2010 by Renaud Guezennec                             *
    *                                                                         *
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
#include "rectitem.h"
#include <QStyle>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QStylePainter>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

RectItem::RectItem()
    : VisualItem()
{
    
}

RectItem::RectItem(QPointF& topleft,QPointF& buttomright,bool filled,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent)
{
    
    m_rect.setBottomRight(buttomright);
    m_rect.setTopLeft(topleft);
    m_filled= filled;
}

QRectF RectItem::boundingRect() const
{
    return m_rect;
}
void RectItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    if(!m_filled)
    {
        painter->setPen(m_color);
        painter->drawRect(m_rect);
    }
    else
    {
        painter->setBrush(QBrush(m_color));
        painter->fillRect(m_rect, m_color);

    }
    if(option->state & QStyle::State_Selected)
    {
        painter->save();
        painter->setBrush(Qt::NoBrush);
        QRectF tmp_rect=m_rect.adjusted(5,5,-5,-5);
        QPen pen=painter->pen();
        pen.setWidth(2);
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::white);
        painter->setPen(pen);
        painter->drawRect(tmp_rect);
        pen.setColor(Qt::black);
        QVector<qreal> dashes;
        dashes << tmp_rect.width()/20 << tmp_rect.width()/20;
        pen.setDashPattern(dashes);

        painter->setPen(pen);
        painter->drawRect(tmp_rect);
        painter->restore();
        /*if(widget!=NULL)
    {
    QStyleOptionFocusRect option;
    option.initFrom(widget);
    option.backgroundColor =m_color;
    //         option.rect=m_rect.toRect();    //option.backgroundColor = palette().color(m_color);
    
    //QStylePainter painterstyle(widget);
    widget->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, painter, widget);
    }*/
    }
    painter->restore();
    
}
void RectItem::setNewEnd(QPointF& p)
{
    m_rect.setBottomRight(p);
}

VisualItem::ItemType RectItem::getType()
{
    return VisualItem::RECT;
}
void RectItem::writeData(QDataStream& out) const
{
    out << m_rect;
    out << m_filled;
    out << m_color;
}

void RectItem::readData(QDataStream& in)
{
    in >> m_rect;
    in >> m_filled;
    in >> m_color;
}
void RectItem::fillMessage(NetworkMessageWriter* msg)
{
    //rect
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());

    msg->int8(m_filled);
    msg->rgb(m_color);
}
void RectItem::readItem(NetworkMessageReader* msg)
{
    //rect
    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());

    m_filled = msg->int8();
    m_color = msg->rgb();

}
