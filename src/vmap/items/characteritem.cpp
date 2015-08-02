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
#include "data/character.h"
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
    //m_center = nend;
}
void CharacterItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    if(m_thumnails==NULL)
    {
        generatedThumbnail();
    }
    if(hasFocusOrChild())
    {
        foreach(ChildPointItem* item, *m_child)
        {
            item->setVisible(true);
        }
    }
    else
    {
        if(NULL!=m_child)
        {
            foreach(ChildPointItem* item, *m_child)
            {
                item->setVisible(false);
            }
        }
    }
    painter->setPen(m_character->color());
    painter->drawRect(m_rect);
    painter->drawEllipse(m_rect);
    QString toShow;
    if(m_character->isNpc())
    {
        if(m_showNpcName)
        {
            toShow = m_character->name();
        }
        if(m_showNpcNumber)
        {
            toShow = m_character->name();
        }
        if(m_showNpcName && m_showNpcNumber)
        {
            toShow = QString("%1 - %2").arg(m_character->name()).arg(m_character->number());
        }
    }
    else if(m_showPcName)
    {
        toShow = m_character->name();
    }
    QRectF rectText;
    QFontMetrics metric(painter->font());
    rectText.setRect(m_rect.left(),m_rect.bottom(),m_rect.width(),metric.height());

    painter->drawText(rectText,Qt::AlignCenter,toShow);

    //painter->drawPixmap(m_rect,*m_thumnails,m_thumnails->rect());
}
void CharacterItem::sizeChanged(int m_size)
{
    m_diameter=m_size;
    m_rect.setRect(m_center.x()-m_diameter/2,m_center.y()-m_diameter/2,m_diameter,m_diameter);
    //generatedThumbnail();
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
    msg->real(scale());
    msg->real(rotation());
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
void CharacterItem::resizeContents(const QRect& rect, bool )
{
    if (!rect.isValid())
        return;

    prepareGeometryChange();
    m_rect = rect;
    m_diameter = qMin(m_rect.width(),m_rect.height());
    sizeChanged(m_diameter);
    updateChildPosition();
}
void CharacterItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
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
QVariant CharacterItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
   /* if (change == ItemPositionChange && scene() && hasFocus())
    {
        QPointF newPos = value.toPointF();
        m_center = newPos;
        sizeChanged(m_diameter);
    }*/
    return QGraphicsItem::itemChange(change, value);
}
void CharacterItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    QRectF rect=m_rect;
    switch ((int)pointId)
    {
    case 0:
        rect.setTopLeft(pos);
        break;
    case 1:
        rect.setTopRight(pos);
        break;
    case 2:
        rect.setBottomRight(pos);
        break;
    case 3:
        rect.setBottomLeft(pos);
        break;
    default:
        break;
    }
    m_diameter = qMin(rect.width(),rect.height());
    sizeChanged(m_diameter);
    switch ((int)pointId)
    {
    case 0:
        pos = m_rect.topLeft();
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 1:
        pos = m_rect.topRight();
         m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 2:
        pos = m_rect.bottomRight();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 3:
        pos = m_rect.bottomLeft();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        break;
    default:
        break;
    }
    setTransformOriginPoint(m_center);
}
void CharacterItem::initChildPointItem()
{
    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 4 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this);
        tmp->setMotion(ChildPointItem::ALL);
        tmp->setRotationEnable(true);
        m_child->append(tmp);
    }
    updateChildPosition();
}
void CharacterItem::updateChildPosition()
{
    m_child->value(0)->setPos(m_rect.topLeft());
    m_child->value(0)->setPlacement(ChildPointItem::TopLeft);
    m_child->value(1)->setPos(m_rect.topRight());
    m_child->value(1)->setPlacement(ChildPointItem::TopRight);
    m_child->value(2)->setPos(m_rect.bottomRight());
    m_child->value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_child->value(3)->setPos(m_rect.bottomLeft());
    m_child->value(3)->setPlacement(ChildPointItem::ButtomLeft);

    setTransformOriginPoint(m_rect.center());

    update();
}
void CharacterItem::showNpcName(bool b)
{
    m_showNpcName = b;
}

void CharacterItem::showNpcNumber(bool b)
{
    m_showNpcNumber = b;
}

void CharacterItem::showPcName(bool b)
{
    m_showPcName = b;
}
