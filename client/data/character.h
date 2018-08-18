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

class CharacterField
{
public:
    enum Type {Shape, Action, Property};
    CharacterField();
    virtual ~CharacterField();

    virtual Type getType() = 0;
    virtual QVariant getData(int col,int role) = 0;
    virtual bool setData(int col,QVariant value, int role ) = 0;
};

class CharacterShape : public CharacterField
{
public:
	CharacterShape();
	
	bool hasMovie();
	
    QString name() const;
    void setName(const QString &name);

    QImage image() const;
    void setImage(const QImage &image);

    const QMovie& movie() const;
    void setMovie(const QMovie &movie);

    QString uri() const;
    void setUri(const QString &uri);

    virtual QVariant getData(int col,int role);
    virtual bool setData(int col,QVariant value, int role );

    virtual Type getType() { return Shape; }

private:
    QString m_name;
    QString m_uri;
    QImage m_image;
    QMovie m_movie;
};

class CharacterAction : public CharacterField
{
public:
    CharacterAction();

    QString name() const;
    void setName(const QString &name);

    QString command() const;
    void setCommand(const QString &command);

    bool isValid(){ return !m_command.isEmpty(); }

    virtual QVariant getData(int col,int role);
    virtual bool setData(int col,QVariant value, int role );

    virtual Type getType() { return Action; }

private:
    QString m_name;
    QString m_command;
};

class CharacterProperty : public CharacterField
{
public:
    CharacterProperty();

    QString name() const;
    void setName(const QString &name);

    QString value() const;
    void setValue(const QString &value);

    virtual QVariant getData(int col,int role);
    virtual bool setData(int col,QVariant value, int role );

     virtual Type getType() { return Property; }

private:
    QString m_name;
    QString m_value;
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
    Q_PROPERTY(QColor lifeColor READ getLifeColor WRITE setLifeColor NOTIFY lifeColorChanged)
    Q_PROPERTY(QString initCommand READ getInitCommand WRITE setInitCommand NOTIFY initCommandChanged)

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

    bool hasInitScore();


    CharacterSheet* getSheet() const;
    void setSheet(CharacterSheet* sheet);

    Player *getParentPlayer() const;
    QString getParentId() const;

    virtual QHash<QString,QString> getVariableDictionnary();

    int indexOfState(CharacterState *state);

	void insertAction(const QString& name, const QImage& img, const QKeySequence& seq);
	void removeAction(const QString& name);
	void clearActions();

    virtual void write(QDataStream &out, bool tag, bool saveData = true) const;
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

    QColor getLifeColor() const;
    void setLifeColor(QColor color);

    virtual QString getToolTip() const;
    void readTokenObj(const QJsonObject& obj);

    QString getInitCommand()const;
    void setInitCommand(const QString& init);

    QList<CharacterAction *> getActionList() const;
    QList<CharacterShape *> getShapeList() const;
public slots:
    void clearInitScore();
    void setDefaultShape();
    void setCurrentShape(int index);
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
    void lifeColorChanged();
    void initCommandChanged();

protected:
    CharacterState* getStateFromIndex(int i);
private:
    void init();

private:
    bool m_isNpc = true;
    int m_number = 0;
    static QList<CharacterState*>* m_stateList;
    QList<CharacterShape*> m_shapeList;
    QList<CharacterAction*> m_actionList;
    QList<CharacterProperty*> m_propertyList;
    CharacterState* m_currentState = nullptr;
    CharacterSheet* m_sheet = nullptr;
    int m_healthPointsMax=100;
    int m_healthPointsMin=0;
    int m_healthPointsCurrent=100;
    QString m_avatarPath;
    RolisteamImageProvider* m_imageProvider = nullptr;
    int m_initiativeScore = 0;
    CharacterAction m_initiativeRoll;
    qreal m_distancePerTurn = 0;
    QColor m_lifeColor = QColor(Qt::green);
    bool m_hasInitScore = false;
    QImage m_defaultAvatar;
};

#endif // CHARACTER_H
