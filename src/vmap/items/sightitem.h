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
#include "characteritem.h"

class Vision : public QObject
{
    Q_OBJECT
public:
    enum SHAPE {DISK,ANGLE};
    Vision();

    void setAngle(qreal);
    void setRadius(qreal);
    void setPosition(QPointF& p);
    void setShape(Vision::SHAPE s);
    void setCharacterItem(CharacterItem* item);

    qreal getAngle();
    qreal getRadius();
    const QPointF& getPos();
    Vision::SHAPE getShape();
    CharacterItem* getCharacterItem();

public slots:
    void updatePosition();

private:
    Vision::SHAPE m_shape;
    QPointF m_pos;
    qreal m_radius;
    qreal m_angle;
    CharacterItem* m_character;
};

class SightItem : public VisualItem
{
    Q_OBJECT
public:
    SightItem(QMap<QString,VisualItem*>* characterItemMap);
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
    /**
     * @brief getType
     * @return
     */
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
    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId,QPointF& pos) ;
    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem() ;
    /**
     * @brief getItemCopy
     * @return
     */
    virtual VisualItem* getItemCopy() ;
    /**
     * @brief boundingRect
     * @return
     */
    virtual QRectF boundingRect() const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

    void setDefaultShape(Vision::SHAPE shape);
    void setDefaultRaduis(qreal rad);
    void setDefaultAngle(qreal rad);

    void updateChildPosition();

    void createActions();
    void addActionContextMenu(QMenu* menu);
    /**
     * @brief setColor
     */
    void setColor(QColor& color);
    /**
     * @brief insertVision
     */
    void insertVision(CharacterItem* item);

public slots:
    void moveVision(QString id, QPointF& pos);

private slots:
    void computeGradiants();

private:
    Vision::SHAPE m_defaultShape;
    qreal m_defaultRadius;
    qreal m_defaultAngle;


    QAction* m_diskShape;
    QAction* m_angleShape;

    QMap<QString,Vision*> m_visionMap;
    QMap<QString,VisualItem*>* m_characterItemMap;
    QColor m_bgColor;
};

#endif // SIGHTITEM_H
