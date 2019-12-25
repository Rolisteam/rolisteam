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
#ifndef CHARACTERITEMCONTROLLER_H
#define CHARACTERITEMCONTROLLER_H

#include <QColor>
#include <QObject>
#include <QPixmap>
#include <QRectF>

#include "visualitemcontroller.h"

class Character;

namespace vmap
{

class CharacterItemController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(qreal side READ side WRITE setSide NOTIFY sideChanged)
    Q_PROPERTY(QColor stateColor READ stateColor WRITE setStateColor NOTIFY stateColorChanged)
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(bool playableCharacter READ playableCharacter WRITE setPlayableCharacter NOTIFY playableCharacterChanged)
    Q_PROPERTY(QRectF thumnailRect READ thumnailRect NOTIFY thumnailRectChanged)
    Q_PROPERTY(bool vision READ vision WRITE setVision NOTIFY visionChanged)
    Q_PROPERTY(VisionMode visionMode READ visionMode WRITE setVisionMode NOTIFY visionModeChanged)
    Q_PROPERTY(QRectF textRect READ textRect WRITE setTextRect NOTIFY textRectChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool hasAvatar READ hasAvatar NOTIFY hasAvatarChanged)
    Q_PROPERTY(QPixmap avatar READ avatar NOTIFY avatarChanged)
    Q_PROPERTY(QColor color READ color NOTIFY colorChanged)
public:
    enum class VisionMode
    {
        Disk,
        Cone
    };
    Q_ENUM(VisionMode)

    CharacterItemController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                            QObject* parent= nullptr);

    void aboutToBeRemoved() override;
    void endGeometryChange() override;
    void setCorner(const QPointF& move, int corner) override;

    qreal side() const;
    QColor stateColor() const;
    int number() const;
    bool playableCharacter() const;
    bool vision() const;
    QRectF thumnailRect() const;
    CharacterItemController::VisionMode visionMode() const;
    QRectF textRect() const;
    QString text() const;
    bool hasAvatar() const;
    QPixmap avatar() const;
    QColor color() const;

public slots:
    void setSide(qreal side);
    void setStateColor(QColor stateColor);
    void setNumber(int number);
    void setPlayableCharacter(bool playableCharacter);
    void setVision(bool vision);
    void setVisionMode(CharacterItemController::VisionMode visionMode);
    void setTextRect(QRectF textRect);
    void setText(QString text);

signals:
    void sideChanged(qreal side);
    void stateColorChanged(QColor stateColor);
    void numberChanged(int number);
    void playableCharacterChanged(bool playableCharacter);
    void thumnailRectChanged(QRectF name);
    void visionChanged(bool vision);
    void visionModeChanged(VisionMode visionMode);
    void textRectChanged(QRectF textRect);
    void textChanged(QString text);

    void hasAvatarChanged(bool hasAvatar);

    void avatarChanged(QPixmap avatar);

    void colorChanged(QColor color);

private:
    QPointer<Character> m_character;
    qreal m_side;
    QColor m_stateColor;
    int m_number;
    bool m_playableCharacter;
    int m_name;
    bool m_vision;
    VisionMode m_visionMode;
    QRectF m_textRect;
    QString m_text;
    bool m_hasAvatar;
    QPixmap m_avatar;
    QColor m_color;
};
} // namespace vmap

#endif // CHARACTERITEMCONTROLLER_H
