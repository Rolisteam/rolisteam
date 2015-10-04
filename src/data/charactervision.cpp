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

#include "charactervision.h"

CharacterVision::CharacterVision(QObject* parent)
    : QObject(parent),m_cornerPoint(NULL)
{

}

CharacterVision::~CharacterVision()
{

}

void CharacterVision::setAngle(qreal a)
{
    m_angle = a;
}

void CharacterVision::setRadius(qreal r)
{
    m_radius = r;
}

void CharacterVision::setPosition(QPointF& p)
{
    m_pos = p;
}

void CharacterVision::setShape(CharacterVision::SHAPE s)
{
    m_shape = s;
}


qreal CharacterVision::getAngle()
{
    return m_angle;
}

qreal CharacterVision::getRadius()
{
    return m_radius;
}

/*const QPointF CharacterVision::getPos()
{
    return m_character->pos();
}*/

CharacterVision::SHAPE CharacterVision::getShape()
{
    return m_shape;
}

/*void CharacterVision::setCharacterItem(CharacterItem* item)
{
    m_character = item;
    connect(m_character,SIGNAL(selectStateChange(bool)),this,SLOT(showCorner(bool)));
    connect(m_character,SIGNAL(positionChanged()),this,SLOT(updatePosition()));
    connect(m_character,SIGNAL(heightChanged()),this,SLOT(updatePosition()));
    connect(m_character,SIGNAL(itemGeometryChanged(VisualItem*)),this,SLOT(updatePosition()));
}*/
void CharacterVision::showCorner(bool b)
{
    if(NULL!=m_cornerPoint)
    {
        m_cornerPoint->setVisible(b);
    }
}

void CharacterVision::updatePosition()
{
//    if(NULL!=m_cornerPoint)
//    {
//        m_cornerPoint
//    }
}
void CharacterVision::setCornerPoint(ChildPointItem* b)
{
    m_cornerPoint = b;
}
ChildPointItem* CharacterVision::getCornerPoint()
{
    return m_cornerPoint;
}
bool CharacterVision::isVisible()
{
    return m_visible;
}

void CharacterVision::setVisible(bool b)
{
    m_visible = b;
}
