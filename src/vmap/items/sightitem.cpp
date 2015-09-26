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

SightItem::SightItem()
 : m_shape(DISK),m_angle(120),m_radius(50)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);

    m_radialGradient = new QRadialGradient();
    m_conicalGradient = new QConicalGradient();
    computeGradiants();

}

SightItem::~SightItem()
{

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

}
void SightItem::initChildPointItem()
{

}
VisualItem* SightItem::getItemCopy()
{

}
void SightItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{

    qDebug() << option->rect << option->exposedRect;
    QBrush brush(*m_gradient);

    painter->setBrush(brush);
   // painter->setPen(Qt::NoPen);

    painter->drawRect(-boundingRect().width()/2,-boundingRect().height()/2,boundingRect().width(),boundingRect().height());


}
void SightItem::computeGradiants()
{
    switch(m_shape)
    {
    case DISK:
        m_radialGradient->setCenter(pos());
        m_radialGradient->setRadius(m_radius);
        //m_radialGradient->setFocalPoint(pos());
        //m_radialGradient->setFocalRadius(m_radius);
        m_radialGradient->setColorAt(0.0,QColor(0,0,0,0));
        m_radialGradient->setColorAt(0.89,QColor(0,0,0,0));
        m_radialGradient->setColorAt(0.9,QColor(0,0,0,120));
        m_radialGradient->setColorAt(1.0,QColor(0,0,0));
        m_gradient = m_radialGradient;
        break;
    case ANGLE:
        m_conicalGradient->setCenter(pos());
        m_conicalGradient->setAngle(0);
        m_conicalGradient->setColorAt(0.0,QColor(0,0,0,0));
        m_conicalGradient->setColorAt(0.14,QColor(0,0,0,0));
        m_conicalGradient->setColorAt(0.15,QColor(0,0,0));
        m_conicalGradient->setColorAt(0.85,QColor(0,0,0));
        m_conicalGradient->setColorAt(0.86,QColor(0,0,0,0));
        m_conicalGradient->setColorAt(1.0,QColor(0,0,0,0));
        m_gradient = m_conicalGradient;
        break;
    }
}
QRectF  SightItem::boundingRect() const
{
    return scene()->sceneRect();
}
void SightItem::setShape(SHAPE shape)
{
    m_shape = shape;
    computeGradiants();
    update();

}
void  SightItem::setRaduis(qreal rad)
{
    m_radius = rad;
    computeGradiants();
    update();
}

void  SightItem::setAngle(qreal rad)
{
    m_angle = rad;
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
