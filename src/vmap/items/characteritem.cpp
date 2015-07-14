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
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"


CharacterItem::CharacterItem()
: VisualItem()
{

}

CharacterItem::CharacterItem(const Character* m,QPointF pos,int diameter)
    : VisualItem(),m_character(m),m_center(pos),m_diameter(diameter),m_thumnails(NULL)
{
    m_rect.setRect(m_center.x()-m_diameter/2,m_center.y()-m_diameter/2,m_diameter,m_diameter);
    /// @todo make it
    //connect(m_character,SIGNAL(avatarChanged()),this,SLOT(generatedThumbnail()));
}

/*CharacterItem::CharacterItem(QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent)
    {
    
    }*/


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
    return m_rect;
}
void CharacterItem::setNewEnd(QPointF& nend)
{
    m_center = nend;
}
void CharacterItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    if(m_thumnails==NULL)
    {
        generatedThumbnail();
    }
    if(option->state & QStyle::State_Selected)
    {
        painter->drawRect(m_thumnails->rect());
    }
    painter->drawPixmap(m_rect,*m_thumnails,m_thumnails->rect());
}
void CharacterItem::sizeChanged(int m_size)
{
    m_diameter=m_size;
    m_rect.setRect(m_center.x()-m_diameter/2,m_center.y()-m_diameter/2,m_diameter,m_diameter);
    generatedThumbnail();
}
void CharacterItem::generatedThumbnail()
{
    if(m_thumnails!=NULL)
    {
        delete m_thumnails;
        m_thumnails = NULL;
    }
    m_thumnails=new QPixmap(m_diameter,m_diameter);
    m_thumnails->fill(Qt::transparent);
    QPainter painter(m_thumnails);
    QBrush brush;
    /*if(m_character->getAvatar().isNull())
    {
        painter.setPen(m_character->getColor());
        brush.setColor(m_character->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        QImage img =m_character->getAvatar();
        brush.setTextureImage(img.scaled(m_diameter,m_diameter));
    }*/
    
    painter.setBrush(brush);
    painter.drawRoundedRect(0,0,m_diameter,m_diameter,m_diameter/10,m_diameter/10);
}
void CharacterItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->string16(m_character->uuid());
    msg->uint16(m_diameter);
    //pos
    msg->real(m_center.x());
    msg->real(m_center.y());

    //rect
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());

    //path
    QByteArray data;
    QDataStream in(&data,QIODevice::WriteOnly);
    in << *m_thumnails;
    msg->byteArray32(data);
}
void CharacterItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    QString idCharacter = msg->string16();
    m_diameter = msg->uint16();
//pos
    m_center.setX(msg->real());
    m_center.setY(msg->real());
 //rect
    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());

    //path
    QByteArray data;
    data = msg->byteArray32();

    QDataStream out(&data,QIODevice::ReadOnly);
    m_thumnails = new QPixmap();
    out >> *m_thumnails;

}
