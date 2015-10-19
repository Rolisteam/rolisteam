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
#include <QMenu>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"
#include "data/character.h"
#include "userlist/playersList.h"
#include "vmap/items/sightitem.h"

CharacterItem::CharacterItem()
: VisualItem()
{
    createActions();
}

CharacterItem::CharacterItem(Character* m,QPointF pos,int diameter)
    : VisualItem(),m_character(m),m_center(pos),m_diameter(diameter),m_thumnails(NULL)
{
	setPos(m_center-QPoint(diameter/2,diameter/2));
	sizeChanged(diameter);
    /// @todo make it
    //connect(m_character,SIGNAL(avatarChanged()),this,SLOT(generatedThumbnail()));

     createActions();
}

void CharacterItem::writeData(QDataStream& out) const
{
    out << m_center;
    out << m_diameter;
    out << *m_thumnails;
    out << m_rect;
    out << m_showNpcName;
    out << m_showNpcNumber;
    out << m_showPcName;
    if(NULL!=m_character)
    {
        m_character->writeData(out);
    }
}

void CharacterItem::readData(QDataStream& in)
{
    in >> m_center;
    in >> m_diameter;
    m_thumnails = new QPixmap();
    in >> *m_thumnails;
    in >> m_rect;
    in >> m_showNpcName;
    in >> m_showNpcNumber;
    in >> m_showPcName;
    m_character = new Character();
    m_character->readData(in);

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
    setChildrenVisible(hasFocusOrChild());
    emit selectStateChange(hasFocusOrChild());

    if(m_character->hasAvatar())
    {
        painter->drawPixmap(m_rect,*m_thumnails,m_thumnails->rect());
    }
    else
    {
        painter->setRenderHint(QPainter::Antialiasing,true);
        painter->setRenderHint(QPainter::TextAntialiasing,true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform,true);
    //    painter->save();
        painter->setPen(m_character->getColor());
        painter->setBrush(QBrush(m_character->getColor(),Qt::SolidPattern));
        painter->drawEllipse(m_rect);
  //      painter->restore();



 //       painter->save();
        QPen pen = painter->pen();
        pen.setWidth(6);
		if(( NULL!= m_character )&&(NULL!=m_character->getState()))
		{
			pen.setColor(m_character->getState()->getColor());
		}

        painter->setPen(pen);
        painter->drawEllipse(m_rect.adjusted(3,3,-3,-3));
       // painter->restore();
    }

    QString toShow;
    //qDebug() << "isNpc" << m_character->isNpc() << "m_showNpcName " << m_showNpcName << "showNpcNumber" << m_showNpcNumber << "showPCName"<< m_showPcName << m_character->getName();
    if(m_character->isNpc())
    {
        if(m_showNpcName)
        {
            toShow = m_character->getName();
        }
        if(m_showNpcNumber)
        {
            toShow = m_character->getName();
        }
        if(m_showNpcName && m_showNpcNumber)
        {
            toShow = QString("%1 - %2").arg(m_character->getName()).arg(m_character->number());
        }
    }
    else if(m_showPcName)
    {
        toShow = m_character->getName();
    }
    QRectF rectText;
    QFontMetrics metric(painter->font());
	rectText.setRect(m_rect.center().x()-(metric.boundingRect(toShow).width()/2),m_rect.bottom(),metric.boundingRect(toShow).width(),metric.height());

	//qDebug() << rectText;
    //painter->save();
	painter->setPen(m_character->getColor());
    painter->drawText(rectText,Qt::AlignCenter,toShow);
    //painter->restore();


}
const QPointF& CharacterItem::getCenter() const
{
    return m_center;
}
void CharacterItem::sizeChanged(int m_size)
{
    m_diameter=m_size;
	m_rect.setRect(0,0,m_diameter,m_diameter);
	//m_rect.setRect(pos().x()-m_diameter/2,pos().y()-m_diameter/2,m_diameter,m_diameter);
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
    if(m_character->getAvatar().isNull())
    {
        painter.setPen(m_character->getColor());
        brush.setColor(m_character->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        QImage img =m_character->getAvatar();
        brush.setTextureImage(img.scaled(m_diameter,m_diameter));
    }
    
    painter.setBrush(brush);
    painter.drawRoundedRect(0,0,m_diameter,m_diameter,m_diameter/10,m_diameter/10);
}
int CharacterItem::getRadius() const
{
    return m_diameter/2;
}
void CharacterItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    msg->string16(m_character->getUuid());
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
    if((m_thumnails==NULL)||(m_thumnails->isNull()))
    {
        generatedThumbnail();
    }
    in << *m_thumnails;
    msg->byteArray32(data);

    m_character->fill(*msg);
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

    Character* tmp = PlayersList::instance()->getCharacter(idCharacter);
    if(NULL!=tmp)
    {
        m_character = tmp;
    }
    else
    {
        QString idparent = msg->string8();
        m_character = new Character(*msg);
        m_character->setParent(PlayersList::instance()->getPerson(idparent));
    }
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
QString CharacterItem::getCharacterId() const
{
    return m_character->getUuid();
}

QVariant CharacterItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemPositionHasChanged)
    {
        emit positionChanged();
    }
    return QGraphicsItem::itemChange(change, value);
}
int CharacterItem::getChildPointCount() const
{
    return m_child->size();
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
    case 4:
        m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    default:
       // emit geometryChangeOnUnkownChild(pointId,pos);
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
        m_child->value(4)->setPos(m_vision->getRadius(),m_rect.height()/2-m_child->value(4)->boundingRect().height()/2);
        m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    case 1:
        pos = m_rect.topRight();
         m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        m_child->value(4)->setPos(m_vision->getRadius(),m_rect.height()/2-m_child->value(4)->boundingRect().height()/2);
        m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    case 2:
        pos = m_rect.bottomRight();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        m_child->value(4)->setPos(m_vision->getRadius(),m_rect.height()/2-m_child->value(4)->boundingRect().height()/2);
        m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    case 3:
        pos = m_rect.bottomLeft();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(4)->setPos(m_vision->getRadius(),m_rect.height()/2-m_child->value(4)->boundingRect().height()/2);
        m_vision->setRadius(pos.x()-(getRadius()*2)+m_child->at(4)->boundingRect().width());
        break;
    case 4:
        break;
    default:
        break;
    }

	setTransformOriginPoint(m_rect.center());
}
void CharacterItem::initChildPointItem()
{
    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 5 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this,(i==4));
        tmp->setMotion(ChildPointItem::ALL);
        tmp->setRotationEnable(true);
        m_child->append(tmp);
    }

    m_child->at(4)->setMotion(ChildPointItem::X_AXIS);
    m_child->at(4)->setRotationEnable(false);
    m_child->at(4)->setVisible(false);


    updateChildPosition();


}
ChildPointItem* CharacterItem::getRadiusChildWidget()
{
    if(m_child->size()>=5)
    {
        return  m_child->value(4);
    }
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


    m_child->value(4)->setPos(m_vision->getRadius()+getRadius(),m_rect.height()/2-m_child->value(4)->boundingRect().height()/2);

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
/*  state->addAction(m_healthyStateAct);
  state->addAction(m_lightlyStateAct);
  state->addAction(m_seriouslyStateAct);
  state->addAction(m_deadStateAct);
  state->addAction(m_spleepingStateAct);
  state->addAction(m_bewitchedStateAct);*/


  QMenu* user =  menu->addMenu(tr("Affect to"));
  foreach(Character* character, PlayersList::instance()->getCharacterList())
  {
	QAction* act = user->addAction(character->getName());
    act->setData(character->getUuid());

	connect(act,SIGNAL(triggered()),this,SLOT(changeCharacter()));
  }
  QMenu* shape =  menu->addMenu(tr("Vision Shape"));
  shape->addAction(m_visionShapeDisk);
  shape->addAction(m_visionShapeAngle);

  if(CharacterVision::DISK == m_vision->getShape())
  {
	  m_visionShapeDisk->setChecked(true);
	  m_visionShapeAngle->setChecked(false);
  }
  else
  {
	 m_visionShapeDisk->setChecked(false);
     m_visionShapeAngle->setChecked(true);
  }

}
void CharacterItem::changeCharacter()
{
	QAction* act = qobject_cast<QAction*>(sender());
	QString uuid = act->data().toString();


	Character* tmp = PlayersList::instance()->getCharacter(uuid);
	if(NULL!=tmp)
	{
		m_character = tmp;
	}
}

