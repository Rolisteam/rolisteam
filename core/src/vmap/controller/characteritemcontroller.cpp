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

#include "controller/view_controller/vectorialmapcontroller.h"

#include "data/character.h"
#include "data/characterstate.h"

namespace vmap
{
CharacterItemController::CharacterItemController(const std::map<QString, QVariant>& params,
                                                 VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(ctrl, parent)
{
    if(params.end() != params.find("character"))
        m_character= params.at(QStringLiteral("character")).value<Character*>();

    if(params.end() != params.find("side"))
        setSide(params.at(QStringLiteral("side")).toDouble());

    if(params.end() != params.find("position"))
        setPos(params.at(QStringLiteral("position")).toPointF());

    if(params.end() != params.find("tool"))
        m_playableCharacter= (params.at(QStringLiteral("tool")).value<Core::SelectableTool>()
                              == Core::SelectableTool::PlayableCharacter);

    if(params.end() != params.find("position"))
        setPos(params.at(QStringLiteral("position")).toPointF());
}

void CharacterItemController::aboutToBeRemoved() {}

void CharacterItemController::endGeometryChange() {}

void CharacterItemController::setCorner(const QPointF& move, int corner) {}

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
    return m_playableCharacter;
}

QRectF CharacterItemController::thumnailRect() const
{
    return QRectF(0.0, 0.0, m_side, m_side);
}

bool CharacterItemController::vision() const
{
    return m_vision;
}

CharacterItemController::VisionMode CharacterItemController::visionMode() const
{
    return m_visionMode;
}

QRectF CharacterItemController::textRect() const
{
    return m_textRect;
}

QString CharacterItemController::text() const
{
    QStringList label;
    auto name= m_playableCharacter ? QStringLiteral("PC") : QStringLiteral("NPC");
    if(m_character)
        name= m_character->name();

    if((!m_playableCharacter && m_ctrl->npcNameVisible()) || (m_playableCharacter && m_ctrl->pcNameVisible()))
        label << name;

    if(m_ctrl->npcNumberVisible() && !m_playableCharacter)
    {
        auto number= m_playableCharacter ? QStringLiteral("") : QString::number(m_number);
        label << number;
    }

    if(m_ctrl->stateLabelVisible())
        label << m_character->currentStateLabel();

    return label.join(QStringLiteral(" - "));
}

bool CharacterItemController::hasAvatar() const
{
    return m_hasAvatar;
}

QPixmap CharacterItemController::avatar() const
{
    return m_avatar;
}

QColor CharacterItemController::color() const
{
    return m_color;
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
    if(m_playableCharacter == playableCharacter)
        return;

    m_playableCharacter= playableCharacter;
    emit playableCharacterChanged(m_playableCharacter);
}

void CharacterItemController::setVision(bool vision)
{
    if(m_vision == vision)
        return;

    m_vision= vision;
    emit visionChanged(m_vision);
}

void CharacterItemController::setVisionMode(CharacterItemController::VisionMode visionMode)
{
    if(m_visionMode == visionMode)
        return;

    m_visionMode= visionMode;
    emit visionModeChanged(m_visionMode);
}

void CharacterItemController::setTextRect(QRectF textRect)
{
    if(m_textRect == textRect)
        return;

    m_textRect= textRect;
    emit textRectChanged(m_textRect);
}

void CharacterItemController::setText(QString text)
{
    if(m_text == text)
        return;

    m_text= text;
    emit textChanged(m_text);
}
} // namespace vmap
