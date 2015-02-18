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
#ifndef VISUALITEM_H
#define VISUALITEM_H

#include <QGraphicsItem>
/**
  * @brief abstract class which defines interface for all map items.
  * @todo Allows the modification of item's geometry, enable the selection.
  */
class VisualItem : public QGraphicsItem
{
public:
    enum ItemType{PATH,LINE,ELLISPE,CHARACTER,TEXT,RECT};
    VisualItem();
    VisualItem(QColor& penColor,QGraphicsItem * parent = 0);


    virtual void setNewEnd(QPointF& nend)=0;
    virtual void setPenColor(QColor& penColor);

    virtual void writeData(QDataStream& out) const =0;
    virtual void readData(QDataStream& in)=0;

    friend QDataStream& operator<<(QDataStream& os,const VisualItem&);
    friend QDataStream& operator>>(QDataStream& is,VisualItem&);

    virtual VisualItem::ItemType getType()=0;

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );


    QColor m_color;
    ItemType m_type;

};

#endif // VISUALITEM_H
