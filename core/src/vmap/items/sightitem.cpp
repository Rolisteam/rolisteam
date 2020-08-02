/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <math.h>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "userlist/playermodel.h"
#include "vmap/controller/sightcontroller.h"

#include "data/character.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#define PI 3.14159265
/////////////////////////////////
/// Code FogSingularity
/////////////////////////////////
/*FogSingularity::FogSingularity(QPolygonF* poly, bool isAdding) : m_poly(poly), m_adding(isAdding) {}

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
    for(auto& point : *m_poly)
    {
        msg->real(point.x());
        msg->real(point.y());
    }
}

void FogSingularity::readItem(NetworkMessageReader* msg)
{
    quint64 pointCount= msg->uint64();
    m_adding= static_cast<bool>(msg->uint8());

    m_poly= new QPolygonF();
    for(unsigned int j= 0; j < pointCount; ++j)
    {
        qreal x= msg->real();
        qreal y= msg->real();
        QPointF pos(x, y);
        m_poly->append(pos);
    }
}
void FogSingularity::setPolygon(QPolygonF* poly)
{
    if(nullptr != m_poly)
    {
        delete m_poly;
    }
    m_poly= poly;
}*/

/////////////////////////////////
/// Code SightItem
/////////////////////////////////

SightItem::SightItem(vmap::SightController* ctrl, QMap<QString, CharacterItem*>* characterItemMap)
    : VisualItem(ctrl)
    , m_sightCtrl(ctrl)
    , m_defaultShape(CharacterVision::ANGLE)
    , m_defaultAngle(120)
    , m_defaultRadius(50)
//    , m_characterItemMap(characterItemMap)
{
    auto updateFunc= [this]() { update(); };
    connect(m_sightCtrl, &vmap::SightController::visibleChanged, this, [this](bool b) { setVisible(b); });
    connect(m_sightCtrl, &vmap::SightController::colorChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::fowPathChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::rectChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::characterSightChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::characterCountChanged, this, updateFunc);

    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    createActions();
    setAcceptedMouseButtons(Qt::NoButton);
    m_ctrl->setLayer(Core::Layer::FOG);
    setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

SightItem::~SightItem() {}
void SightItem::updateItemFlags()
{
    VisualItem::updateItemFlags();
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}

void SightItem::endOfGeometryChange()
{
    if(m_resizing)
    {
        markDirty();
    }
    VisualItem::endOfGeometryChange();
}

void SightItem::markDirty()
{
    m_fogChanged= true;
    updateVeil();
}
QRectF SightItem::boundingRect() const
{
    /*if(nullptr == scene())
        return QRectF();

    auto rect= scene()->sceneRect();

    if(rect.isNull())
        rect= QRectF(0, 0, 1000, 1000);

    return rect;*/

    return m_sightCtrl->rect();

    /*QList<QGraphicsView*> list= scene()->views();
    if(!list.isEmpty())
    {
        QGraphicsView* view= list.at(0);

        QPointF A= view->mapToScene(QPoint(0, 0));
        QPointF B= view->mapToScene(QPoint(view->viewport()->width(), view->viewport()->height()));

        return QRectF(A, B);
    }
    return scene()->sceneRect();*/
}
void SightItem::setNewEnd(const QPointF& nend)
{
    Q_UNUSED(nend)
    return;
}
void SightItem::writeData(QDataStream& out) const
{
    /*   out << m_fogHoleList.count();
       for(auto fog : m_fogHoleList)
       {
           out << *fog->getPolygon();
           out << fog->isAdding();
       }*/
}

void SightItem::readData(QDataStream& in)
{
    /*    int count;
        in >> count;
        for(int i= 0; i < count; ++i)
        {
            bool adding;
            auto poly= new QPolygonF();
            in >> *poly;
            in >> adding;

            auto hole= new FogSingularity(poly, adding);
            m_fogHoleList.append(hole);
        }
        updateVeil();
        update();*/
}

void SightItem::fillMessage(NetworkMessageWriter* msg)
{
    // msg->string16(m_id);

    // rect
    /*msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());*/

    // pos
    /*   msg->real(pos().x());
       msg->real(pos().y());

       msg->real(zValue());

       msg->uint64(static_cast<quint64>(m_fogHoleList.count()));
       for(auto& hole : m_fogHoleList)
       {
           hole->fillMessage(msg);
       }

       auto keys= m_characterItemMap->keys();
       msg->uint64(static_cast<quint64>(keys.size()));
       for(const QString& key : keys)
       {
           msg->string8(key);
       }*/
}

void SightItem::readItem(NetworkMessageReader* msg)
{
    // m_id= msg->string16();
    // rect
    /*m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());*/

    // pos
    /*  qreal x= msg->real();
      qreal y= msg->real();
      setPos(x, y);
      qreal z= msg->real();
      setZValue(z);

      quint64 count= msg->uint64();
      for(unsigned int i= 0; i < count; ++i)
      {
          FogSingularity* fogs= new FogSingularity();
          fogs->readItem(msg);
          m_fogHoleList.append(fogs);
      }

      count= msg->uint64();
      for(unsigned int i= 0; i < count; ++i)
      {
          QString str= msg->string8();
          // Character* item = PlayersList::instance()->getCharacter(str);
          VisualItem* item= m_characterItemMap->value(str);
          if(nullptr != item)
          {
              CharacterItem* cItem= dynamic_cast<CharacterItem*>(item);
              if(nullptr != cItem)
              {
                  m_characterItemMap->insert(str, cItem);
              }
          }
      }
      updateVeil();
      update();*/
}
void SightItem::setGeometryPoint(qreal pointId, QPointF& pos)
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
    // m_child= new QVector<ChildPointItem*>();
    connect(scene(), &QGraphicsScene::sceneRectChanged, m_sightCtrl, &vmap::SightController::setRect);
    m_sightCtrl->setRect(scene()->sceneRect());
}

