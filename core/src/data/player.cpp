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

#include "player.h"

#include <QBuffer>
#include <QDebug>

#include "data/character.h"
bool containsCharacter(QString name, QColor color, QString path,
                       const std::vector<std::unique_ptr<Character>>& characters)
{
    return (characters.end()
            != std::find_if(characters.begin(), characters.end(),
                            [name, color, path](const std::unique_ptr<Character>& character) {
                                return character->name() == name && character->getColor() == color
                                       && character->avatarPath() == path;
                            }));
}

bool containsPointer(Character* character, const std::vector<std::unique_ptr<Character>>& characters)
{
    return (
        characters.end()
        != std::find_if(characters.begin(), characters.end(),
                        [character](const std::unique_ptr<Character>& pointer) { return character == pointer.get(); }));
}

Player::Player() {}

Player::Player(const QString& nom, const QColor& color, bool master)
    : Person(nom, color), m_gameMaster(master) //, m_link(link)
{
}

Player::Player(const QString& uuid, const QString& nom, const QColor& color, bool master)
    : Person(nom, color, uuid), m_gameMaster(master) //, m_link(link)
{
}

Player::~Player()= default;

/*void Player::readFromMsg(NetworkMessageReader& data)
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
    setUserVersion(data.string16());

    bool hasAvatar= static_cast<bool>(data.uint8());
    if(hasAvatar)
    {
        auto avatar= QImage::fromData(data.byteArray32());
        setAvatar(avatar);
    }

    int childCount= data.int32();
    for(int i= 0; (i < childCount && data.isValid()); ++i)
    {
        try
        {
            Character* child= new Character();
            child->read(data);
            //            m_characters.append(child);
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

/*NetworkLink* Player::link() const
{
    return m_link;
}*/

int Player::childrenCount() const
{
    return static_cast<int>(m_characters.size());
}

const std::vector<std::unique_ptr<Character>>& Player::children()
{
    return m_characters;
}

Character* Player::getCharacterByIndex(int index) const
{
    if(index < m_characters.size() && index >= 0)
        return m_characters[index].get();
    return nullptr;
}

Character* Player::characterById(const QString& id) const
{
    auto it= std::find_if(m_characters.begin(), m_characters.end(),
                          [id](const std::unique_ptr<Character>& character) { return character->uuid() == id; });
    if(it == m_characters.end())
        return nullptr;
    return it->get();
}

int Player::indexOf(ResourcesNode* character) const
{
    auto it= std::find_if(m_characters.begin(), m_characters.end(),
                          [character](const std::unique_ptr<Character>& data) { return character == data.get(); });

    if(it == m_characters.end())
        return -1;
    else
        return static_cast<int>(std::distance(m_characters.begin(), it));
}

bool Player::isGM() const
{
    return m_gameMaster;
}

void Player::setGM(bool value)
{
    m_gameMaster= value;
}

void Player::addCharacter(const QString& name, const QColor& color, const QString& path,
                          const QHash<QString, QVariant>& params, bool Npc)
{
    if(containsCharacter(name, color, path, m_characters))
        return;

    auto character= new Character(name, color, m_gameMaster, Npc);
    character->setAvatarPath(path);
    addCharacter(character);
    // params FIXME set value from params.
    // data->setLifeColor();
}

void Player::addCharacter(Character* character)
{
    if(containsPointer(character, m_characters))
        return;

    std::unique_ptr<Character> data(character);

    data->setParentPerson(this);
    m_characters.push_back(std::move(data));
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
    auto character= dynamic_cast<Character*>(node);
    if(nullptr == character)
        return false;

    auto id= character->uuid();
    auto size= m_characters.size();
    m_characters.erase(
        std::remove_if(m_characters.begin(), m_characters.end(),
                       [id](const std::unique_ptr<Character>& character) { return character->uuid() == id; }));

    return size != m_characters.size();
}

bool Player::searchCharacter(Character* character, int& index) const
{
    /* for(int i= 0; i < m_characters.size(); i++)
     {
         if(m_characters.at(i) == character)
         {
             index= i;
             return true;
         }
     }*/
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
const QMap<QString, quint8>& Player::features() const
{
    return m_features;
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