void CharacterItem::createActions()
{
    m_vision = new CharacterVision(this);
  /*  m_healthyStateAct = new QAction(tr("Healthy"),this);
    m_lightlyStateAct= new QAction(tr("Lightly wounded"),this);
    m_seriouslyStateAct= new QAction(tr("Seriously injured"),this);
    m_deadStateAct= new QAction(tr("Dead"),this);
    m_spleepingStateAct= new QAction(tr("Sleeping"),this);
    m_bewitchedStateAct= new QAction(tr("Bewitched"),this);*/




	m_visionShapeDisk =  new QAction(tr("Disk"),this);
	m_visionShapeDisk->setCheckable(true);
	m_visionShapeAngle = new QAction(tr("Conical"),this);
	m_visionShapeAngle->setCheckable(true);

   /* connect(m_healthyStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_lightlyStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_seriouslyStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_deadStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_spleepingStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));
    connect(m_bewitchedStateAct,SIGNAL(triggered()),this,SLOT(characterStateChange()));*/

	connect(m_visionShapeAngle,SIGNAL(triggered()),this,SLOT(changeVisionShape()));
	connect(m_visionShapeDisk,SIGNAL(triggered()),this,SLOT(changeVisionShape()));
}
void CharacterItem::changeVisionShape()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(act == m_visionShapeDisk)
    {
        m_visionShapeAngle->setChecked(false);
        m_visionShapeDisk->setChecked(true);
        m_vision->setShape(CharacterVision::DISK);
    }
    else if(act == m_visionShapeAngle)
    {
        m_visionShapeAngle->setChecked(true);
        m_visionShapeDisk->setChecked(false);
        m_vision->setShape(CharacterVision::ANGLE);
    }
}

