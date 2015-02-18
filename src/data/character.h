/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
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
#include <QMetaType>
/**
* @brief Character herit from person
* @brief this class is part of Composite pattern.
*
*/
class Character : public Person
{
public:
    /**
      * @brief Constructor with argument
      */
    Character(QString name,QColor color,QString uri);
    /**
      * @brief default constructor
      */
    Character();
    /**
      * @brief Copy constructor
      */
    Character(const Character& p);
    /**
      * @brief overridden function tell either are children or not.
      * @return true means it has children, otherwise false
      */
    bool hasChildren() const;

private:
    /**
     * @brief pointer to its parent.
     */
    Person* m_parent;
};
Q_DECLARE_METATYPE(Character)
#endif // CHARACTER_H
