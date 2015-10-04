/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
public:
    enum SHAPE {DISK,ANGLE};
    /**
             * @brief Vision
             */
    CharacterVision(QObject* parent = NULL);
    ~CharacterVision();
    /**
             * @brief setAngle
             */
    void setAngle(qreal);
    /**
             * @brief setRadius
             */
    void setRadius(qreal);
    /**
             * @brief setPosition
             * @param p
             */
    void setPosition(QPointF& p);
    /**
             * @brief setShape
             * @param s
             */
    void setShape(CharacterVision::SHAPE s);
    /**
             * @brief getAngle
             * @return
             */
    qreal getAngle();
    /**
             * @brief getRadius
             * @return
             */
    qreal getRadius();
    /**
             * @brief getPos
             * @return
             */
    const QPointF getPos();
    /**
             * @brief getShape
             * @return
             */
    CharacterVision::SHAPE getShape();
    /**
             * @brief setCornerPoint
             */
    void setCornerPoint(ChildPointItem*);
    /**
             * @brief getCornerPoint
             * @return
             */
    ChildPointItem* getCornerPoint();

    /**
     * @brief isVisible
     * @return
     */
    bool isVisible();
    /**
     * @brief setVisible
     */
    void setVisible(bool);

public slots:
    /**
             * @brief updatePosition
             */
    void updatePosition();
    /**
             * @brief showCorner
             */
    void showCorner(bool);

private:
    CharacterVision::SHAPE m_shape;
    QPointF m_pos;
    qreal m_radius;
    qreal m_angle;
    ChildPointItem* m_cornerPoint;
    bool m_visible;
};


#endif // CHARACTERVISION_H
