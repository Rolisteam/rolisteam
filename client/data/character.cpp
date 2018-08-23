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
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFileInfo>

#include "character.h"
#include "data/player.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

//  Begin character field
CharacterField::CharacterField()
{

}

CharacterField::~CharacterField()
{

}


CharacterShape::CharacterShape()
{

}

QString CharacterShape::name() const
{
    return m_name;
}

void CharacterShape::setName(const QString &name)
{
    m_name = name;
}

QImage CharacterShape::image() const
{
    return m_image;
}

void CharacterShape::setImage(const QImage &image)
{
    m_image = image;
}

const QMovie& CharacterShape::movie() const
{
    return m_movie;
}

void CharacterShape::setMovie(const QMovie &movie)
{
    m_movie.setFileName(movie.fileName());
}

QString CharacterShape::uri() const
{
    return m_uri;
}

void CharacterShape::setUri(const QString &uri)
{
    if(m_uri == uri)
        return;

    m_uri = uri;

    if(QFileInfo::exists(m_uri))
        m_image = QImage(m_uri);
    else
        m_image = QImage();
}

QVariant CharacterShape::getData(int col, int role)
{
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            return name();
        }
        else if(col == 1)
        {
            return uri();
        }
    }
    else if(Qt::DecorationRole == role && col == 0)
    {
        return image();
    }
    return QVariant();
}

bool CharacterShape::setData(int col, QVariant value, int role)
{
    bool set = false;
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            setName(value.toString());
            set = true;
        }
        else if(col == 1)
        {
            setUri(value.toString());
            set = true;
        }
    }
    return set;
}

CharacterAction::CharacterAction()
{

}

QString CharacterAction::name() const
{
    return m_name;
}

void CharacterAction::setName(const QString &name)
{
    m_name = name;
}

QString CharacterAction::command() const
{
    return m_command;
}

void CharacterAction::setCommand(const QString &command)
{
    m_command = command;
}

QVariant CharacterAction::getData(int col, int role)
{
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            return name();
        }
        else if(col == 1)
        {
            return command();
        }
    }
    return QVariant();
}

bool CharacterAction::setData(int col, QVariant value, int role)
{
    bool set = false;
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            setName(value.toString());
            set = true;
        }
        else if(col == 1)
        {
            setCommand(value.toString());
            set = true;
        }
    }
    return set;
}

CharacterProperty::CharacterProperty()
{

}

QString CharacterProperty::name() const
{
    return m_name;
}

void CharacterProperty::setName(const QString &name)
{
    m_name = name;
}

QString CharacterProperty::value() const
{
    return m_value;
}

void CharacterProperty::setValue(const QString &value)
{
    m_value = value;
}

QVariant CharacterProperty::getData(int col, int role)
{
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            return name();
        }
        else if(col == 1)
        {
            return value();
        }
    }
    return QVariant();
}

bool CharacterProperty::setData(int col, QVariant value, int role)
{
    bool set = false;
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            setName(value.toString());
            set = true;
        }
        else if(col == 1)
        {
            setValue(value.toString());
            set = true;
        }
    }
    return set;
}

//end character field

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

QList<CharacterShape *> Character::getShapeList() const
{
    return m_shapeList;
}

