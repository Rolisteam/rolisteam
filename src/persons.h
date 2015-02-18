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


#ifndef PERSONS_H
#define PERSONS_H

#include <QColor>
#include <QList>

class Character;
class DataReader;
class DataWriter;
class Liaison;
class Player;

/**
 * @brief Abstract class for players and characters.
 */
class Person 
{
    public:
        Person(const QString & name, const QColor & color);
        Person(const QString & uuid, const QString & name, const QColor & color);

        const QString uuid() const;
        QString name() const;
        QColor  color() const;
        virtual Player * parent() const;

        virtual void fill(DataWriter & message) = 0;

    protected:
        Person();

        QString m_uuid;
        QString m_name;
        QColor  m_color;

    private:
        friend class PlayersList;
        bool setColor(const QColor & color);
        bool setName(const QString & name);
};


/**
 * @brief Represents a player.
 *
 * Players are stored in PlayersList. A player may have some characters.
 */
class Player : public Person {
    public:
        Player(const QString & name, const QColor & color, bool master = false, Liaison * link = NULL);
        Player(const QString & uuid, const QString & name, const QColor & color, bool master = false, Liaison * link = NULL);
        Player(DataReader & data, Liaison * link = NULL);
        ~Player();

        void fill(DataWriter & message);

        Liaison * link() const;

        int         getCharactersCount() const;
        Character * getCharacterByIndex(int index) const;
        int         getIndexOfCharacter(Character * character) const;

        bool isGM() const;

    private:
        friend class PlayersList;
        void addCharacter(Character * character);
        void delCharacter(int index);
        bool searchCharacter(Character * character, int & index) const;
        void setGM(bool value);

        bool m_gameMaster;
        Liaison * m_link;
        QList<Character *> m_characters;
};


/**
 * @brief Represents PCs and NPCs.
 *
 * They are stored inside the Player who own them.
 */
class Character : public Person {
    public:
        Character(const QString & name, const QColor & color);
        Character(const QString & uuid, const QString & name, const QColor & color);
        Character(DataReader & data);

        void fill(DataWriter & message);

        Player * parent() const;

    private:
        friend class Player;
        void setParent(Player * player);

        Player * m_parent;
};

#endif
