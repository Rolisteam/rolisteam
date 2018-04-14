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
#include "network/networkmessagereader.h"

#ifdef QT_QUICK_LIB
#include "charactersheet/charactersheet.h"
#include "charactersheet/rolisteamimageprovider.h"
#else
class RolisteamImageProvider
{

};
class CharacterSheet
{

};
#endif

#include <QList>
#include <QMovie>
class CharacterShape
{
public:
	CharacterShape();
	
	bool hasMovie();
	
private:
	QString m_name;
	QImage m_image;
	QMovie m_movie;
};
/**
 * @brief Represents PCs and NPCs.
 *
 * They are stored inside the Player who own them.
 */
class Character : public QObject,public Person
{
    Q_OBJECT
    Q_PROPERTY(int healthPoints READ getHealthPointsCurrent WRITE setHealthPointsCurrent NOTIFY currentHealthPointsChanged)
    Q_PROPERTY(int maxHP READ getHealthPointsMax WRITE setHealthPointsMax NOTIFY maxHPChanged)
    Q_PROPERTY(int minHP READ getHealthPointsMin WRITE setHealthPointsMin NOTIFY minHPChanged)
    Q_PROPERTY(QString avatarPath READ getAvatarPath WRITE setAvatarPath NOTIFY avatarPathChanged)
    Q_PROPERTY(bool isNpc READ isNpc WRITE setNpc NOTIFY npcChanged)
    Q_PROPERTY(int initiative READ getInitiativeScore WRITE setInitiativeScore NOTIFY initiativeChanged)
    Q_PROPERTY(qreal distancePerTurn READ getDistancePerTurn WRITE setDistancePerTurn NOTIFY distancePerTurnChanged)
    Q_PROPERTY(CharacterState* state READ getState WRITE setState NOTIFY stateChanged)

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
     * @brief setNpc
     */
    void setNpc(bool);
    /**
     * @brief number
     * @return
     */
    int number() const;
    void setNumber(int n);
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


    CharacterSheet* getSheet() const;
    void setSheet(CharacterSheet* sheet);

    Player *getParentPlayer() const;
    QString getParentId() const;

    virtual QHash<QString,QString> getVariableDictionnary();

    int indexOf(CharacterState *state);

	void insertAction(const QString& name, const QImage& img, const QKeySequence& seq);
	void removeAction(const QString& name);
	void clearActions();

    virtual void write(QDataStream &out, bool tag) const;
    virtual void read(QDataStream &in);

    int getHealthPointsMax() const;
    void setHealthPointsMax(int hpMax);

    int getHealthPointsMin() const;
    void setHealthPointsMin(int hpMin);

    int getHealthPointsCurrent() const;
    void setHealthPointsCurrent(int hpCurrent);

    QString getAvatarPath() const;
    void setAvatarPath(const QString &avatarPath);

    void setCurrentState(QString name,QColor color, QString image);

    int getInitiativeScore() const;
    void setInitiativeScore(int intiativeScore);

    qreal getDistancePerTurn() const;
    void setDistancePerTurn(const qreal &distancePerTurn);

    RolisteamImageProvider *getImageProvider() const;
    void setImageProvider(RolisteamImageProvider *imageProvider);

signals:
    void avatarChanged();
    void currentHealthPointsChanged();
    void maxHPChanged();
    void minHPChanged();
    void avatarPathChanged();
    void npcChanged();
    void initiativeChanged();
    void distancePerTurnChanged();
    void stateChanged();

protected:
    CharacterState* getStateFromIndex(int i);
private:
    void init();

private:
    bool m_isNpc = true;
    int m_number = 0;
    static QList<CharacterState*>* m_stateList;
	QList<CharacterShape*>* m_actionList;
    CharacterState* m_currentState = nullptr;
    CharacterSheet* m_sheet = nullptr;
    int m_healthPointsMax=100;
    int m_healthPointsMin=0;
    int m_healthPointsCurrent=100;
    QString m_avatarPath;
    RolisteamImageProvider* m_imageProvider = nullptr;
    int m_initiativeScore = 0;
    qreal m_distancePerTurn = 0;
};

#endif // CHARACTER_H
