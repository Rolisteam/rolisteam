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
#ifndef LINEITEM_H
#define LINEITEM_H

#include "visualitem.h"
#include <QPen>
class LineItem : public VisualItem
{
public:
    LineItem(QPointF& p,QColor& penColor,int penSize,QGraphicsItem * parent = 0);

    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    virtual QRectF boundingRect() const ;


    virtual void setNewEnd(QPointF& nend);

private:
    QRectF m_rect;
    QPointF m_startPoint;
    QPointF m_endPoint;

    QPen m_pen;
    bool m_filled;
};

#endif // LINEITEM_H
