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
#include <QBuffer>
#include <QColor>
#include <QDebug>
#include <QFileInfo>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QUuid>

#include "charactersheet/rolisteamimageprovider.h"

#include "worker/iohelper.h"

#include "data/character.h"
#ifndef UNIT_TEST
#include "charactersheet.h"
#endif
#include "data/player.h"

//  Begin character field
CharacterField::CharacterField() {}

CharacterField::~CharacterField() {}

CharacterShape::CharacterShape() {}

QString CharacterShape::name() const
{
    return m_name;
}

void CharacterShape::setName(const QString& name)
{
    m_name= name;
}

const QImage& CharacterShape::image() const
{
    return m_image;
}

void CharacterShape::setImage(const QImage& image)
{
    m_image= image;
}

const QMovie& CharacterShape::movie() const
{
    return m_movie;
}

void CharacterShape::setMovie(const QMovie& movie)
{
    m_movie.setFileName(movie.fileName());
}

QString CharacterShape::uri() const
{
    return m_uri;
}

void CharacterShape::setUri(const QString& uri)
{
    if(m_uri == uri)
        return;

    m_uri= uri;
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
    else if(Qt::DecorationRole == role && col == 0 && !m_image.isNull())
    {
        return QPixmap::fromImage(m_image).scaled(m_size, m_size, Qt::KeepAspectRatio);
    }
    return QVariant();
}

bool CharacterShape::setData(int col, QVariant value, int role)
{
    bool set= false;
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            setName(value.toString());
            set= true;
        }
        else if(col == 1)
        {
            setUri(value.toString());
            updateImage();
            set= true;
        }
    }
    return set;
}

void CharacterShape::updateImage()
{
    m_image= QImage(m_uri);
}

void CharacterShape::setSize(int size)
{
    if(size == m_size)
        return;
    m_size= size;
}

///////////////////////////////////
///
/// Start CharacterAction
///
///////////////////////////////////

CharacterAction::CharacterAction() {}

QString CharacterAction::name() const
{
    return m_name;
}

void CharacterAction::setName(const QString& name)
{
    m_name= name;
}

QString CharacterAction::command() const
{
    return m_command;
}

void CharacterAction::setCommand(const QString& command)
{
    m_command= command;
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
    bool set= false;
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            setName(value.toString());
            set= true;
        }
        else if(col == 1)
        {
            setCommand(value.toString());
            set= true;
        }
    }
    return set;
}

CharacterProperty::CharacterProperty() {}

QString CharacterProperty::name() const
{
    return m_name;
}

void CharacterProperty::setName(const QString& name)
{
    m_name= name;
}

QString CharacterProperty::value() const
{
    return m_value;
}

void CharacterProperty::setValue(const QString& value)
{
    m_value= value;
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
    bool set= false;
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        if(col == 0)
        {
            setName(value.toString());
            set= true;
        }
        else if(col == 1)
        {
            setValue(value.toString());
            set= true;
        }
    }
    return set;
}

// end character field

QList<CharacterState*>* Character::m_stateList= nullptr;

Character::Character(QObject* parent) : Person(parent), m_sheet(nullptr) {}

Character::Character(const QString& name, const QColor& color, bool npc, int number)
    : Person(name, color), m_isNpc(npc), m_number(number), m_sheet(nullptr)
{
}

Character::Character(const QString& uuid, const QString& name, const QColor& color, bool npc, int number)
    : Person(name, color, uuid), m_isNpc(npc), m_number(number), m_sheet(nullptr)
{
}

/*Character::Character(NetworkMessageReader& data) : Person(), m_sheet(nullptr)
{
    read(data);
}*/

Character::~Character() {}

QList<CharacterShape*> Character::shapeList() const
{
    return m_shapeList;
}

QList<CharacterAction*> Character::actionList() const
{
    return m_actionList;
}

QList<CharacterProperty*> Character::propertiesList() const
{
    return m_propertyList;
}

void Character::defineActionList(const QList<CharacterAction*>& actions)
{
    m_actionList.clear();
    m_actionList << actions;
}

void Character::defineShapeList(const QList<CharacterShape*>& shape)
{
    m_shapeList.clear();
    m_shapeList << shape;
}
void Character::definePropertiesList(const QList<CharacterProperty*>& props)
{
    m_propertyList.clear();
    m_propertyList << props;
}

