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

#include <QMenu>

CharacterItem::CharacterItem()
: VisualItem()
{
    createActions();
}

CharacterItem::CharacterItem(Character* m,QPointF pos,int diameter)
    : VisualItem(),m_character(m),m_center(pos),m_diameter(diameter),m_thumnails(NULL)
{
    m_rect.setRect(m_center.x()-m_diameter/2,m_center.y()-m_diameter/2,m_diameter,m_diameter);
    /// @todo make it
    //connect(m_character,SIGNAL(avatarChanged()),this,SLOT(generatedThumbnail()));

     createActions();
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
    painter->save();
    painter->setPen(m_character->color());
    painter->setBrush(QBrush(m_character->color(),Qt::SolidPattern));
    painter->drawEllipse(m_rect);
    painter->restore();



    painter->save();
    QPen pen = painter->pen();
    pen.setWidth(6);
    switch(m_character->getHeathState())
    {
    case Character::Healthy:
        pen.setColor(Qt::black);
    break;
    case Character::Lightly:
        pen.setColor(QColor(255, 100, 100));
    break;
    case Character::Seriously:
        pen.setColor(QColor(255, 0, 0));
    break;
    case Character::Dead:
        pen.setColor(Qt::gray);
    break;
    case Character::Sleeping:
        pen.setColor(QColor(80, 80, 255));
    break;
    case Character::Bewitched:
        pen.setColor(QColor(0, 200, 0));
    break;
    }
    painter->setPen(pen);
    painter->drawEllipse(m_rect.adjusted(3,3,-3,-3));
    painter->restore();

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

    painter->save();
    painter->setPen(m_character->color());
    painter->drawText(rectText,Qt::AlignCenter,toShow);
    painter->restore();

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
void CharacterItem::addActionContextMenu(QMenu* menu)
{
  QMenu* state =  menu->addMenu(tr("Change State"));
  state->addAction(m_healthyStateAct);
  state->addAction(m_lightlyStateAct);
  state->addAction(m_seriouslyStateAct);
  state->addAction(m_deadStateAct);
  state->addAction(m_spleepingStateAct);
  state->addAction(m_bewitchedStateAct);

}
void CharacterItem::createActions()
{
    m_healthyStateAct = new QAction(tr("Healthy"),this);
    m_lightlyStateAct= new QAction(tr("Lightly wounded"),this);
    m_seriouslyStateAct= new QAction(tr("Seriously injured"),this);
    m_deadStateAct= new QAction(tr("Dead"),this);
    m_spleepingStateAct= new QAction(tr("Sleeping"),this);
    m_bewitchedStateAct= new QAction(tr("Bewitched"),this);


    connect(m_healthyStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_lightlyStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_seriouslyStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_deadStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_spleepingStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_bewitchedStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));

}
void CharacterItem::characterStateChange()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(NULL == m_character)
        return;


    if(act == m_healthyStateAct)
    {
        m_character->setHeathState(Character::Healthy);
    }
    else if(act == m_lightlyStateAct)
    {
        m_character->setHeathState(Character::Lightly);
    }
    else if (act == m_seriouslyStateAct)
    {
        m_character->setHeathState(Character::Seriously);
    }
    else if (act == m_deadStateAct)
    {
       m_character->setHeathState(Character::Dead);
    }
    else if ( act == m_spleepingStateAct)
    {
        m_character->setHeathState(Character::Sleeping);
    }
    else if (act == m_bewitchedStateAct)
    {
        m_character->setHeathState(Character::Bewitched);
    }
}
