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
#include "ellipsitem.h"
#include <QPainter>
#include <QDebug>
#include "math.h"
EllipsItem::EllipsItem(QPointF& center,bool filled,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent)
{
    m_center = center;
    m_rect.setBottomLeft(m_center);
    m_rect.setTopLeft(m_center);
    m_filled = filled;
}
QRectF EllipsItem::boundingRect() const
{
    return m_rect;
}
void EllipsItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    if(!m_filled)
    {
        painter->setPen(m_color);
        painter->drawEllipse(m_rect);
    }
    else
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(m_color,Qt::SolidPattern));
        painter->drawEllipse(m_rect);

    }
    painter->restore();

}
void EllipsItem::setNewEnd(QPointF& p)
{
    //QRectF tmp= m_rect;
    float dx = p.x()-m_center.x();
    float dy = p.y()-m_center.y();

    //qDebug() << "dy = "<< dy << "dx = " << dx;
    m_rect.setBottomRight(p);
    m_rect.setTopLeft(QPointF(m_center.x()-dx,m_center.y()-dy));



   /*

    float hypo = sqrt((dx*dx)+(dy*dy));
    float angle = acos(dy/hypo);
    float newdx = abs(dx/angle);
   float newdy = abs(dy/(3.14159265358979323846/2)-angle);
   qDebug() << acos(dx/hypo)*(180/3.14159265358979323846) << newdx;
   qDebug() << ((3.14159265358979323846/2)-angle)*(180/3.14159265358979323846)<< newdy;

   if(newdy<dy)
       newdy=dy;
   if(newdx<dx)
       newdx=dx;
    qDebug()<< newdx << " " << dx;
    qDebug()<< newdy << " " << dy;

    QPointF point(m_center.x()+newdx,m_center.y()+newdy);
//    dx = point.x()-m_center.x();
  //  dy = point.y()-m_center.y();
    m_rect.setBottomRight(point);
    //qDebug() << "dy = "<< dy << "dx = " << dx;
    m_rect.setTopLeft(QPointF(m_center.x()-newdx,m_center.y()-newdy));


    qDebug() << m_rect << point << p;*/


    //m_rect.set(dy);
    //update(tmp);
}
void EllipsItem::writeData(QDataStream& out) const
{

}

void EllipsItem::readData(QDataStream& in)
{

}
