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

#include "player.h"

#include <QApplication>



#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "data/character.h"
Player::Player()
{

}

Player::Player(const QString & nom, const QColor & color, bool master, NetworkLink * link)
    : Person(nom, color), m_gameMaster(master), m_link(link)
{
}

Player::Player(const QString & uuid, const QString & nom, const QColor & color, bool master, NetworkLink * link)
    : Person(uuid, nom, color), m_gameMaster(master), m_link(link)
{
}

Player::Player(NetworkMessageReader & data, NetworkLink * link)
    : Person(), m_link(link)
{
    m_name = data.string16();
    m_uuid = data.string8();
    m_color = QColor(data.rgb());
    m_gameMaster  = (data.uint8() != 0);
    m_softVersion = data.string16();
    int childCount = data.int32();
    for(int i = 0; i < childCount;++i)
    {
        Character* child = new Character(data);
        m_characters.append(child);
        data.uint8();
    }
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
    message.string16(qApp->applicationVersion());
    message.int32(m_characters.size());

    foreach(Character* item,m_characters)
    {
        item->fill(message);
        message.uint8(1); // add it to the map
    }
}

NetworkLink * Player::link() const
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
QList<Character*> Player::getChildrenCharacter()
{
	return m_characters;
}
int Player::getIndexOfCharacter(Character * character) const
{
    return m_characters.indexOf(character);
}
int Player::getIndexOf(QString id) const
{
    for(int i =0; i< m_characters.size() ; i++)
    {
        if(m_characters[i]->getUuid()==id)
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

void Player::addCharacter(Character* character)
{
    character->setParent(this);
    m_characters.append(character);
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
QString Player::getUserVersion()
{
    return m_softVersion;
}

void Player::setUserVersion(QString softV)
{
    m_softVersion = softV;
}
