/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "sightitem.h"


#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QMenu>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>

/////////////////////////////////
/// Code Vision
/////////////////////////////////
Vision::Vision()
{

}

void Vision::setAngle(qreal a)
{
    m_angle = a;
}

void Vision::setRadius(qreal r)
{
    m_radius = r;
}

void Vision::setPosition(QPointF& p)
{
    m_pos = p;
}

void Vision::setShape(Vision::SHAPE s)
{
    m_shape = s;
}


qreal Vision::getAngle()
{
    return m_angle;
}

qreal Vision::getRadius()
{
    return m_radius;
}

const QPointF& Vision::getPos()
{
    return m_character->pos();
}

Vision::SHAPE Vision::getShape()
{
    return m_shape;
}

void Vision::setCharacterItem(CharacterItem* item)
{
    m_character = item;
}
CharacterItem* Vision::getCharacterItem()
{
    return m_character;
}
void Vision::updatePosition()
{
    if(NULL!=m_character)
    {
        //setPosition();
    }
}



/////////////////////////////////
/// Code SightItem
/////////////////////////////////

SightItem::SightItem(QMap<QString,VisualItem*>* characterItemMap)
 : m_defaultShape(Vision::DISK),m_defaultAngle(120),m_defaultRadius(50),m_bgColor(Qt::black),m_characterItemMap(characterItemMap)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    createActions();
    computeGradiants();

    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);

}

SightItem::~SightItem()
{

}
QRectF  SightItem::boundingRect() const
{

    if(NULL!=scene())
    {
        QList<QGraphicsView*> list = scene()->views();
        if(!list.isEmpty())
        {
            QGraphicsView* view = list.at(0);

            QPointF A = view->mapToScene( QPoint(0,0) );
            QPointF B = view->mapToScene( QPoint(
            view->viewport()->width(),
            view->viewport()->height() ));
            return QRectF( A, B );

        }
        return scene()->sceneRect();
    }
    else
    {
        return QRectF();
    }
}
void SightItem::setNewEnd(QPointF& nend)
{

}
void SightItem::writeData(QDataStream& out) const
{

}

void SightItem::readData(QDataStream& in)
{

}
VisualItem::ItemType SightItem::getType()
{
    return VisualItem::SIGHT;
}

void SightItem::fillMessage(NetworkMessageWriter* msg)
{

}

void SightItem::readItem(NetworkMessageReader* msg)
{

}
void SightItem::setGeometryPoint(qreal pointId,QPointF& pos)
{
    qDebug() << pointId << pos;
    if(pointId == 0)
    {
       // qDebug() << pos << m_radius << center;
        if(pos.x()<0)
        {
            pos.setX(1);
        }
        m_defaultRadius = pos.x();
       // qDebug() << pos << m_radius;
        computeGradiants();

    }
}
void SightItem::initChildPointItem()
{
    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 1 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this);
        tmp->setMotion(ChildPointItem::X_AXIS);
        //tmp->setRotationEnable(true);
        m_child->append(tmp);
    }
    updateChildPosition();
}
VisualItem* SightItem::getItemCopy()
{

}

void  SightItem::updateChildPosition()
{
    QPointF center = boundingRect().center();
    QPointF offSet(m_defaultRadius,0);
    center += offSet;
    m_child->at(0)->setPos(center);
}

void SightItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());//.width()/2,-boundingRect().height()/2,boundingRect().width(),boundingRect().height()
}
void SightItem::computeGradiants()
{
 /*   switch(m_defaultShape)
    {
    case DISK:
        m_radialGradient->setCenter(QPointF(0,0));
        m_radialGradient->setRadius(m_radius);
        //m_radialGradient->setFocalPoint(pos());
        //m_radialGradient->setFocalRadius(m_radius);
        m_bgColor.setAlpha(0);
        m_radialGradient->setColorAt(0.0,m_bgColor);
        m_radialGradient->setColorAt(0.89,m_bgColor);
        m_bgColor.setAlpha(120);
        m_radialGradient->setColorAt(0.9,m_bgColor);
        m_bgColor.setAlpha(255);
        m_radialGradient->setColorAt(1.0,m_bgColor);
        m_gradient = m_radialGradient;
        break;
    case ANGLE:
        m_conicalGradient->setCenter(pos());
        m_conicalGradient->setAngle(0);
        m_bgColor.setAlpha(0);
        m_conicalGradient->setColorAt(0.0,m_bgColor);
        m_conicalGradient->setColorAt(0.14,m_bgColor);
        m_bgColor.setAlpha(255);
        m_conicalGradient->setColorAt(0.15,m_bgColor);
        m_conicalGradient->setColorAt(0.85,m_bgColor);
        m_bgColor.setAlpha(0);
        m_conicalGradient->setColorAt(0.86,m_bgColor);
        m_conicalGradient->setColorAt(1.0,m_bgColor);
        m_gradient = m_conicalGradient;
        break;
    }*/
}
void SightItem::insertVision(CharacterItem* item)
{
    Vision* tmp = new Vision();
    tmp->setShape(m_defaultShape);
    tmp->setRadius(m_defaultRadius);
    tmp->setAngle(m_defaultAngle);
    tmp->setCharacterItem(item);
    m_visionMap.insert(item->getId(),tmp);
}
void SightItem::setDefaultShape(Vision::SHAPE shape)
{
    m_defaultShape = shape;
    computeGradiants();
    update();

}
void SightItem::setColor(QColor& color)
{
    m_bgColor = color;
}

void  SightItem::setDefaultRaduis(qreal rad)
{
    m_defaultRadius = rad;
    computeGradiants();
    update();
}

void  SightItem::setDefaultAngle(qreal rad)
{
    m_defaultAngle = rad;
    computeGradiants();
    update();
}
void SightItem::createActions()
{
    m_diskShape = new QAction(tr("Disk Shape"),this);
    m_diskShape->setCheckable(true);
    m_angleShape = new QAction(tr("Arc Shape"),this);
    m_angleShape->setCheckable(true);

    connect(m_diskShape,SIGNAL(triggered()),this,SLOT(closePath()));
    connect(m_angleShape,SIGNAL(triggered()),this,SLOT(closePath()));
}

void SightItem::addActionContextMenu(QMenu* menu)
{
    menu->addAction(m_diskShape);
    menu->addAction(m_angleShape);
}
void SightItem::moveVision(QString id, QPointF& pos)
{
    if(m_visionMap.contains(id))
    {
        m_visionMap.value(id)->setPosition(pos);
    }
}
