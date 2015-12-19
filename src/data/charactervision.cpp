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
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include <QDebug>

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

CharacterVision::SHAPE CharacterVision::getShape()
{
    return m_shape;
}

void CharacterVision::showCorner(bool b)
{
    if(NULL!=m_cornerPoint)
    {
        m_cornerPoint->setVisible(b);
    }
}

void CharacterVision::updatePosition()
{

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
void CharacterVision::fill(NetworkMessageWriter* msg)
{
    msg->int8((int)m_shape);
    //msg->real(m_pos.x());
    //msg->real(m_pos.y());

    msg->real(m_radius);
    msg->real(m_angle);

   // qDebug() <<"Fill vision:" << m_pos << m_radius << m_angle;

}

void CharacterVision::readMessage(NetworkMessageReader* msg)
{
    m_shape =(CharacterVision::SHAPE) msg->int8();

    //m_pos.setX(msg->real());
    //m_pos.setY(msg->real());
    m_radius = msg->real();
    m_angle = msg->real();
   // qDebug() <<"read vision:" <<m_pos << m_radius << m_angle;

}
