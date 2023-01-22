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
#include "data/characterstate.h"

#include <QUuid>

CharacterState::CharacterState() : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    int a= 10;
    a= a + 10;
}
CharacterState::CharacterState(const CharacterState& copy)
{
    m_id= copy.id();
    m_color= copy.color();
    m_label= copy.label();
    m_imagePath= copy.imagePath();
    m_pixmap= copy.pixmap();
}

QString CharacterState::id() const
{
    return m_id;
}

void CharacterState::setId(const QString& id)
{
    if(id.isEmpty())
        return;
    m_id= id;
}

void CharacterState::setLabel(QString str)
{
    m_label= str;
}

void CharacterState::setColor(QColor str)
{
    m_color= str;
}

void CharacterState::setImagePath(const QString& str)
{
    m_imagePath= str;
    setPixmap(QPixmap(str));
}

void CharacterState::setPixmap(const QPixmap& pix)
{
    m_pixmap= pix;
}

const QString& CharacterState::label() const
{
    return m_label;
}
const QColor& CharacterState::color() const
{
    return m_color;
}
const QString& CharacterState::imagePath() const
{
    return m_imagePath;
}
const QPixmap& CharacterState::pixmap() const
{
    return m_pixmap;
}

bool CharacterState::hasImage() const
{
    return !m_pixmap.isNull();
}
