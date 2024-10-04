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

#include "data/player.h"

#include <QBuffer>
#include <QDebug>

#include "data/character.h"
#include "worker/utilshelper.h"

bool containsCharacter(QString name, QColor color, QByteArray avatar,
                       const std::vector<std::unique_ptr<Character>> &characters)
{
    return (characters.end()
            != std::find_if(characters.begin(), characters.end(),
                            [name, color, avatar](const std::unique_ptr<Character> &character) {
                                return character->name() == name && character->getColor() == color
                                        && character->avatar() == avatar;
                            }));
}

bool containsPointer(Character *character,
                     const std::vector<std::unique_ptr<Character>> &characters)
{
    return (characters.end()
            != std::find_if(characters.begin(), characters.end(),
                            [character](const std::unique_ptr<Character> &pointer) {
                                return character == pointer.get();
                            }));
}

Player::Player() { }

Player::Player(const QString &nom, const QColor &color, bool master)
    : Person(nom, color), m_gameMaster(master) //, m_link(link)
{
}

Player::Player(const QString &uuid, const QString &nom, const QColor &color, bool master)
    : Person(nom, color, uuid), m_gameMaster(master) //, m_link(link)
{
}

Player::~Player() = default;

int Player::characterCount() const
{
    return static_cast<int>(m_characters.size());
}

const std::vector<std::unique_ptr<Character>> &Player::children()
{
    return m_characters;
}

Character *Player::getCharacterByIndex(int index) const
{
    if (index < static_cast<int>(m_characters.size()) && index >= 0)
        return m_characters[static_cast<std::size_t>(index)].get();
    return nullptr;
}

Character *Player::characterById(const QString &id) const
{
    auto it = std::find_if(
            m_characters.begin(), m_characters.end(),
            [id](const std::unique_ptr<Character> &character) { return character->uuid() == id; });
    if (it == m_characters.end())
        return nullptr;
    return it->get();
}

int Player::indexOf(Character *character) const
{
    auto it = std::find_if(m_characters.begin(), m_characters.end(),
                           [character](const std::unique_ptr<Character> &data) {
                               return character == data.get();
                           });

    if (it == m_characters.end())
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
    if (value == m_gameMaster)
        return;
    m_gameMaster = value;
    emit gmChanged();
}

void Player::addCharacter(const QString &uuid, const QString &name, const QColor &color,
                          const QByteArray &data, const QHash<QString, QVariant> &params, bool Npc)
{
    if (containsCharacter(name, color, data, m_characters))
        return;

    using std::placeholders::_1;

    auto character = new Character(uuid, name, color, m_gameMaster, Npc);
    helper::utils::setParamIfAny<int>(Core::updater::key_char_property_hp, params,
                                      std::bind(&Character::setHealthPointsCurrent, character, _1));
    helper::utils::setParamIfAny<int>(Core::updater::key_char_property_maxhp, params,
                                      std::bind(&Character::setHealthPointsMax, character, _1));
    helper::utils::setParamIfAny<int>(Core::updater::key_char_property_minhp, params,
                                      std::bind(&Character::setHealthPointsMin, character, _1));
    helper::utils::setParamIfAny<int>(Core::updater::key_char_property_dist, params,
                                      std::bind(&Character::setDistancePerTurn, character, _1));
    helper::utils::setParamIfAny<QString>(Core::updater::key_char_property_state_id, params,
                                          std::bind(&Character::setStateId, character, _1));
    helper::utils::setParamIfAny<QColor>(Core::updater::key_char_property_life_color, params,
                                         std::bind(&Character::setLifeColor, character, _1));
    helper::utils::setParamIfAny<bool>(Core::updater::key_char_property_has_init, params,
                                       std::bind(&Character::setHasInitiative, character, _1));
    helper::utils::setParamIfAny<QString>(Core::updater::key_char_property_init_cmd, params,
                                          std::bind(&Character::setInitCommand, character, _1));
    helper::utils::setParamIfAny<int>(Core::updater::key_char_property_init_score, params,
                                      std::bind(&Character::setInitiativeScore, character, _1));

    character->setAvatar(data);
    addCharacter(character);
}

void Player::addCharacter(Character *character)
{
    if (!character || containsPointer(character, m_characters))
        return;

    std::unique_ptr<Character> data(character);

    connect(data.get(), &Character::nameChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::colorChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::avatarChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::currentHealthPointsChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::maxHPChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::minHPChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::distancePerTurnChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::hasInitScoreChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::lifeColorChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::initiativeChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::stateIdChanged, this, &Player::characterChanged);
    connect(data.get(), &Character::initCommandChanged, this, &Player::characterChanged);

    data->setParentPerson(this);
    auto id = data->uuid();
    m_characters.push_back(std::move(data));
    emit characterCountChanged();
    emit characterAdded(id, uuid());
}

QStringList Player::characterIds() const
{
    QStringList res;

    std::transform(std::begin(m_characters), std::end(m_characters), std::back_inserter(res),
                   [](const std::unique_ptr<Character> &character) { return character->uuid(); });

    return res;
}

void Player::clearCharacterList()
{
    for (auto &character : m_characters) {
        character->setParentPerson(nullptr);
    }
    m_characters.clear();
    emit characterCountChanged();
}

bool Player::removeChild(Character *character)
{
    if (nullptr == character)
        return false;
    auto id = character->uuid();
    auto size = m_characters.size();
    m_characters.erase(std::remove_if(
            m_characters.begin(), m_characters.end(),
            [id](const std::unique_ptr<Character> &tmp) { return tmp->uuid() == id; }));

    if (size != m_characters.size()) {
        emit characterCountChanged();
        return true;
    } else
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

QString Player::userVersion() const
{
    return m_softVersion;
}

void Player::setUserVersion(QString softV)
{
    m_softVersion = softV;
}
void Player::copyPlayer(Player *player)
{
    setAvatar(player->avatar());
    setColor(player->getColor());
    setGM(player->isGM());
    setName(player->name());
    setUserVersion(player->userVersion());
}

bool Player::isFullyDefined()
{
    if (m_uuid.isEmpty())
        return false;
    if (m_name.isEmpty())
        return false;

    return true;
}
