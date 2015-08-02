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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "person.h"

/**
 * @brief Represents PCs and NPCs.
 *
 * They are stored inside the Player who own them.
 */
class Character : public QObject,public Person
{
    Q_OBJECT
public:
    /**
     * @brief construtor
     */
    Character(const QString & name, const QColor & color,bool NPC = false, int number = 0);
    /**
     * @brief Character
     * @param uuid
     * @param name
     * @param color
     */
    Character(const QString & uuid, const QString & name, const QColor & color,bool NPC = false, int number = 0);
    /**
     * @brief Character
     * @param data
     */
    Character(NetworkMessageReader & data);
    /**
     * @brief fill
     * @param message
     */
    void fill(NetworkMessageWriter & message);
    /**
     * @brief parent
     * @return
     */
    Person* parent() const;
    /**
     * @brief setParent
     * @param player
     */
    void setParent(Person* player);
    /**
     * @brief isBool
     * @return
     */
    bool isNpc() const;
    /**
     * @brief number
     * @return
     */
    int number() const;
signals:
    void avatarChanged();
private:
    Person* m_parent;
    QPixmap* m_avatar;
    bool m_isNpc;
    int m_number;
};

#endif // CHARACTER_H