RolisteamImageProvider* Character::getImageProvider() const
{
    return m_imageProvider;
}

void Character::setImageProvider(RolisteamImageProvider* imageProvider)
{
    m_imageProvider= imageProvider;
}

qreal Character::getDistancePerTurn() const
{
    return m_distancePerTurn;
}

void Character::setDistancePerTurn(const qreal& distancePerTurn)
{
    if(qFuzzyCompare(m_distancePerTurn, distancePerTurn))
        return;
    m_distancePerTurn= distancePerTurn;
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
    m_hasInitScore= true;
    m_initiativeScore= intiativeScore;
    emit initiativeChanged();
}

int Character::getHealthPointsCurrent() const
{
    return m_healthPointsCurrent;
}

void Character::setHealthPointsCurrent(int hpCurrent)
{
    if(hpCurrent == m_healthPointsCurrent)
        return;
    m_healthPointsCurrent= qBound(m_healthPointsMin, hpCurrent, m_healthPointsMax);
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
    m_healthPointsMin= hpMin;
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
    m_healthPointsMax= hpMax;
    emit maxHPChanged();
}

QString Character::getParentId() const
{
    if(nullptr != m_parentPerson)
    {
        return m_parentPerson->uuid();
    }
    return QString();
}

QHash<QString, QString> Character::getVariableDictionnary()
{
    QHash<QString, QString> variables;
    if(nullptr != m_sheet)
    {
#ifndef UNIT_TEST
        variables= m_sheet->getVariableDictionnary();
#endif
    }
    else if(!m_propertyList.isEmpty())
    {
        for(auto& property : m_propertyList)
        {
            if(property != nullptr)
                variables.insert(property->name(), property->value());
        }
    }
    return variables;
}

CharacterSheet* Character::getSheet() const
{
    return m_sheet;
}

void Character::setSheet(CharacterSheet* sheet)
{
    m_sheet= sheet;
}
void Character::setListOfCharacterState(QList<CharacterState*>* list)
{
    m_stateList= list;
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

void Character::addShape(const QString& name, const QString& path)
{
    auto shape= new CharacterShape();

    if(path.startsWith(QStringLiteral("image://rcs/")))
    {
#ifdef QT_QUICK_LIB
        auto key= path;
        key.replace(QStringLiteral("image://rcs/"), "");
        QSize size;
        QSize resquestedSize;
        auto img= m_imageProvider->requestPixmap(key, &size, resquestedSize);
        shape->setImage(img.toImage());
#endif
    }
    else if(path.startsWith("http"))
    {
#ifdef HAVE_QT_NETWORK
        QNetworkAccessManager* manager= new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, [shape, manager](QNetworkReply* reply) {
            QByteArray data= reply->readAll();
            QPixmap map;
            bool ok= map.loadFromData(data);
            if(ok)
                shape->setImage(map.toImage());
            if(manager != nullptr)
                delete manager;
        });

        manager->get(QNetworkRequest(QUrl(path)));
#endif
    }
    shape->setName(name);
    shape->setUri(path);
    m_shapeList.append(shape);
}

CharacterState* Character::getStateFromLabel(QString label)
{
    for(auto& state : *m_stateList)
    {
        if(state->label() == label)
        {
            return state;
        }
    }
    return nullptr;
}

CharacterState* Character::getStateFromIndex(int i)
{
    if(nullptr == m_stateList)
        return nullptr;

    if(m_stateList->empty())
        return nullptr;
    if(m_stateList->size() > i)
    {
        return m_stateList->at(i);
    }
    return nullptr;
}

/*QString Character::read(NetworkMessageReader& msg)
{
    if(!msg.isValid())
        return {};
    QString parentId= msg.string8();
    m_uuid= msg.string8();
    setName(msg.string16());
    setStateId(msg.string16());
    setNpc(static_cast<bool>(msg.uint8()));
    setNumber(msg.int32());
    setColor(QColor(msg.rgb()));
    setHealthPointsCurrent(msg.int32());
    setHealthPointsMin(msg.int32());
    setHealthPointsMax(msg.int32());
    setInitiativeScore(msg.int32());
    m_initiativeRoll.setCommand(msg.string32());
    setDistancePerTurn(msg.real());
    m_hasInitScore= static_cast<bool>(msg.uint8());

    bool hasAvatar= static_cast<bool>(msg.uint8());

    if(hasAvatar)
    {
        auto avatar= QImage::fromData(msg.byteArray32());
        setAvatar(avatar);
    }

    return parentId;
}*/

