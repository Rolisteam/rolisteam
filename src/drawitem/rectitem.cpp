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

#include <QPainter>
RectItem::RectItem(QPointF& topleft,QPointF& buttomright,bool filled,QGraphicsItem * parent)
    : VisualItem(parent)
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

    if(!m_filled)
    {
        painter->drawRect(m_rect);
    }
    else
    {
        painter->fillRect(m_rect, m_color);

    }


}
void RectItem::setNewEnd(QPointF& p)
{
    QRectF tmp= m_rect;
    m_rect.setBottomRight(p);
    update(tmp);
}