void CharacterItem::characterStateChange()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(NULL == m_character)
        return;




    CharacterState* state = m_stateActList->value(act);

    m_character->setState(state);



/*    if(act == m_healthyStateAct)
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
    else if (act == m_spleepingStateAct)
    {
        m_character->setHeathState(Character::Sleeping);
    }
    else if (act == m_bewitchedStateAct)
    {
        m_character->setHeathState(Character::Bewitched);
    }*/
}
VisualItem* CharacterItem::getItemCopy()
{
	CharacterItem* charactItem = new CharacterItem(m_character,pos(),m_diameter);
	charactItem->setPos(pos());
	return charactItem;
}

QString CharacterItem::getParentId() const
{
    Person* pers = m_character->parent();
    if(NULL!=pers)
    {
        return pers->getUuid();
    }
    return QString();
}

void CharacterItem::addChildPoint(ChildPointItem* item)
{
    if(NULL!=m_child)
    {
        //item->setPointID(m_child->size());
        m_child->append(item);
    }
}
void CharacterItem::setDefaultVisionParameter(CharacterVision::SHAPE shape, qreal radius, qreal angle)
{
    m_vision->setAngle(angle);
    m_vision->setRadius(radius);
    m_vision->setShape(shape);
}
CharacterVision* CharacterItem::getVision()const
{
    return m_vision;
}
