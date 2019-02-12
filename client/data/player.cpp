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

#include "player.h"
#include "data/character.h"

#include <QDebug>
Player::Player() {}

Player::Player(const QString& nom, const QColor& color, bool master, NetworkLink* link)
    : Person(nom, color), m_gameMaster(master), m_link(link)
{
}

Player::Player(const QString& uuid, const QString& nom, const QColor& color, bool master, NetworkLink* link)
    : Person(uuid, nom, color), m_gameMaster(master), m_link(link)
{
}

Player::Player(NetworkMessageReader& data, NetworkLink* link) : Person(), m_link(link)
{
    readFromMsg(data);
}

Player::~Player()
{
    qDeleteAll(m_characters);
    m_characters.clear();
}
void Player::readFromMsg(NetworkMessageReader& data)
{
    if(!data.isValid())
    {
        qWarning() << "Network message OUT OF MEMORY";
        return;
    }
    m_name= data.string16();
    m_uuid= data.string8();

    m_color= QColor(data.rgb());
    m_gameMaster= (data.uint8() != 0);
    m_softVersion= data.string16();
    int childCount= data.int32();
    for(int i= 0; (i < childCount && data.isValid()); ++i)
    {
        try
        {
            Character* child= new Character();
            child->read(data);
            m_characters.append(child);
            child->setParentPerson(this);
            data.uint8();
        }
        catch(std::bad_alloc&)
        {
            qWarning() << "Bad alloc";
            return;
        }
    }
    if(!data.isValid())
    {
        qWarning() << "Network message OUT OF MEMORY";
        return;
    }
    QByteArray array= data.byteArray32();
    QDataStream in(&array, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_7);
    in >> m_features;
}
void Player::fill(NetworkMessageWriter& message, bool addAvatar)
{
    message.string16(m_name);
    message.string8(m_uuid);
    message.rgb(m_color.rgb());
    message.uint8(m_gameMaster ? 1 : 0);
    message.string16(QCoreApplication::instance()->applicationVersion());
    message.int32(m_characters.size());

    for(auto& item : m_characters)
    {
        item->fill(message, addAvatar);
        message.uint8(1); // add it to the map
    }

    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out << m_features;

    message.byteArray32(array);
}

NetworkLink* Player::link() const
{
    return m_link;
}

int Player::getChildrenCount() const
{
    return m_characters.size();
}

Character* Player::getCharacterByIndex(int index) const
{
    return m_characters[index];
}
QList<Character*> Player::getChildrenCharacter()
{
    return m_characters;
}
int Player::getIndexOfCharacter(Character* character) const
{
    return m_characters.indexOf(character);
}
int Player::getIndexOf(QString id) const
{
    for(int i= 0; i < m_characters.size(); i++)
    {
        if(m_characters[i]->getUuid() == id)
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
    m_gameMaster= value;
}

void Player::addCharacter(Character* character)
{
    if(!m_characters.contains(character))
    {
        character->setParentPerson(this);
        m_characters.append(character);
    }
}
void Player::clearCharacterList()
{
    for(auto& character : m_characters)
    {
        character->setParentPerson(nullptr);
    }
    m_characters.clear();
}

bool Player::removeChild(ResourcesNode* node)
{
    auto character= static_cast<Character*>(node);
    if(!m_characters.contains(character))
        return false;

    m_characters.removeOne(character);
    if(!character->isNpc())
    {
        delete character;
    }
    else
    {
        character->setParentPerson(nullptr);
    }
    return true;
}

bool Player::searchCharacter(Character* character, int& index) const
{
    for(int i= 0; i < m_characters.size(); i++)
    {
        if(m_characters.at(i) == character)
        {
            index= i;
            return true;
        }
    }
    return false;
}

QHash<QString, QString> Player::getVariableDictionnary()
{
    return QHash<QString, QString>();
}

bool Player::isLeaf() const
{
    return false;
}

bool Player::hasFeature(const QString& name, quint8 version) const
{
    return m_features.contains(name) && m_features.value(name) >= version;
}

void Player::setFeature(const QString& name, quint8 version)
{
    if(hasFeature(name, version))
        return;

    m_features.insert(name, version);
}
QString Player::getUserVersion() const
{
    return m_softVersion;
}

void Player::setUserVersion(QString softV)
{
    m_softVersion= softV;
}
void Player::copyPlayer(Player* player)
{
    setAvatar(player->getAvatar());
    setColor(player->getColor());
    setGM(player->isGM());
    setName(player->name());
    setUserVersion(player->getUserVersion());
}
bool Player::isFullyDefined()
{
    if(m_uuid.isEmpty())
        return false;
    if(m_name.isEmpty())
        return false;
    if(m_features.isEmpty())
        return false;

    return true;
}
