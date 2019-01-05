/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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
/// Code FogSingularity
/////////////////////////////////
FogSingularity::FogSingularity(QPolygonF* poly,bool isAdding)
    : m_poly(poly),m_adding(isAdding)
{

}

const QPolygonF* FogSingularity::getPolygon() const
{
    return m_poly;
}
bool FogSingularity::isAdding() const
{
    return m_adding;
}

void FogSingularity::fillMessage(NetworkMessageWriter* msg)
{
    msg->uint64(static_cast<quint64>(m_poly->size()));
    msg->uint8(m_adding);
    for( auto& point : *m_poly)
    {
        msg->real(point.x());
        msg->real(point.y());
    }
}

void FogSingularity::readItem(NetworkMessageReader* msg)
{
    quint64 pointCount = msg->uint64();
    m_adding = static_cast<bool>(msg->uint8());

    m_poly = new QPolygonF();
    for(unsigned int j = 0; j <pointCount; ++j)
    {
        qreal x = msg->real();
        qreal y = msg->real();
        QPointF pos(x,y);
        m_poly->append(pos);
    }

}
void FogSingularity::setPolygon(QPolygonF* poly)
{
    if(nullptr!=m_poly)
    {
        delete m_poly;
    }
    m_poly = poly;
}

/////////////////////////////////
/// Code SightItem
/////////////////////////////////

