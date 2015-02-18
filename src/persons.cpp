/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include "persons.h"

#include <QUuid>


#include "networkmessagereader.h"
#include "networkmessagewriter.h"

/**********
 * Person *
 **********/
 
Person::Person(const QString & name, const QColor & color)
    : m_name(name), m_color(color)
{
    m_uuid = QUuid::createUuid();
}

Person::Person(const QString & uuid, const QString & name, const QColor & color)
    : m_uuid(uuid), m_name(name), m_color(color)
{
}

Person::Person()
{
}
Person::~Person()
{
}
const QString Person::uuid() const
{
    return m_uuid;
}

QString Person::name() const
{
    return m_name;
}

QColor Person::color() const
{
    return m_color;
}

Player * Person::parent() const
{
    return NULL;
}

bool Person::setColor(const QColor & color)
{
    if (color == m_color)
        return false;

    m_color = color;
    return true;
}

bool Person::setName(const QString & name)
{
    if (name == m_name)
        return false;

    m_name = name;
    return true;
}

/**********
 * Player *
 **********/

Player::Player(const QString & nom, const QColor & color, bool master, Liaison * link)
 : Person(nom, color), m_gameMaster(master), m_link(link)
{
}

Player::Player(const QString & uuid, const QString & nom, const QColor & color, bool master, Liaison * link)
 : Person(uuid, nom, color), m_gameMaster(master), m_link(link)
{
}

Player::Player(NetworkMessageReader & data, Liaison * link)
 : Person(), m_link(link)
{
    m_name = data.string16();
    m_uuid = data.string8();
    m_color = QColor(data.rgb());
    m_gameMaster  = (data.uint8() != 0);
}

Player::~Player()
{
    int charactersCount = m_characters.size();
    if (charactersCount > 0)
        qWarning("Player with characters deleted");
    for (int i = 0; i < charactersCount; i++)
    {
        delete m_characters.at(i);
    }
}

void Player::fill(NetworkMessageWriter & message)
{
    message.string16(m_name);
    message.string8(m_uuid);
    message.rgb(m_color);
    message.uint8(m_gameMaster ? 1 : 0);
}

Liaison * Player::link() const
{
    return m_link;
}

int Player::getCharactersCount() const
{
    return m_characters.size();
}

Character * Player::getCharacterByIndex(int index) const
{
    return m_characters[index];
}

int Player::getIndexOfCharacter(Character * character) const
{
    return m_characters.indexOf(character);
}
int Player::getIndexOf(QString id) const
{
    for(int i =0; i< m_characters.size() ; i++)
    {

        if(m_characters[i]->uuid()==id)
            return i;
    }

    return -1;
}
bool Player::isGM() const
{
    return m_gameMaster;
}

void Player::setGM(bool value)
{
    m_gameMaster = value;
}

void Player::addCharacter(Character * character)
{
    character->setParent(this);
    m_characters << character;
}

void Player::delCharacter(int index)
{
    if (index >= 0 && index < m_characters.size())
    {
        delete m_characters.at(index);
        m_characters.removeAt(index);
    }
}

bool Player::searchCharacter(Character * character, int & index) const
{
    for (int i=0; i < m_characters.size(); i++)
    {
        if (m_characters.at(i) == character)
        {
            index = i;
            return true;
        }
    }
    return false;
}

bool Player::hasFeature(const QString & name, quint8 version) const
{
    return m_features.contains(name) && m_features.value(name) >= version;
}

void Player::setFeature(const QString & name, quint8 version)
{
    if (hasFeature(name, version))
        return;

    m_features.insert(name, version);
}


/*************
 * Character *
 *************/

Character::Character(const QString & nom, const QColor & color)
    : Person(nom, color), m_parent(NULL)
{
}

Character::Character(const QString & uuid, const QString & nom, const QColor & color)
    : Person(uuid, nom, color), m_parent(NULL)
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

Player * Character::parent() const
{
    return m_parent;
}

void Character::setParent(Player * parent)
{
    // We can't move a character from a Player to another one.
    // If that happens, it's a fatal error.
    if (m_parent != NULL)
        qFatal("Illegal call of Character::setParent(Player *) : m_parent is not NULL.");

    m_parent = parent;
}
