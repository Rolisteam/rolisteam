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
#include "visualitem.h"
#include <QGraphicsSceneHoverEvent>
#include <math.h>
#include <QCursor>
#include <QDebug>

VisualItem::VisualItem()
: QGraphicsItem()
{

}

VisualItem::VisualItem(QColor& penColor,QGraphicsItem * parent )
    : QGraphicsItem(parent),m_color(penColor)
{
    //m_state = IDLE;
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
}

void VisualItem::setPenColor(QColor& penColor)
{
    m_color = penColor;
}
void VisualItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mouseMoveEvent(event);   
}
void VisualItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
void VisualItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

QDataStream& operator<<(QDataStream& os,const VisualItem& c)
{

  c.writeData(os);

  return os;
}
QDataStream& operator>>(QDataStream& is,VisualItem& c)
{
  c.readData(is);
  return is;
}