Player* Character::getParentPlayer() const
{
    return dynamic_cast<Player*>(m_parentPerson);
}

int Character::number() const
{
    return m_number;
}

void Character::setNumber(int n)
{
    m_number= n;
}
QColor Character::getLifeColor() const
{
    return m_lifeColor;
}
void Character::setLifeColor(QColor color)
{
    if(m_lifeColor == color)
        return;

    m_lifeColor= color;
    emit lifeColorChanged();
}

void Character::setInitCommand(const QString& init)
{
    if(m_initiativeRoll.command() == init)
        return;

    m_initiativeRoll.setCommand(init);
    emit initCommandChanged();
}
QString Character::initCommand() const
{
    return m_initiativeRoll.command();
}

bool Character::isNpc() const
{
    return m_isNpc;
}
void Character::setStateId(const QString& stateId)
{
    if(stateId == m_stateId)
        return;
    m_stateId= stateId;
    emit stateIdChanged();
}

QString Character::currentStateLabel() const
{
    auto it= std::find_if(m_stateList->begin(), m_stateList->end(), [this](const CharacterState* state) {
        if(nullptr == state)
            return false;
        return state->id() == m_stateId;
    });

    if(it == m_stateList->end())
        return {};
    return (*it)->label();
}

QImage Character::currentStateImage() const
{
    if(!m_stateList)
        return {};

    auto it= std::find_if(m_stateList->begin(), m_stateList->end(), [this](const CharacterState* state) {
        if(nullptr == state)
            return false;
        return state->id() == m_stateId;
    });
    if(it == m_stateList->end())
        return {};
    return QImage((*it)->imagePath());
}

bool Character::hasInitScore() const
{
    return m_hasInitScore;
}

void Character::setHasInitiative(bool b)
{
    if(m_hasInitScore == b)
        return;
    m_hasInitScore= b;
    emit hasInitScoreChanged();
}

QString Character::stateId() const
{
    return m_stateId;
}
/*void Character::writeData(QDataStream& out) const
{
    out << m_uuid;
    out << m_name;
    out << m_stateId;
    out << m_isNpc;
    out << m_number;
    out << m_color;
    out << m_avatar;
}
void Character::readData(QDataStream& in)
{
    in >> m_uuid;
    in >> m_name;
    in >> m_stateId;
    in >> m_isNpc;
    in >> m_number;
    in >> m_color;
    in >> m_avatar;
}*/

QList<CharacterState*>* Character::getCharacterStateList()
{
    return m_stateList;
}
void Character::setNpc(bool b)
{
    if(m_isNpc == b)
        return;

    m_isNpc= b;
    emit npcChanged();
}
/*void Character::write(QDataStream& out, bool tag, bool) const
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
    if(nullptr != m_parentPerson)
        out << m_parentPerson->uuid();
    else
        out << QString();

    out << static_cast<int>(m_checkState);

    QByteArray array;
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    m_avatar.save(&buffer, "PNG");
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
    m_checkState= static_cast<Qt::CheckState>(checkState);
    QByteArray array;
    in >> array;
    m_avatar= QImage::fromData(array);
}*/

void Character::setCurrentShape(int index)
{
    if(index < 0)
    {
        setCurrentShape(nullptr);
        return;
    }

    if(qBound(0, index, m_shapeList.size()) != index)
        return;

    auto shape= m_shapeList[index];
    setCurrentShape(shape);
}

void Character::setCurrentShape(CharacterShape* shape)
{
    if(shape == m_currentShape)
        return;
    m_currentShape= shape;
    emit avatarChanged();
}

void Character::addShape(CharacterShape* shape)
{
    m_shapeList.append(shape);
    // TODO emit signal ?
}

void Character::addAction(CharacterAction* action)
{
    m_actionList.append(action);
    // TODO emit signal ?
}

void Character::addProperty(CharacterProperty* property)
{
    m_propertyList.append(property);
    // TODO emit signal ?
}

QByteArray Character::avatar() const
{
    if(nullptr == m_currentShape)
        return Person::avatar();
    // else
    //        return IOHelper::imageToData(m_currentShape->image());
    // TODO change API of shapes
    return {};
}

CharacterShape* Character::currentShape() const
{
    return m_currentShape;
}
