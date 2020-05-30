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

#ifndef CHARACTERVISION_H
#define CHARACTERVISION_H

#include "vmap/items/visualitem.h"
//#include "vmap/items/characteritem.h"

/**
 * @brief The Vision class
 */
class CharacterVision : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(SHAPE shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

public:
    enum SHAPE
    {
        DISK,
        ANGLE
    };
    Q_ENUM(SHAPE)
    CharacterVision(QObject* parent= nullptr);
    virtual ~CharacterVision();

    void setAngle(qreal);
    void setRadius(qreal);
    void setPosition(QPointF& p);
    void setShape(CharacterVision::SHAPE s);
    void setVisible(bool);

    qreal angle() const;
    qreal radius() const;
    QPointF position() const;
    CharacterVision::SHAPE shape() const;
    bool visible() const;

    void setCornerPoint(ChildPointItem*);
    ChildPointItem* getCornerPoint();
public slots:
    void updatePosition();
    void showCorner(bool);

signals:
    void radiusChanged(qreal);
    void angleChanged(qreal);
    void positionChanged(QPointF);
    void shapeChanged(SHAPE);
    void visibleChanged(bool);

private:
    CharacterVision::SHAPE m_shape= ANGLE;
    QPointF m_pos;
    qreal m_angle= 50;
    ChildPointItem* m_cornerPoint= nullptr;
    bool m_visible= false;
    qreal m_radius= 50;
};

#endif // CHARACTERVISION_H
