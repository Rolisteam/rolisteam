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
#include "characteritemcontroller.h"

#include <QDebug>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/character.h"
#include "data/characterstate.h"

#define MARGING 1

namespace vmap
{
CharacterItemController::CharacterItemController(const std::map<QString, QVariant>& params,
                                                 VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(ctrl, parent)
{
    m_tool= Core::SelectableTool::NonPlayableCharacter;
    if(params.end() != params.find("character"))
    {
        m_character= params.at(QStringLiteral("character")).value<Character*>();
        connect(m_character, &Character::npcChanged, this, &CharacterItemController::refreshTextRect);
        connect(m_character, &Character::colorChanged, this, [this]() { emit colorChanged(m_character->getColor()); });
        connect(m_character, &Character::avatarChanged, this, &CharacterItemController::computeThumbnail);
        connect(ctrl, &VectorialMapController::npcNameVisibleChanged, this, &CharacterItemController::refreshTextRect);
        connect(ctrl, &VectorialMapController::pcNameVisibleChanged, this, &CharacterItemController::refreshTextRect);
        connect(ctrl, &VectorialMapController::npcNumberVisibleChanged, this,
                &CharacterItemController::refreshTextRect);
        connect(ctrl, &VectorialMapController::stateLabelVisibleChanged, this,
                &CharacterItemController::refreshTextRect);

        if(!m_character->isNpc())
        {
            m_tool= Core::SelectableTool::PlayableCharacter;
            m_vision.reset(new CharacterVision());
        }
    }
    Q_ASSERT(m_character);

    if(params.end() != params.find("side"))
    {
        setSide(params.at(QStringLiteral("side")).toDouble());
        m_rect= QRectF(0.0, 0.0, m_side, m_side);
    }

    if(params.end() != params.find("position"))
        setPos(params.at(QStringLiteral("position")).toPointF());

    refreshTextRect();
    computeThumbnail();
}

void CharacterItemController::aboutToBeRemoved() {}

void CharacterItemController::endGeometryChange() {}

VisualItemController::ItemType CharacterItemController::itemType() const
{
    return VisualItemController::CHARACTER;
}

void CharacterItemController::setCorner(const QPointF& move, int corner)
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
    return m_vision->getShape();
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
                  path.addRoundedRect(0, 0, m_side, m_side, m_side / m_radius, m_side / m_radius);
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
    if(m_vision->getShape() == visionShape)
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

void CharacterItemController::computeThumbnail()
{
    int diam= static_cast<int>(m_side);
    m_thumb.reset(new QImage(diam, diam, QImage::Format_ARGB32));
    m_thumb->fill(Qt::transparent);
    QPainter painter(m_thumb.get());
    QBrush brush;
    if(m_character->getAvatar().isNull())
    {
        painter.setPen(m_character->getColor());
        brush.setColor(m_character->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        painter.setPen(Qt::NoPen);
        QImage img= m_character->getAvatar();
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
} // namespace vmap