SightItem::SightItem(QMap<QString,CharacterItem*>* characterItemMap)
    : m_defaultShape(CharacterVision::ANGLE),m_defaultAngle(120),m_defaultRadius(50),m_characterItemMap(characterItemMap),m_bgColor(Qt::black),m_count(0)
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
void SightItem::updateItemFlags()
{
    VisualItem::updateItemFlags();
    setFlag(QGraphicsItem::ItemIsMovable,false);
}
QRectF  SightItem::boundingRect() const
{
    if(nullptr!=scene())
    {
        QList<QGraphicsView*> list = scene()->views();
        if(!list.isEmpty())
        {
            QGraphicsView* view = list.at(0);

            QPointF A = view->mapToScene( QPoint(0,0) );
            QPointF B = view->mapToScene( QPoint(view->viewport()->width(),view->viewport()->height()));

            return QRectF( mapFromScene(A), mapFromScene(B) );
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
    Q_UNUSED(nend)
    return;
}
void SightItem::writeData(QDataStream& out) const
{
    out << m_fogHoleList.count();
    qDebug() << "count write data" << m_fogHoleList.count();
    for(auto fog : m_fogHoleList)
    {
        out << *fog->getPolygon();
        out << fog->isAdding();
    }
}

void SightItem::readData(QDataStream& in)
{
    int count;
    in >> count;
    qDebug() << "count read data" << count;
    for(int i = 0; i < count; ++i)
    {
        bool adding;
        auto poly = new QPolygonF();
        in >> *poly;
        in >> adding;

        auto hole = new FogSingularity(poly,adding);
        m_fogHoleList.append(hole);
    }
    updateVeil();
    update();
}
VisualItem::ItemType SightItem::getType() const
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

     msg->real(zValue());

    msg->uint64(static_cast<quint64>(m_fogHoleList.count()));
    for(auto& hole: m_fogHoleList)
    {

        hole->fillMessage(msg);
    }

    auto keys = m_characterItemMap->keys();
    msg->uint64(static_cast<quint64>(keys.size()));
    for(const QString& key: keys)
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
    qreal z = msg->real();
    setZValue(z);

    quint64 count = msg->uint64();
    for(unsigned int i = 0; i<count;++i)
    {
        FogSingularity* fogs = new FogSingularity();
        fogs->readItem(msg);
        m_fogHoleList.append(fogs);
    }

    count = msg->uint64();
    for(unsigned int i = 0;i < count;++i)
    {
        QString str = msg->string8();
        //Character* item = PlayersList::instance()->getCharacter(str);
        VisualItem* item= m_characterItemMap->value(str);
        if(nullptr!=item)
        {
            CharacterItem* cItem = dynamic_cast<CharacterItem*>(item);
            if(nullptr!=cItem)
            {
                m_characterItemMap->insert(str,cItem);
            }
        }
    }
    updateVeil();
    update();
}
void SightItem::setGeometryPoint(qreal pointId,QPointF& pos)
{
    Q_UNUSED(pointId)
    Q_UNUSED(pos)
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
    return nullptr;
}


void  SightItem::updateChildPosition()
{

}

void SightItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    painter->setPen(Qt::NoPen);
    if(getOption(VisualItem::LocalIsGM).toBool())
    {
        painter->setBrush(QColor(0,0,0,125));
    }
    else
    {
        painter->setBrush(QColor(0,0,0));
    }

    updateVeil();
    QPainterPath path = m_path;

    if(getOption(VisualItem::EnableCharacterVision).toBool())
    {
        auto const& values = m_characterItemMap->values();
        for(auto& charact : values)
        {
            if((nullptr!=charact)&&
                    ((charact->isLocal())||getOption(VisualItem::LocalIsGM).toBool()))
            {
                CharacterVision* vision = charact->getVision();

                QPainterPath subArea;
                subArea.setFillRule(Qt::WindingFill);
                auto itemRadius = charact->getRadius();
                qreal rot = charact->rotation();
                QMatrix mat;
                QPointF center = charact->pos()+QPointF(itemRadius,itemRadius);
                mat.translate(center.x(),center.y());
                mat.rotate(rot);

                path = path.subtracted(mat.map(charact->shape().translated(-itemRadius,-itemRadius)));//always see the user
                switch(vision->getShape())
                {
                    case CharacterVision::DISK:
                    {
                        subArea.addEllipse(QPointF(0,0),vision->getRadius()+itemRadius,vision->getRadius()+itemRadius);
                    }
                    break;
                    case CharacterVision::ANGLE:
                    {
                        QRectF rectArc;
                        rectArc.setCoords(-vision->getRadius(),-vision->getRadius(),vision->getRadius(),vision->getRadius());
                        subArea.arcTo(rectArc,-vision->getAngle()/2,vision->getAngle());
                        painter->setPen(QColor(255,0,0));

                    }
                    break;
                }
                path.moveTo(charact->pos());
                path = path.subtracted(mat.map(subArea));
            }
        }
    }
    painter->drawPath(path);
    painter->restore();
}
void SightItem::insertVision(CharacterItem* item)
{
    item->setDefaultVisionParameter(m_defaultShape,m_defaultRadius,m_defaultAngle);
    if(nullptr!=m_child)
    {
        m_child->append(item->getRadiusChildWidget());
    }
}
void SightItem::removeVision(CharacterItem* item)
{
    if(m_characterItemMap->contains(item->getId()))
    {
        m_characterItemMap->remove(item->getId());
    }
    if(nullptr!=m_child)
    {
        m_child->removeAll(item->getRadiusChildWidget());
    }
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

void  SightItem::setDefaultRadius(qreal rad)
{
    m_defaultRadius = rad;
    update();
}
void SightItem::setVisible(bool visible)
{
    if(nullptr!=m_child)
    {
        for (auto& item: *m_child)
        {
            if(nullptr!=item)
            {
               // item->setVisible(visible);
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

}

void SightItem::addActionContextMenu(QMenu* menu)
{
    Q_UNUSED(menu)
    /*menu->addAction(m_diskShape);
    menu->addAction(m_angleShape);*/
}
void SightItem::moveVision(qreal id, QPointF& pos)
{
    Q_UNUSED(id)
    Q_UNUSED(pos)
    /*if(m_visionMap.contains(id))
    {
        m_visionMap.value(id)->setRadius(pos.x());
    }*/
}
void SightItem::updateVeil()
{
    QPainterPath path;
    QRectF rect = boundingRect();
    if(m_rectOfVeil.isNull())
    {
        m_rectOfVeil = rect;
    }
    else //if(rect.width()*rect.height() > m_rectOfVeil.width()*m_rectOfVeil.height())
    {
        m_rectOfVeil = m_rectOfVeil.united(rect);
    }
    path.addRect(m_rectOfVeil);
    for(auto& fogs: m_fogHoleList)
    {
        QPainterPath subPoly;
        const QPolygonF* poly = fogs->getPolygon();
        subPoly.addPolygon(*poly);
        if(!fogs->isAdding())
        {
            path = path.subtracted(subPoly);
        }
        else
        {
            path = path.united(subPoly);
        }
    }
    m_path = path;
}

void SightItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsObject::contextMenuEvent(event);
}
FogSingularity* SightItem::addFogPolygon(QPolygonF* a,bool adding)
{
    FogSingularity* fogs = new FogSingularity(a,adding);
    m_fogHoleList << fogs;
    updateVeil();
    update();
    return fogs;
}
