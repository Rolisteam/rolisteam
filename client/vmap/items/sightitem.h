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

#ifndef SIGHTITEM_H
#define SIGHTITEM_H

#include <QGradient>
#include <QConicalGradient>
#include <QRadialGradient>
#include "visualitem.h"
#include "characteritem.h"
#include "data/charactervision.h"


class FogSingularity
{
public:
    FogSingularity(QPolygonF* poly = nullptr,bool isAdding = false);
    const QPolygonF* getPolygon() const;

    bool isAdding() const;
    void fillMessage(NetworkMessageWriter* msg);
    void readItem(NetworkMessageReader* msg);
    void setPolygon(QPolygonF* );
private:
    QPolygonF* m_poly;
    bool m_adding;
};

/**
 * @brief The SightItem class
 */
class SightItem : public VisualItem
{
    Q_OBJECT
public:
    /**
     * @brief SightItem
     * @param characterItemMap
     */
    SightItem(QMap<QString,CharacterItem*>* characterItemMap);
    /**
     * @brief ~SightItem
     */
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
    virtual VisualItem::ItemType getType() const;
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
    /**
     * @brief setDefaultShape
     * @param shape
     */
    void setDefaultShape(CharacterVision::SHAPE shape);
    /**
     * @brief setDefaultRadius
     * @param rad
     */
    void setDefaultRadius(qreal rad);
    /**
     * @brief setDefaultAngle
     * @param rad
     */
    void setDefaultAngle(qreal rad);
    /**
     * @brief updateChildPosition
     */
    void updateChildPosition();
    /**
     * @brief createActions
     */
    void createActions();
    /**
     * @brief addActionContextMenu
     * @param menu
     */
    void addActionContextMenu(QMenu* menu);
    /**
     * @brief setColor
     */
    void setColor(QColor& color);
    /**
     * @brief insertVision
     */
    void insertVision(CharacterItem* item);
    /**
     * @brief setVisible
     * @param visible
     */
    virtual void setVisible(bool visible);
    /**
     * @brief addFogPolygon
     * @param a
     */
    FogSingularity*  addFogPolygon(QPolygonF* a,bool adding);
    virtual void updateItemFlags();
public slots:
    /**
     * @brief moveVision
     * @param id
     * @param pos
     */
    void moveVision(qreal id, QPointF& pos);
    /**
     * @brief removeVision vision to the given character.
     * @param item
     */
    void removeVision(CharacterItem* item);
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void updateVeil();
private:
    CharacterVision::SHAPE m_defaultShape;
    qreal m_defaultAngle;
    qreal m_defaultRadius;
    QMap<QString,CharacterItem*>* m_characterItemMap;
    QColor m_bgColor;
    QImage m_image;
    qreal m_count;
    QList<FogSingularity*> m_fogHoleList;
    QPainterPath m_path;
    QRectF m_rectOfVeil;
};

#endif // SIGHTITEM_H
