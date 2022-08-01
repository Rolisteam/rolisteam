/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "controller/item_controllers/characteritemcontroller.h"

#include <QDebug>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/characterstate.h"
#include "utils/iohelper.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"
#include "worker/vectorialmapmessagehelper.h"

#include "undoCmd/rollinitcommand.h"

#define MARGING 1

namespace vmap
{
CharacterItemController::CharacterItemController(const std::map<QString, QVariant>& params,
                                                 VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::CHARACTER, params, ctrl, parent)
{
    m_tool= Core::SelectableTool::NonPlayableCharacter;
    if(params.end() != params.find(Core::vmapkeys::KEY_CHARACTER))
    {
        m_character= params.at(Core::vmapkeys::KEY_CHARACTER).value<Character*>();
    }
    else
    {
        m_character= new Character();
        VectorialMapMessageHelper::fetchCharacter(params, m_character);
    }
    connect(m_character, &Character::stateIdChanged, this, &CharacterItemController::stateIdChanged);
    connect(m_character, &Character::npcChanged, this, &CharacterItemController::playableCharacterChanged);
    connect(m_character, &Character::npcChanged, this, &CharacterItemController::refreshTextRect);
    connect(m_character, &Character::colorChanged, this, [this]() { emit colorChanged(m_character->getColor()); });
    connect(m_character, &Character::avatarChanged, this, &CharacterItemController::computeThumbnail);
    connect(m_character, &Character::hasInitScoreChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::initCommandChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::initiativeChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::currentHealthPointsChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::maxHPChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::minHPChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::distancePerTurnChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::lifeColorChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::nameChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::colorChanged, this, &CharacterItemController::setModified);
    connect(m_character, &Character::avatarChanged, this, &CharacterItemController::setModified);

    connect(ctrl, &VectorialMapController::npcNameVisibleChanged, this, &CharacterItemController::refreshTextRect);
    connect(ctrl, &VectorialMapController::pcNameVisibleChanged, this, &CharacterItemController::refreshTextRect);
    connect(ctrl, &VectorialMapController::npcNumberVisibleChanged, this, &CharacterItemController::refreshTextRect);
    connect(ctrl, &VectorialMapController::stateLabelVisibleChanged, this, &CharacterItemController::refreshTextRect);
    connect(ctrl, &VectorialMapController::healthBarVisibleChanged, this,
            &CharacterItemController::healthStatusVisibleChanged);

    if(!m_character->isNpc())
    {
        m_tool= Core::SelectableTool::PlayableCharacter;
        m_vision.reset(new CharacterVision());
        VectorialMapMessageHelper::fetchCharacterVision(params, m_vision.get());
        ctrl->addVision(m_vision.get());

        auto updatePos= [this]() {
            auto rect= thumnailRect();
            auto p= pos() + rect.center(); // ;

            m_vision->setPosition(p);
        };

        connect(this, &CharacterItemController::posChanged, m_vision.get(), updatePos);
        connect(this, &CharacterItemController::sideChanged, m_vision.get(), updatePos);
        connect(this, &CharacterItemController::rotationChanged, m_vision.get(), &CharacterVision::setRotation);
        updatePos();
    }

    {
        namespace hu= helper::utils;
        namespace cv= Core::vmapkeys;
        using std::placeholders::_1;

        hu::setParamIfAny<qreal>(cv::KEY_SIDE, params, std::bind(&CharacterItemController::setSide, this, _1));

        m_rect= QRectF(0.0, 0.0, m_side, m_side);
    }

    refreshTextRect();
    computeThumbnail();

    connect(this, &CharacterItemController::sideChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::stateColorChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::stateIdChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::stateImageChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::numberChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::playableCharacterChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::thumnailRectChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::visionShapeChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::textRectChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::textChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::hasAvatarChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::avatarChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::fontChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::visionChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::radiusChanged, this, [this] { setModified(); });
    connect(this, &CharacterItemController::healthStatusVisibleChanged, this, [this] { setModified(); });
}

void CharacterItemController::aboutToBeRemoved()
{
    emit removeItem();
}

void CharacterItemController::endGeometryChange() {}

void CharacterItemController::setCorner(const QPointF& move, int corner,
                                        Core::TransformType tt)
{
    if(move.isNull())
        return;

    auto rect= thumnailRect();
    qreal x2= rect.right();
    qreal y2= rect.bottom();
    qreal x= rect.x();
    qreal y= rect.y();
    qreal motion= 0.0;

    if(move.x() > 0 || move.y() > 0)
        motion= std::max(move.x(), move.y());
    else
        motion= std::min(move.x(), move.y());

    switch(corner)
    {
    case TopLeft:
        x+= motion;
        y+= motion;
        break;
    case TopRight:
        if(qFuzzyCompare(move.x(), motion))
        {
            x2+= motion;
            y-= motion;
        }
        else
        {
            x2-= motion;
            y+= motion;
        }
        break;
    case BottomRight:
        x2+= motion;
        y2+= motion;
        break;
    case BottomLeft:
        if(qFuzzyCompare(move.x(), motion))
        {
            x+= motion;
            y2-= motion;
        }
        else
        {
            x-= motion;
            y2+= motion;
        }

        break;
    case DirectionHandle:
    {
        auto xmove= move.x();
        auto r= m_vision->radius();
        if((xmove + r) < (m_rect.width() / 2))
        {
            xmove= (m_rect.width() / 2) - r;
        }
        m_vision->setRadius(xmove + m_vision->radius());
    }
    break;
    case AngleHandle:
    {
        m_vision->setAngle(std::min(std::max(m_vision->angle() + (move.y() * -1), 0.), 360.));
    }
    break;
    }

    rect.setCoords(x, y, x2, y2);
    if(!rect.isValid())
        rect= rect.normalized();
    setRect(rect);
    setSide(rect.width());
    refreshTextRect();
    computeThumbnail();
}

qreal CharacterItemController::side() const
{
    return m_side;
}

QColor CharacterItemController::stateColor() const
{
    return m_stateColor;
}

int CharacterItemController::number() const
{
    return m_number;
}

bool CharacterItemController::playableCharacter() const
{
    if(!m_character)
        return false;
    return !m_character->isNpc();
}

QRectF CharacterItemController::thumnailRect() const
{
    return m_rect;
}

void CharacterItemController::setRect(const QRectF& rect)
{
    if(rect == m_rect)
        return;

    m_rect= rect;
    emit thumnailRectChanged(m_rect);
}

CharacterVision::SHAPE CharacterItemController::visionShape() const
{
    return m_vision ? m_vision->shape() : CharacterVision::ANGLE;
}

QRectF CharacterItemController::textRect() const
{
    return m_textRect;
}

QString CharacterItemController::text() const
{
    if(!m_character)
        return {};

    QStringList label;
    auto playableCharacter= !m_character->isNpc();
    auto name= playableCharacter ? QStringLiteral("PC") : QStringLiteral("NPC");
    if(m_character)
        name= m_character->name();

    if((!playableCharacter && m_ctrl->npcNameVisible()) || (playableCharacter && m_ctrl->pcNameVisible()))
        label << name;

    if(m_ctrl->npcNumberVisible() && !playableCharacter)
    {
        auto number= playableCharacter ? QStringLiteral("") : QString::number(m_number);
        label << number;
    }

    if(m_ctrl->stateLabelVisible())
        label << m_character->currentStateLabel();

    return label.join(QStringLiteral(" - "));
}

bool CharacterItemController::hasAvatar() const
{
    if(nullptr == m_character)
        return false;

    return m_character->hasAvatar();
}

QImage* CharacterItemController::avatar() const
{
    return m_thumb.get();
}

QColor CharacterItemController::color() const
{
    if(nullptr == m_character)
        return {};
    return m_character->getColor();
}

QFont CharacterItemController::font() const
{
    return m_font;
}

QPainterPath CharacterItemController::shape() const
{
    QPainterPath path;
    path.moveTo(0, 0);
    hasAvatar() ? path.addEllipse(thumnailRect().united(textRect())) :
                  path.addRoundedRect(m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height(), m_side / m_radius,
                                      m_side / m_radius);
    path.addRect(textRect());
    return path;
}

CharacterVision* CharacterItemController::vision() const
{
    return m_vision.get();
}

qreal CharacterItemController::radius() const
{
    return m_radius;
}

QRectF CharacterItemController::rect() const
{
    return thumnailRect();
}

QString CharacterItemController::stateId() const
{
    return m_character ? m_character->stateId() : QString();
}

QImage CharacterItemController::stateImage() const
{
    return m_character ? m_character->currentStateImage() : QImage();
}

bool CharacterItemController::healthStatusVisible() const
{
    return m_ctrl->healthBarVisible();
}

void CharacterItemController::refreshTextRect()
{
    auto subtext= text();
    QFontMetrics metrics(m_font);
    QRectF rect(thumnailRect().center().x() - ((metrics.boundingRect(subtext).width() + MARGING) / 2),
                thumnailRect().bottom(), metrics.boundingRect(subtext).width() + MARGING + MARGING, metrics.height());
    setTextRect(rect);
}

void CharacterItemController::setSide(qreal side)
{
    if(qFuzzyCompare(m_side, side))
        return;

    m_side= side;
    emit sideChanged(m_side);
}

void CharacterItemController::setStateColor(QColor stateColor)
{
    if(m_stateColor == stateColor)
        return;

    m_stateColor= stateColor;
    emit stateColorChanged(m_stateColor);
}

void CharacterItemController::setNumber(int number)
{
    if(m_number == number)
        return;

    m_number= number;
    emit numberChanged(m_number);
}

void CharacterItemController::setPlayableCharacter(bool playableCharacter)
{
    if(!m_character)
        return;
    m_character->setNpc(!playableCharacter);
}

void CharacterItemController::setVisionShape(CharacterVision::SHAPE visionShape)
{
    if(!m_vision)
        return;
    if(m_vision->shape() == visionShape)
        return;

    m_vision->setShape(visionShape);
    emit visionShapeChanged(visionShape);
}

void CharacterItemController::setTextRect(QRectF textRect)
{
    if(m_textRect == textRect)
        return;

    m_textRect= textRect;
    emit textRectChanged(m_textRect);
}

void CharacterItemController::setFont(const QFont& font)
{
    if(m_font == font)
        return;
    m_font= font;
    emit fontChanged(m_font);
}

void CharacterItemController::setStateId(const QString& id)
{
    if(m_character)
        m_character->setStateId(id);
}

void CharacterItemController::setRadius(qreal r)
{
    if(qFuzzyCompare(r, m_radius))
        return;
    m_radius= r;
    emit radiusChanged(r);
}

void CharacterItemController::computeThumbnail()
{
    int diam= static_cast<int>(m_side);

    m_thumb.reset(new QImage(diam, diam, QImage::Format_ARGB32));
    m_thumb->fill(Qt::transparent);
    QPainter painter(m_thumb.get());
    QBrush brush;
    if(m_character->avatar().isNull())
    {
        painter.setPen(m_character->getColor());
        brush.setColor(m_character->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        painter.setPen(Qt::NoPen);
        QImage img= utils::IOHelper::dataToImage(m_character->avatar());
        brush.setTextureImage(img.scaled(diam, diam));
    }

    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, diam, diam, m_side / m_radius, m_side / m_radius);
    emit avatarChanged();
}

Character* CharacterItemController::character() const
{
    return m_character;
}
const QList<CharacterAction*> CharacterItemController::actionList() const
{
    if(m_character)
        return m_character->actionList();
    return {};
}
const QList<CharacterShape*> CharacterItemController::shapeList() const
{
    if(m_character)
        return m_character->shapeList();
    return {};
}
const QList<CharacterProperty*> CharacterItemController::propertiesList() const
{
    if(m_character)
        return m_character->propertiesList();
    return {};
}

void CharacterItemController::runInit()
{
    m_ctrl->rollInit({this});
}
void CharacterItemController::cleanInit()
{
    m_ctrl->cleanUpInit({this});
}
void CharacterItemController::setShape(int index)
{
    if(m_character)
        m_character->setCurrentShape(index);
}
void CharacterItemController::cleanShape()
{
    if(m_character)
        m_character->setCurrentShape(nullptr);
}

void CharacterItemController::runCommand(int index)
{
    if(!m_character)
        return;

    auto list= m_character->actionList();

    if(list.size() <= index)
        return;

    auto cmd= list[index];
    m_ctrl->runDiceCommand({this}, cmd->command());
}
} // namespace vmap
