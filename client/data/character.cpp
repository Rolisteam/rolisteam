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
#include <QColor>
#include <QUuid>
#include <QDebug>
#include <QPixmap>
#include <QBuffer>

#include "character.h"
#include "data/player.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

QList<CharacterState*>* Character::m_stateList = nullptr;

Character::Character()
    :Person(), m_currentState(nullptr),m_sheet(nullptr)
{
	init();
}

Character::Character(const QString & nom, const QColor & color,bool npc,int number)
    : Person(nom, color), m_isNpc(npc),m_number(number),m_currentState(nullptr),m_sheet(nullptr)
{
	init();
}

Character::Character(const QString & uuid, const QString & nom, const QColor & color,bool npc,int number)
    : Person(uuid, nom, color),m_isNpc(npc),m_number(number),m_currentState(nullptr),m_sheet(nullptr)
{
	init();
}

Character::Character(NetworkMessageReader & data)
    : Person(), m_currentState(nullptr),m_sheet(nullptr)
{
    read(data);

    init();
}

Character::~Character()
{
}
void Character::init()
{
	if((nullptr != m_stateList)&&(nullptr == m_currentState)&&(!m_stateList->isEmpty()))
	{
		m_currentState = m_stateList->first();
	}
}

RolisteamImageProvider *Character::getImageProvider() const
{
    return m_imageProvider;
}

void Character::setImageProvider(RolisteamImageProvider *imageProvider)
{
    m_imageProvider = imageProvider;
}

qreal Character::getDistancePerTurn() const
{
    return m_distancePerTurn;
}

void Character::setDistancePerTurn(const qreal &distancePerTurn)
{
    if(m_distancePerTurn == distancePerTurn)
        return;
    m_distancePerTurn = distancePerTurn;
    emit distancePerTurnChanged();
}

int Character::getInitiativeScore() const
{
    return m_initiativeScore;
}

void Character::setInitiativeScore(int intiativeScore)
{
    if(m_initiativeScore == intiativeScore)
        return;
    m_initiativeScore = intiativeScore;
    emit initiativeChanged();
}

QString Character::getAvatarPath() const
{
    return m_avatarPath;
}

void Character::setAvatarPath(const QString &avatarPath)
{
    if(m_avatarPath == avatarPath)
        return;

    m_avatarPath = avatarPath;
    emit avatarPathChanged();
}

void Character::setCurrentState(QString name, QColor color, QString image)
{
    CharacterState* tmpState=nullptr;
    for(auto state: *m_stateList)
    {
        if(state->getLabel() == name && color == state->getColor() )
        {
            tmpState = state;
        }
    }

    if(tmpState == nullptr)
    {
        tmpState = new CharacterState();
        tmpState->setLabel(name);
        tmpState->setColor(color);
        m_stateList->append(tmpState);
    }

    //m_currentState = tmpState;
    setState(tmpState);
}

int Character::getHealthPointsCurrent() const
{
    return m_healthPointsCurrent;
}

void Character::setHealthPointsCurrent(int hpCurrent)
{
    if(hpCurrent == m_healthPointsCurrent)
        return;
    m_healthPointsCurrent = hpCurrent;
    emit currentHealthPointsChanged();
}

int Character::getHealthPointsMin() const
{
    return m_healthPointsMin;
}

void Character::setHealthPointsMin(int hpMin)
{
    if(hpMin == m_healthPointsMin)
        return;
    m_healthPointsMin = hpMin;
    emit minHPChanged();
}

int Character::getHealthPointsMax() const
{
    return m_healthPointsMax;
}

void Character::setHealthPointsMax(int hpMax)
{
    if(hpMax == m_healthPointsMax)
        return;
    m_healthPointsMax = hpMax;
    emit maxHPChanged();
}

QString Character::getParentId() const
{
    if(nullptr!=m_parent)
    {
        return m_parent->getUuid();
    }
    return QString();
}

QHash<QString, QString> Character::getVariableDictionnary()
{
    if(nullptr!=m_sheet)
    {
        #ifndef UNIT_TEST
        return m_sheet->getVariableDictionnary();
        #endif
    }
    return QHash<QString, QString>();
}

CharacterSheet *Character::getSheet() const
{
    return m_sheet;
}

