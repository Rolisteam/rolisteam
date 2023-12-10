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
#include <QTransform>
#include <math.h>

#include "controller/item_controllers/sightcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "model/playermodel.h"

#include "data/character.h"

#define PI 3.14159265

/////////////////////////////////
/// Code SightItem
/////////////////////////////////

SightItem::SightItem(vmap::SightController* ctrl) : VisualItem(ctrl), m_sightCtrl(ctrl)
{
    setZValue(std::numeric_limits<qreal>::max());
    auto updateFunc= [this]() { update(); };
    connect(m_sightCtrl, &vmap::SightController::visibleChanged, this,
            [this]() { setVisible(m_sightCtrl->visible()); });
    connect(m_sightCtrl, &vmap::SightController::colorChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::fowPathChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::rectChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::characterSightChanged, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::requiredUpdate, this, updateFunc);
    connect(m_sightCtrl, &vmap::SightController::characterCountChanged, this, updateFunc);

    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    setAcceptedMouseButtons(Qt::NoButton);
    if(m_ctrl)
        m_ctrl->setLayer(Core::Layer::FOG);
    setFlags(QGraphicsItem::ItemSendsGeometryChanges);

    connect(this, &QGraphicsObject::parentChanged, this,
            [this]()
            {
                connect(scene(), &QGraphicsScene::sceneRectChanged, m_sightCtrl, &vmap::SightController::setRect);
                m_sightCtrl->setRect(scene()->sceneRect());
            });
}

SightItem::~SightItem() {}
void SightItem::updateItemFlags()
{
    VisualItem::updateItemFlags();
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}

QRectF SightItem::boundingRect() const
{
    if(m_sightCtrl)
        return m_sightCtrl->rect();
    else
        return {};
}
void SightItem::setNewEnd(const QPointF& nend)
{
    Q_UNUSED(nend)
    return;
}

void SightItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    painter->setPen(Qt::NoPen);
    m_sightCtrl->localIsGM() ? painter->setBrush(QColor(0, 0, 0, 125)) : painter->setBrush(QColor(0, 0, 0));

    QPainterPath path= m_sightCtrl->fowPath();

    if(m_sightCtrl->characterSight())
    {
        auto visions= m_sightCtrl->visionData();
        for(auto& vision : visions)
        {
            if(vision->removed())
                continue;

            QPainterPath subArea;
            subArea.setFillRule(Qt::WindingFill);
            auto itemRadius= vision->radius();
            qreal rot= vision->rotation();
            QTransform trans;
            QPointF center= vision->position(); // + QPointF(itemRadius, itemRadius);
            trans.translate(center.x(), center.y());
            trans.rotate(rot);

            auto side = vision->side()/2;
            QPainterPath token;
            token.addRect(QRectF{0,0,side*2,side*2});
            path= path.subtracted(
                trans.map(token.translated(-side,-side))); // always see the user
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
            }
            break;
            }
            path.moveTo(vision->position());
            path= path.subtracted(trans.map(subArea));
        }
    }

    painter->drawPath(path);
    painter->restore();
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
