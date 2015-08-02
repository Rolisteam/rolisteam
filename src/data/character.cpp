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
#include <QColor>
#include <QUuid>

#include "character.h"
#include "data/player.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

Character::Character(const QString & nom, const QColor & color,bool npc,int number)
    : Person(nom, color), m_parent(NULL),m_isNpc(npc),m_number(number)
{
}

Character::Character(const QString & uuid, const QString & nom, const QColor & color,bool npc,int number)
    : Person(uuid, nom, color), m_parent(NULL),m_isNpc(npc),m_number(number)
{
}

Character::Character(NetworkMessageReader & data)
    : Person(), m_parent(NULL)
{
    m_uuid = data.string8();
    m_name = data.string16();
    m_color = QColor(data.rgb());
}

void Character::fill(NetworkMessageWriter & message)
{
    message.string8(m_parent->uuid());
    message.string8(m_uuid);
    message.string16(m_name);
    message.rgb(m_color);
}

Person* Character::parent() const
{
    return m_parent;
}

void Character::setParent(Person * parent)
{
    // We can't move a character from a Player to another one.
    // If that happens, it's a fatal error.
    if (m_parent != NULL)
        qFatal("Illegal call of Character::setParent(Player *) : m_parent is not NULL.");

    m_parent = parent;
}
int Character::number() const
{
    return m_number;
}

bool Character::isNpc() const
{
    return m_isNpc;
}