void Character::setSheet(CharacterSheet *sheet)
{
    m_sheet = sheet;
}
void Character::setListOfCharacterState(QList<CharacterState*>* list)
{
    m_stateList = list;
}
int Character::indexOf(CharacterState* state)
{
    if(nullptr != m_stateList)
    {
        return m_stateList->indexOf(state);
    }
    else
    {
        return -1;
    }
}

CharacterState* Character::getStateFromLabel(QString label)
{
    for(CharacterState* state: *m_stateList)
    {
        if(state->getLabel() == label)
        {
            return state;
        }
    }
    return nullptr;
}

void Character::fill(NetworkMessageWriter & message,bool addAvatar)
{
    if(nullptr!=m_parent)
    {
        message.string8(m_parent->getUuid());
    }
    else
    {
        message.string8("nullptr");
    }
    message.string8(m_uuid);
    message.string16(m_name);
    message.int8(indexOf(m_currentState));
    message.uint8((int)m_isNpc);
    message.int32(m_number);
    message.rgb(m_color.rgb());

    if(addAvatar)
    {
        message.uint8((quint8)!m_avatar.isNull());
        if(!m_avatar.isNull())
        {
            QByteArray baImage;
            QBuffer bufImage(&baImage);
            if(m_avatar.save(&bufImage, "PNG", 70))
            {
                message.byteArray32(baImage);
            }
        }
    }
    else
    {
        message.uint8((quint8)false);
    }

}
QString Character::read(NetworkMessageReader& msg)
{
    QString parentId = msg.string8();
    m_uuid = msg.string8();
    m_name = msg.string16();
    int currentStateIndex = msg.int8();
    if(currentStateIndex>=0)
    {
        m_currentState = getStateFromIndex(currentStateIndex);
    }
    m_isNpc = (bool)msg.uint8();
    m_number = msg.int32();
    m_color = QColor(msg.rgb());

    bool hasAvatar = (bool) msg.uint8();

    if(hasAvatar)
    {
        m_avatar = QImage::fromData(msg.byteArray32());
    }

    return parentId;
}
CharacterState* Character::getStateFromIndex(int i)
{
    if(m_stateList->empty())
        return nullptr;
    if(m_stateList->size()>i)
    {
        return m_stateList->at(i);
    }
    return nullptr;
}

Player* Character::getParentPlayer() const
{
    return dynamic_cast<Player*>(m_parent);
}

int Character::number() const
{
    return m_number;
}

void Character::setNumber(int n)
{
    m_number = n;
}

bool Character::isNpc() const
{
    return m_isNpc;
}
void  Character::setState(CharacterState*  h)
{
    if(h == m_currentState)
        return;
    m_currentState = h;
    emit stateChanged();
}
CharacterState* Character::getState() const
{
    return m_currentState;
}
void Character::writeData(QDataStream& out) const
{
    out << m_uuid;
    out << m_name;
    if(nullptr!=m_currentState)
    {
        out << true;
        out << m_currentState->getLabel();
    }
    else
    {
        out << false;
    }
    out << m_isNpc;
    out << m_number;
    out << m_color;
    out << m_avatar;
}
void Character::readData(QDataStream& in)
{
    in >> m_uuid;
    in >> m_name;
    bool hasState;
    in >> hasState;
    if(hasState)
    {
        QString value;
        in >> value;
        m_currentState= getStateFromLabel(value);
    }
    else
    {
        m_currentState = nullptr;
    }
    in >> m_isNpc;
    in >> m_number;
    in >> m_color;
    in >> m_avatar;
}
QList<CharacterState*>* Character::getCharacterStateList()
{
	return m_stateList;
}
void Character::setNpc(bool b)
{
    m_isNpc = b;
}
void Character::write(QDataStream& out, bool tag) const
{
    if(tag)
    {
        out << QStringLiteral("Character");
    }
    out << m_isNpc;
    out << m_number;
    out << m_name;
    out << m_uuid;
    out << m_color;
    if(nullptr != m_parent)
        out << m_parent->getUuid();
    else
        out << QString();

    out << static_cast<int>(m_checkState);

    QByteArray array;
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    m_avatar.save(&buffer,"PNG");
    out << array;

}
void Character::read(QDataStream& in)
{
    in >> m_isNpc;
    in >> m_number;
    in >> m_name;
    in >> m_uuid;
    in >> m_color;
    QString parentId;
    in >> parentId;
    int checkState;
    in >> checkState;
    m_checkState = static_cast<Qt::CheckState>(checkState);
    QByteArray array;
    in >> array;
    m_avatar = QImage::fromData(array);

}