QList<CharacterAction *> Character::getActionList() const
{
    return m_actionList;
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
    if(qFuzzyCompare(m_distancePerTurn, distancePerTurn))
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
    m_hasInitScore = true;
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
        QPixmap pix(image);
        if(!pix.isNull())
          tmpState->setImage(pix);
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
    m_healthPointsCurrent = qBound(m_healthPointsMin, hpCurrent, m_healthPointsMax);
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
    QHash<QString,QString> variables;
    if(nullptr!=m_sheet)
    {
        #ifndef UNIT_TEST
        variables = m_sheet->getVariableDictionnary();
        #endif
    }
    else if(!m_propertyList.isEmpty())
    {
        for(auto property : m_propertyList)
        {
            if(property != nullptr)
                variables.insert(property->name(),property->value());
        }
    }
    return variables;
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
int Character::indexOfState(CharacterState* state)
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
    message.string8(nullptr != m_parent ? m_parent->getUuid() : QStringLiteral("nullptr"));
    message.string8(m_uuid);
    message.string16(m_name);
    message.int8(static_cast<qint8>(indexOfState(m_currentState)));
    message.uint8(static_cast<quint8>(m_isNpc));
    message.int32(m_number);
    message.rgb(m_color.rgb());
    message.int32(m_healthPointsCurrent);
    message.int32(m_healthPointsMin);
    message.int32(m_healthPointsMax);
    message.int32(m_initiativeScore);
    message.real(m_distancePerTurn);

    if(addAvatar)
    {
        message.uint8(static_cast<quint8>(!m_avatar.isNull()));
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
        message.uint8(static_cast<quint8>(false));
    }

}
QString Character::read(NetworkMessageReader& msg)
{
    if(!msg.isValid())
        return {};
    QString parentId = msg.string8();
    m_uuid = msg.string8();
    m_name = msg.string16();
    int currentStateIndex = msg.int8();
    if(currentStateIndex>=0)
    {
        m_currentState = getStateFromIndex(currentStateIndex);
    }
    m_isNpc = static_cast<bool>(msg.uint8());
    m_number = msg.int32();
    m_color = QColor(msg.rgb());
    m_healthPointsCurrent = msg.int32();
    m_healthPointsMin = msg.int32();
    m_healthPointsMax = msg.int32();
    m_initiativeScore = msg.int32();
    m_distancePerTurn = msg.real();

    bool hasAvatar = static_cast<bool>(msg.uint8());

    if(hasAvatar)
    {
        m_avatar = QImage::fromData(msg.byteArray32());
        emit avatarChanged();
    }

    return parentId;
}
CharacterState* Character::getStateFromIndex(int i)
{
    if(nullptr == m_stateList)
       return nullptr;

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
QColor Character::getLifeColor() const
{
    return m_lifeColor;
}
void Character::setLifeColor(QColor color)
{
    if(m_lifeColor == color)
        return;

    m_lifeColor = color;
    emit lifeColorChanged();
}

void Character::setInitCommand(const QString &init)
{
    if(m_initiativeRoll.command() == init)
        return;

    m_initiativeRoll.setCommand(init);
    emit initCommandChanged();
}
QString Character::getInitCommand() const
{
    return m_initiativeRoll.command();
}
QString Character::getToolTip() const
{
    QString tooltip(tr("%1:\n"
                       "HP: %2/%3\n"
                       "State: %4\n"
                       "Initiative Score: %5\n"
                       "Distance Per Turn: %6\n"
                       "type: %7\n"));
    QString stateName(tr("Not defined"));
    if(m_currentState)
        stateName = m_currentState->getLabel();

    return tooltip.arg(m_name).arg(m_healthPointsCurrent)
            .arg(m_healthPointsMax)
            .arg(stateName)
            .arg(m_initiativeScore)
            .arg(m_distancePerTurn)
            .arg(m_isNpc ? tr("NPC") : tr("PC"));
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

bool Character::hasInitScore()
{
    return m_hasInitScore;
}

void Character::clearInitScore()
{
    m_hasInitScore = false;
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
    if(m_isNpc == b)
        return;

    m_isNpc = b;
    emit npcChanged();
}
void Character::write(QDataStream& out, bool tag, bool) const
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

void Character::setCurrentShape(int index)
{
    if(qBound(0,index, m_shapeList.size()) != index)
        return;

    auto shape = m_shapeList[index];
    m_avatar = shape->image();
    emit avatarChanged();
}

void Character::setDefaultShape()
{
    m_avatar = m_defaultAvatar;
    emit avatarChanged();
}
void Character::readTokenObj(const QJsonObject& obj)
{
    m_isNpc = true;
    m_name = obj["name"].toString();
    m_color.setNamedColor(obj["color"].toString());
    m_healthPointsMax = obj["lifeMax"].toInt();
    m_healthPointsMin = obj["lifeMin"].toInt();
    m_healthPointsCurrent = obj["lifeCurrent"].toInt();
    m_initiativeScore = obj["initValue"].toInt();
    m_initiativeRoll.setName(tr("Initiative"));
    m_initiativeRoll.setCommand(obj["initCommand"].toString());
    m_avatarPath = obj["avatarUri"].toString();
    auto array = QByteArray::fromBase64(obj["avatarImg"].toString().toUtf8());
    m_avatar = QImage::fromData(array);
    m_defaultAvatar = m_avatar;

    auto actionArray = obj["actions"].toArray();
    for(auto act : actionArray)
    {
        auto actObj = act.toObject();
        auto action = new CharacterAction();
        action->setName(actObj["name"].toString());
        action->setCommand(actObj["command"].toString());
        m_actionList.append(action);
    }

    auto propertyArray = obj["properties"].toArray();
    for(auto pro : propertyArray)
    {
        auto proObj = pro.toObject();
        auto property = new CharacterProperty();
        property->setName(proObj["name"].toString());
        property->setValue(proObj["value"].toString());
        m_propertyList.append(property);
    }

    auto shapeArray = obj["shapes"].toArray();
    for(auto sha : shapeArray)
    {
        auto objSha = sha.toObject();
        auto shape = new CharacterShape();
        shape->setName(objSha["name"].toString());
        shape->setUri(objSha["uri"].toString());
        auto avatarData = QByteArray::fromBase64(objSha["dataImg"].toString().toUtf8());
        QImage img = QImage::fromData(avatarData);
        shape->setImage(img);
        m_shapeList.append(shape);
    }

}

