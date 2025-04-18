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
#include <QFont>
#include <QObject>
#include <QPainterPath>
#include <QPixmap>
#include <QRectF>
#include <memory>

#include "data/character.h"
#include "data/charactervision.h"
#include "visualitemcontroller.h"
#include "worker/characterfinder.h"
#include <core_global.h>

class Character;
class CharacterVision;
namespace vmap
{
class CORE_EXPORT CharacterItemController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(Character* character READ character NOTIFY characterChanged)
    Q_PROPERTY(qreal side READ side WRITE setSide NOTIFY sideChanged)
    Q_PROPERTY(QColor stateColor READ stateColor WRITE setStateColor NOTIFY stateColorChanged)
    Q_PROPERTY(QString stateId READ stateId WRITE setStateId NOTIFY stateIdChanged)
    Q_PROPERTY(QImage stateImage READ stateImage NOTIFY stateImageChanged)
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(bool playableCharacter READ playableCharacter WRITE setPlayableCharacter NOTIFY playableCharacterChanged)
    Q_PROPERTY(QRectF thumnailRect READ thumnailRect WRITE setThumnailRect NOTIFY thumnailRectChanged)
    Q_PROPERTY(CharacterVision::SHAPE visionShape READ visionShape WRITE setVisionShape NOTIFY visionShapeChanged)
    Q_PROPERTY(QRectF textRect READ textRect WRITE setTextRect NOTIFY textRectChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(bool hasAvatar READ hasAvatar NOTIFY hasAvatarChanged)
    Q_PROPERTY(QImage* avatar READ avatar NOTIFY avatarChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(CharacterVision* vision READ vision NOTIFY visionChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(bool healthStatusVisible READ healthStatusVisible NOTIFY healthStatusVisibleChanged)
    Q_PROPERTY(QList<CharacterAction*> actions READ actionList CONSTANT)
    Q_PROPERTY(QList<CharacterShape*> shapes READ shapeList CONSTANT)
    Q_PROPERTY(QList<CharacterProperty*> properties READ propertiesList CONSTANT)
    Q_PROPERTY(QString characterId READ characterId WRITE setCharacterId NOTIFY characterIdChanged FINAL)
    Q_PROPERTY(
        bool waitingCharacter READ waitingCharacter WRITE setWaitingCharacter NOTIFY waitingCharacterChanged FINAL)
public:
    enum AdditionControls
    {
        DirectionHandle= 4,
        AngleHandle
    };
    enum class ChangedProperty
    {
        NONE= 0x0,
        SIDE= 0x1,
        RECT= 0x2
    };
    Q_DECLARE_FLAGS(Changes, ChangedProperty)
    Q_FLAG(Changes)
    CharacterItemController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                            QObject* parent= nullptr);

    qreal side() const;
    // state
    QColor stateColor() const;
    QString stateId() const;
    QImage stateImage() const;

    int number() const;
    bool playableCharacter() const;
    QRectF thumnailRect() const;
    CharacterVision::SHAPE visionShape() const;
    QRectF textRect() const;
    QString text() const;
    bool hasAvatar() const;
    QImage* avatar() const;
    QFont font() const;
    QPainterPath shape() const;
    Character* character() const;
    CharacterVision* vision() const;
    qreal radius() const;
    void aboutToBeRemoved() override;
    void endGeometryChange() override;
    void setCorner(const QPointF& move, int corner,
                   Core::TransformType transformType= Core::TransformType::NoTransform) override;
    QColor color() const override;
    QRectF rect() const override;

    const QList<CharacterAction*> actionList() const;
    const QList<CharacterShape*> shapeList() const;
    const QList<CharacterProperty*> propertiesList() const;

    // accessor to Map properties
    bool healthStatusVisible() const;

    QString characterId() const;
    void setCharacterId(const QString& newCharacterId);

    bool waitingCharacter() const;
    void setWaitingCharacter(bool newWaitingCharacter);

    void updateCharacter();

public slots:
    void setSide(qreal side);
    void setStateColor(QColor stateColor);
    void setNumber(int number);
    void setPlayableCharacter(bool playableCharacter);
    void setVisionShape(CharacterVision::SHAPE visionShape);
    void setTextRect(QRectF textRect);
    void setFont(const QFont& font);
    void setStateId(const QString& id);
    void setRadius(qreal r);
    void setThumnailRect(const QRectF& rect);

    void runInit();
    void cleanInit();
    void setShape(int index);
    void cleanShape();
    void runCommand(int index);
    void findCharacter();

signals:
    void sideChanged(qreal side);
    void stateColorChanged(QColor stateColor);
    void numberChanged(int number);
    void playableCharacterChanged();
    void thumnailRectChanged(QRectF name);
    void visionChanged(bool vision);
    void visionShapeChanged(CharacterVision::SHAPE visionShape);
    void textRectChanged(QRectF textRect);
    void textChanged(QString text);
    void hasAvatarChanged(bool hasAvatar);
    void avatarChanged();
    void fontChanged(QFont font);
    void radiusChanged(qreal radius);
    void stateIdChanged(QString s);
    void stateImageChanged(QString si);
    void healthStatusVisibleChanged(bool);
    void rectEditFinished();
    void sideEdited();
    void characterChanged();
    void characterIdChanged();
    void waitingCharacterChanged();

private:
    void refreshTextRect();
    void computeThumbnail();
    void setRect(const QRectF& rect);
    void computeEditable() override;
    void setCharacter(Character* charac);

private:
    QPointer<Character> m_character;
    QPointer<VectorialMapController> m_mapCtrl;
    std::unique_ptr<QImage> m_thumb;
    std::unique_ptr<CharacterVision> m_vision;
    qreal m_side= 64;
    QColor m_stateColor;
    int m_number= 0;
    QRectF m_textRect;
    QFont m_font;
    QRectF m_rect= {0, 0, 64, 64};
    qreal m_radius= 10.0;

    Changes m_changes{ChangedProperty::NONE};
    CharacterFinder m_finder;
    QString m_characterId;
    bool m_waitingCharacter{false};
    std::unique_ptr<Character> m_characterStored;
};
} // namespace vmap

#endif // CHARACTERITEMCONTROLLER_H
