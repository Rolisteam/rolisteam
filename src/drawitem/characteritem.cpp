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
#include "characteritem.h"
#include <QPainter>


#include <QDebug>
CharacterItem::CharacterItem(const Character* m,QPointF pos,quint32 diameter)
    : VisualItem(),m_character(m),m_center(pos),m_diameter(diameter)
{

}

CharacterItem::CharacterItem(QColor& penColor,QGraphicsItem * parent)
            : VisualItem(penColor,parent)
{

}


void CharacterItem::writeData(QDataStream& out) const
{

}

void CharacterItem::readData(QDataStream& in)
{

}
VisualItem::ItemType CharacterItem::getType()
{
    return VisualItem::CHARACTER;
}
QRectF CharacterItem::boundingRect() const
{
    /**
      * @todo must be changed and managed by preference system
      */
    return QRectF((m_center.x()-m_diameter/2),m_center.y()-m_diameter/2,m_diameter,m_diameter);
}
void CharacterItem::setNewEnd(QPointF& nend)
{
    m_center = nend;
}
void CharacterItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    qDebug() << "paint du character" << m_center ;
    /// @todo implemented it, get the image and display it
    QBrush brush;
    if(m_character->hasAvatar())
    {
        //painter->drawImage(m_center, );
        brush.setTextureImage(m_character->getAvatar());
    }
    else
    {
        painter->setPen(m_character->getColor());
        brush.setColor(m_character->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    painter->setBrush(brush);
    painter->drawRoundedRect(m_center.x()-m_diameter/2,m_center.y()-m_diameter/2,m_diameter,m_diameter,m_diameter/10,m_diameter/10);
}
