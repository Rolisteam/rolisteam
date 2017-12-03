/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
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
#include <QMap>
#include <QImage>

class Character;
class NetworkMessageReader;
class NetworkMessageWriter;
class Player;

/**
 * @brief Abstract class for players and characters.
 */
class Person 
{

public:
    /**
     * @brief Person
     * @param name
     * @param color
     */
    Person(const QString & getName, const QColor & getColor);
    /**
     * @brief Person
     * @param uuid
     * @param name
     * @param color
     */
    Person(const QString & uuid, const QString & getName, const QColor & getColor);
    /**
     * @brief ~Person
     */
    virtual ~Person();
    /**
     * @brief getUuid
     * @return
     */
    const QString getUuid() const;
    /**
     * @brief name
     * @return
     */
    QString getName() const;
    /**
     * @brief color
     * @return
     */
    QColor  getColor() const;
    /**
     * @brief parent
     * @return
     */
    virtual Person* getParent() const;
    void setParentPerson(Person* parent);

    /**
     * @brief fill
     * @param message
     */
    virtual void fill(NetworkMessageWriter & message, bool withAvatar = true) = 0;

    /**
    * @brief gives access to person's avatar.
    */
    virtual const QImage& getAvatar() const;

    /**
    * @brief gives access to person's avatar.
    */
    virtual bool hasAvatar() const ;

    /**
    * @brief set the person's avatar.
    */
    virtual bool setAvatar(const QImage& p);
    /**
     * @brief setColor
     * @param color
     * @return
     */
    bool setColor(const QColor & color);
    /**
     * @brief setName
     * @param getName
     * @return
     */
    bool setName(const QString & getName);
    /**
     * @brief checkedState
     * @return
     */
    Qt::CheckState checkedState();
    /**
     * @brief isLeaf
     * @return
     */
    virtual bool isLeaf() const;
    /**
     * @brief setState
     * @param c
     */
    void setState(Qt::CheckState c);

    virtual QHash<QString,QString> getVariableDictionnary() = 0;


    ResourcesNode::TypeResource getResourcesType() const;
protected:
    /**
     * @brief Person
     */
    Person();

    QString m_uuid;
    QString m_name;
    QColor  m_color;
    QImage  m_avatar;
    Person* m_parent;
    Qt::CheckState m_checkState;
private:
    friend class PlayersList;
};







#endif
