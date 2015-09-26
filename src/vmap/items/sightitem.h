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

#ifndef SIGHTITEM_H
#define SIGHTITEM_H

#include <QGradient>
#include <QConicalGradient>
#include <QRadialGradient>
#include "visualitem.h"

class SightItem : public VisualItem
{
public:
    enum SHAPE {DISK,ANGLE};
    SightItem();
    virtual ~SightItem();


    /**
     * @brief setNewEnd
     * @param nend
     */
    virtual void setNewEnd(QPointF& nend);
    /**
     * @brief writeData
     * @param out
     */
    virtual void writeData(QDataStream& out) const;
    /**
     * @brief readData
     * @param in
     */
    virtual void readData(QDataStream& in);
    virtual VisualItem::ItemType getType();
    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg);
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg);
    virtual void setGeometryPoint(qreal pointId,QPointF& pos) ;
    virtual void initChildPointItem() ;
    virtual VisualItem* getItemCopy() ;
    virtual QRectF boundingRect() const;

    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    void setShape(SHAPE shape);
    void setRaduis(qreal rad);
    void setAngle(qreal rad);

    void createActions();
    void addActionContextMenu(QMenu* menu);

private slots:
    void computeGradiants();

private:
    SHAPE m_shape;
    qreal m_radius;
    qreal m_angle;


    QAction* m_diskShape;
    QAction* m_angleShape;

    QGradient* m_gradient;
    QConicalGradient* m_conicalGradient;
    QRadialGradient* m_radialGradient;
};

#endif // SIGHTITEM_H
