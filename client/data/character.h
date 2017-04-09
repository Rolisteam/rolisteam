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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "person.h"
#include "characterstate.h"
#include "charactersheet/charactersheet.h"

#include <QList>

/**
 * @brief Represents PCs and NPCs.
 *
 * They are stored inside the Player who own them.
 */
class Character : public QObject,public Person
{
    Q_OBJECT
public:
   // enum HeathState {Healthy,Lightly,Seriously,Dead,Sleeping,Bewitched};
    Character();
    /**
     * @brief construtor
     */
    Character(const QString & getName, const QColor & getColor,bool NPC = false, int number = 0);
    /**
     * @brief Character
     * @param uuid
     * @param name
     * @param color
     */
    Character(const QString & uuid, const QString & getName, const QColor & getColor,bool NPC = false, int number = 0);
    /**
     * @brief Character
     * @param data
     */
    Character(NetworkMessageReader & data);
    /**
     * @brief ~Character
     */
    virtual ~Character();
    /**
     * @brief fill
     * @param message
     */
    void fill(NetworkMessageWriter & message,bool addAvatar = true);
    /**
     * @brief read
     * @param msg
     * @return parentId
     */
    QString read(NetworkMessageReader& msg);
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
    /**
     * @brief getHeathState
     * @return
     */
    CharacterState* getState() const;

    /**
    * @brief serialisation function to write data
    */
    virtual void writeData(QDataStream& out) const;
    /**
    * @brief serialisation function to read data.
    */
    virtual void readData(QDataStream& in);

    /**
     * @brief setListOfCharacterState
     */
    static void setListOfCharacterState(QList<CharacterState*>*);

    /**
     * @brief getCharacterStateList
     * @return
     */
    static QList<CharacterState*>* getCharacterStateList();
    /**
     * @brief getStateFromLabel
     * @param label
     * @return
     */
    CharacterState* getStateFromLabel(QString label);

    void  setState(CharacterState*  h);

    void setNpc(bool);

    CharacterSheet* getSheet() const;
    void setSheet(CharacterSheet* sheet);

    Player *getParentPlayer() const;
    QString getParentId() const;


    virtual QHash<QString,QString> getVariableDictionnary();

    int indexOf(CharacterState *state);
signals:
    void avatarChanged();

protected:
    CharacterState* getStateFromIndex(int i);
private:
    void init();
private:
    bool m_isNpc;
    int m_number;
    CharacterState* m_currentState;
    CharacterSheet* m_sheet;
    static QList<CharacterState*>* m_stateList;
};

#endif // CHARACTER_H
