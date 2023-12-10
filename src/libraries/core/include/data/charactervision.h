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

#ifndef CHARACTERVISION_H
#define CHARACTERVISION_H

#include <QObject>
#include <QPainterPath>
#include <QPointF>

#include <network_global.h>

/**
 * @brief The Vision class
 */
class NETWORK_EXPORT CharacterVision : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(SHAPE shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(QPainterPath path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(qreal side READ side WRITE setSide NOTIFY sideChanged FINAL)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool cornerVisible READ cornerVisible WRITE setCornerVisible NOTIFY cornerVisibleChanged)
    Q_PROPERTY(bool removed READ removed WRITE setRemoved NOTIFY removedChanged FINAL)
public:
    enum SHAPE
    {
        DISK,
        ANGLE
    };
    Q_ENUM(SHAPE)

    enum class ChangedProperty {
        NONE=0x0,
        ANGLE= 0x1,
        ROTATION=0x2,
        POSITION=0x4,
        RADIUS=0x8,
        PATH=0xF
    };
    Q_DECLARE_FLAGS(Changes, ChangedProperty)
    Q_FLAG(Changes)

    CharacterVision(QObject* parent= nullptr);
    virtual ~CharacterVision();

    void setAngle(qreal);
    void setRadius(qreal);
    void setPosition(const QPointF& p);
    void setShape(CharacterVision::SHAPE s);
    void setVisible(bool);

    qreal angle() const;
    qreal radius() const;
    QPointF position() const;
    CharacterVision::SHAPE shape() const;
    bool visible() const;
    bool cornerVisible() const;

    QPainterPath path() const;
    void setPath(QPainterPath newPath);

    qreal rotation() const;
    void setRotation(qreal newRotation);

    qreal side() const;
    void setSide(qreal newSide);

    bool removed() const;
    void setRemoved(bool newRemoved);

public slots:
    void updatePosition();
    void setCornerVisible(bool b);
    void endOfGeometryChanges();

signals:
    void radiusChanged(qreal);
    void angleChanged(qreal);
    void positionChanged(QPointF);
    void shapeChanged(SHAPE);
    void visibleChanged(bool);
    void cornerVisibleChanged(bool);
    void pathChanged();
    void rotationChanged();
    void sideChanged();
    void removedChanged();

    //edited
    void radiusEdited();
    void angleEdited();
    void rotationEdited();
    void positionEdited();
    void pathEdited();

private:
    CharacterVision::SHAPE m_shape= ANGLE;
    QPointF m_pos;
    qreal m_angle= 120;
    bool m_cornerVisible= false;
    bool m_visible= false;
    qreal m_radius= 50;
    QPainterPath m_path;
    qreal m_rotation= 0.;
    Changes m_changes{ChangedProperty::NONE};
    qreal m_side;
    bool m_removed{false};
};

#endif // CHARACTERVISION_H
