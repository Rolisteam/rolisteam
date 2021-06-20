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

#ifndef PLAYER_H
#define PLAYER_H
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "person.h"

#include <QMetaType>
#include <memory>

class NetworkLink;
/**
 * @brief Represents a player.
 *
 * Players are stored in PlayersList. A player may have some characters.
 */
class Player : public Person
{
    Q_OBJECT
public:
    Player();
    Player(const QString& getName, const QColor& getColor, bool master= false);
    Player(const QString& uuid, const QString& getName, const QColor& getColor, bool master= false);
    virtual ~Player() override;

    virtual int characterCount() const;
    Character* getCharacterByIndex(int index) const;
    Character* characterById(const QString& id) const;
    const std::vector<std::unique_ptr<Character>>& children();

    // Children
    int indexOf(Character* character) const;
    void addCharacter(const QString& name, const QColor& color, const QString& path, const QHash<QString, QVariant>&,
                      bool Npc);
    void addCharacter(Character* character);
    virtual bool removeChild(Character*);
    void clearCharacterList();
    bool searchCharacter(Character* character, int& index) const;
    virtual bool isLeaf() const override;

    bool isGM() const;
    void setGM(bool value);

    QString getUserVersion() const;

    void setUserVersion(QString softV);

    virtual QHash<QString, QString> getVariableDictionnary() override;
    bool isFullyDefined();
    void copyPlayer(Player* player);

private:
    std::vector<std::unique_ptr<Character>> m_characters;
    bool m_gameMaster= false;
    QString m_softVersion;
};

#endif // PLAYER_H