void SightItem::monitorView()
{
    auto map= scene();
    if(!map)
        return;

    QList<QGraphicsView*> list= map->views();
    connect(map, &QGraphicsScene::sceneRectChanged, this, &SightItem::markDirty);
    connect(map, &QGraphicsScene::changed, this, &SightItem::markDirty);

    if(!list.isEmpty())
    {
        QGraphicsView* view= list.at(0);
        auto viewport= view->viewport();
        viewport->installEventFilter(this);
    }
}

bool SightItem::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::Resize)
    {
        markDirty();
    }
    return QObject::eventFilter(watched, event);
}

VisualItem* SightItem::getItemCopy()
{
    return nullptr;
}

void SightItem::updateChildPosition() {}

void SightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    painter->setPen(Qt::NoPen);
    m_sightCtrl->localIsGM() ? painter->setBrush(QColor(0, 0, 0, 125)) : painter->setBrush(QColor(0, 0, 0));

    QPainterPath path= m_sightCtrl->fowPath();

    // auto const& values= m_characterItemMap->values();
    auto visions= m_sightCtrl->visionData();
    for(auto& visionData : visions)
    {
        CharacterVision* vision= visionData.vision;

        QPainterPath subArea;
        subArea.setFillRule(Qt::WindingFill);
        auto itemRadius= visionData.radius;
        qreal rot= visionData.rotation;
        QMatrix mat;
        QPointF center= visionData.pos + QPointF(itemRadius, itemRadius);
        mat.translate(center.x(), center.y());
        mat.rotate(rot);

        path= path.subtracted(mat.map(visionData.shape.translated(-itemRadius, -itemRadius))); // always see the user
        switch(vision->shape())
        {
        case CharacterVision::DISK:
            subArea.addEllipse(QPointF(0, 0), vision->radius() + itemRadius, vision->radius() + itemRadius);
            break;
        case CharacterVision::ANGLE:
        {
            QRectF rectArc;
            rectArc.setCoords(-vision->radius(), -vision->radius(), vision->radius(), vision->radius());
            subArea.arcTo(rectArc, -vision->angle() / 2, vision->angle());
            // painter->setPen(QColor(255, 0, 0));
        }
        break;
        }
        path.moveTo(visionData.pos);
        path= path.subtracted(mat.map(subArea));
    }

    /*  else
      {
          auto const& values= m_characterItemMap->values();
          for(auto& charact : values)
          {
              if(charact->isLocal())
              {
                  QMatrix mat;
                  auto itemRadius= charact->getRadius();
                  qreal rot= charact->rotation();
                  QPointF center= charact->pos() + QPointF(itemRadius, itemRadius);
                  mat.translate(center.x(), center.y());
                  mat.rotate(rot);
                  auto shape= charact->shape();
                  path= path.subtracted(mat.map(charact->getTokenShape().translated(-itemRadius, -itemRadius)));
              }
          }
      }*/
    painter->drawPath(path);
    painter->restore();
}
// void SightItem::insertVision(CharacterItem* item)
//{
// item->setDefaultVisionParameter(m_defaultShape, m_defaultRadius, m_defaultAngle);
// if(nullptr != m_child)
/*{
    m_children.append(item->getRadiusChildWidget());
}*/
//}
/*void SightItem::removeVision(CharacterItem* item)
{
    if(m_characterItemMap->contains(item->getId()))
    {
        m_characterItemMap->remove(item->getId());
    }
    {
        m_children.removeAll(item->getRadiusChildWidget());
    }
}*/
void SightItem::setDefaultShape(CharacterVision::SHAPE shape)
{
    m_defaultShape= shape;
    update();
}

void SightItem::setDefaultRadius(qreal rad)
{
    m_defaultRadius= rad;
    update();
}

void SightItem::setDefaultAngle(qreal rad)
{
    m_defaultAngle= rad;
    update();
}
void SightItem::createActions() {}

void SightItem::moveVision(qreal id, QPointF& pos)
{
    Q_UNUSED(id)
    Q_UNUSED(pos)
    /*if(m_visionMap.contains(id))
    {
        m_visionMap.value(id)->setRadius(pos.x());
    }*/
}

/*FogSingularity* SightItem::addFogPolygon(QPolygonF* a, bool adding)
{
    FogSingularity* fogs= new FogSingularity(a, adding);
    m_fogHoleList << fogs;
    m_fogChanged= true;
    updateVeil();
    update();
    return fogs;
}*/
