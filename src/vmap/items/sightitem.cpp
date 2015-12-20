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
#include <math.h>


#include "userlist/playersList.h"

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"
#include "data/character.h"

#define PI 3.14159265
/////////////////////////////////
/// Code Vision
/////////////////////////////////


/////////////////////////////////
/// Code SightItem
/////////////////////////////////

SightItem::SightItem(QMap<QString,CharacterItem*>* characterItemMap)
    : m_defaultShape(CharacterVision::ANGLE),m_defaultAngle(120),m_defaultRadius(50),m_bgColor(Qt::black),m_characterItemMap(characterItemMap),m_count(0)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    createActions();
    setAcceptedMouseButtons(Qt::NoButton);
    m_layer = FOG;
    setFlags(QGraphicsItem::ItemSendsGeometryChanges);
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
    return;
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
     msg->string16(m_id);

     //rect
     msg->real(m_rect.x());
     msg->real(m_rect.y());
     msg->real(m_rect.width());
     msg->real(m_rect.height());

     //pos
     msg->real(pos().x());
     msg->real(pos().y());

    msg->uint64(m_fogHoleList.count());
    foreach(QPolygonF* hole, m_fogHoleList)
    {
        msg->uint64(hole->size());
        for( auto point : *hole)
        {
            msg->real(point.x());
            msg->real(point.y());
        }
    }

    msg->uint64(m_characterItemMap->keys().size());
    foreach(QString key, m_characterItemMap->keys())
    {
        msg->string8(key);
    }
}

void SightItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    //rect
    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());

    //pos
    qreal x  = msg->real();
    qreal y = msg->real();
    setPos(x,y);

    quint64 count = msg->uint64();
    for(int i = 0; i<count;++i)
    {
        quint64 pointCount = msg->uint64();
        QPolygonF* poly = new QPolygonF();
        for(int j = 0; j <pointCount; ++j)
        {
            qreal x = msg->real();
            qreal y = msg->real();
            QPointF pos(x,y);
            poly->append(pos);
        }
        m_fogHoleList.append(poly);
    }

    count = msg->uint64();
    for(int i = 0;i < count;++i)
    {
        QString str = msg->string8();
        //Character* item = PlayersList::instance()->getCharacter(str);
        VisualItem* item= m_characterItemMap->value(str);
        if(NULL!=item)
        {
            CharacterItem* cItem = dynamic_cast<CharacterItem*>(item);
            if(NULL!=cItem)
            {
                m_characterItemMap->insert(str,cItem);
            }
        }
    }

    update();
}
void SightItem::setGeometryPoint(qreal pointId,QPointF& pos)
{
    /* if(m_visionMap.contains(pointId))
    {
        Vision* vis = m_visionMap.value(pointId);
        vis->setRadius(pos.x());
    }*/
}
void SightItem::initChildPointItem()
{
    m_child = new QVector<ChildPointItem*>();
}
VisualItem* SightItem::getItemCopy()
{
    return NULL;
}


void  SightItem::updateChildPosition()
{

}

void SightItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->setPen(Qt::NoPen);
    if(getOption(VisualItem::LocalIsGM).toBool())
    {
        painter->setBrush(QColor(0,0,0,125));
    }
    else
    {
        painter->setBrush(QColor(0,0,0));
    }

    QRectF rect = boundingRect();

    QPainterPath path;
    path.addRect(rect);


    foreach(QPolygonF* poly, m_fogHoleList)
    {
        QPainterPath subPoly;
        subPoly.addPolygon(*poly);
        path = path.subtracted(subPoly);
    }

    if(getOption(VisualItem::EnableCharacterVision).toBool())
    {
        foreach(CharacterItem* charact , m_characterItemMap->values())
        {
            if((NULL!=charact)&&
                    ((charact->isLocal())||getOption(VisualItem::LocalIsGM).toBool()))
            {
                CharacterVision* vision = charact->getVision();

                QPainterPath subArea;
                subArea.setFillRule(Qt::WindingFill);
                int itemRadius = charact->getRadius();

                 path = path.subtracted(charact->shape().translated(charact->pos()));//always see the user

                switch(vision->getShape())
                {
                    case CharacterVision::DISK:
                    {
                        subArea.moveTo(charact->pos()+QPointF(itemRadius,itemRadius));
                        subArea.addEllipse(charact->pos()+QPointF(itemRadius,itemRadius),vision->getRadius()+itemRadius,vision->getRadius()+itemRadius);
                        //subArea.addEllipse(vision->getCharacterItem()->boundingRect());
                    }
                    break;
                    case CharacterVision::ANGLE:
                    {
                        QPointF center= charact->pos()+QPointF(itemRadius,itemRadius);
                        subArea.moveTo(center);


                        QRectF rectArc;
                        rectArc.setCoords(center.x()-vision->getRadius(),center.y()-vision->getRadius(),center.x()+vision->getRadius(),center.y()+vision->getRadius());

                        qreal rot = charact->rotation();

                        subArea.arcTo(rectArc,-vision->getAngle()/2-rot,vision->getAngle());


                        painter->setPen(QColor(255,0,0));

                    }
                    break;
                }
                path.moveTo(charact->pos());
                path = path.subtracted(subArea);
            }
        }


    }
    painter->drawPath(path);
}
void SightItem::insertVision(CharacterItem* item)
{
    item->setDefaultVisionParameter(m_defaultShape,m_defaultRadius,m_defaultAngle);
    m_child->append(item->getRadiusChildWidget());
}
void SightItem::removeVision(CharacterItem* item)
{

}
void SightItem::setDefaultShape(CharacterVision::SHAPE shape)
{
    m_defaultShape = shape;
    update();
}
void SightItem::setColor(QColor& color)
{
    m_bgColor = color;
}

void  SightItem::setDefaultRaduis(qreal rad)
{
    m_defaultRadius = rad;
    update();
}
void SightItem::setVisible(bool visible)
{
    if(NULL!=m_child)
    {
        foreach (ChildPointItem* item, *m_child)
        {
            if(NULL!=item)
            {
                item->setVisible(visible);
            }
        }
    }
    VisualItem::setVisible(visible);
}

void  SightItem::setDefaultAngle(qreal rad)
{
    m_defaultAngle = rad;
    update();
}
void SightItem::createActions()
{
    /*m_diskShape = new QAction(tr("Disk Shape"),this);
    m_diskShape->setCheckable(true);
    m_angleShape = new QAction(tr("Arc Shape"),this);
    m_angleShape->setCheckable(true);*/

    /*connect(m_diskShape,SIGNAL(triggered()),this,SLOT(closePath()));
    connect(m_angleShape,SIGNAL(triggered()),this,SLOT(closePath()));*/
}

void SightItem::addActionContextMenu(QMenu* menu)
{
    menu->addAction(m_diskShape);
    menu->addAction(m_angleShape);
}
void SightItem::moveVision(qreal id, QPointF& pos)
{
    /*if(m_visionMap.contains(id))
    {
        m_visionMap.value(id)->setRadius(pos.x());
    }*/
}
void SightItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsObject::contextMenuEvent(event);
}
void SightItem::addFogPolygon(QPolygonF* a)
{
    m_fogHoleList << a;
    update();
}
void SightItem::setGM(bool b)
{
    m_isGM = b;
}
