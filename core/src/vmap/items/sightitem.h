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

#ifndef SIGHTITEM_H
#define SIGHTITEM_H

#include "characteritem.h"
#include "data/charactervision.h"
#include "visualitem.h"
#include <QConicalGradient>
#include <QGradient>
#include <QRadialGradient>

/*class FogSingularity
{
public:
    FogSingularity(QPolygonF* poly= nullptr, bool isAdding= false);
    const QPolygonF* getPolygon() const;

    bool isAdding() const;
    void fillMessage(NetworkMessageWriter* msg);
    void readItem(NetworkMessageReader* msg);
    void setPolygon(QPolygonF*);

private:
    QPolygonF* m_poly;
    bool m_adding;
};*/
namespace vmap
{
class SightController;
}
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
    explicit SightItem(vmap::SightController* ctrl, QMap<QString, CharacterItem*>* characterItemMap);
    /**
     * @brief ~SightItem
     */
    virtual ~SightItem();
    /**
     * @brief setNewEnd
     * @param nend
     */
    virtual void setNewEnd(const QPointF& nend);
    /**
     * @brief writeData
     * @param out
     */
    virtual void writeData(QDataStream& out) const override;
    /**
     * @brief readData
     * @param in
     */
    virtual void readData(QDataStream& in) override;
    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg) override;
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg) override;
    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId, QPointF& pos) override;
    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem() override;
    /**
     * @brief getItemCopy
     * @return
     */
    virtual VisualItem* getItemCopy() override;
    /**
     * @brief boundingRect
     * @return
     */
    virtual QRectF boundingRect() const override;
    void monitorView();
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
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
    void updateChildPosition() override;
    /**
     * @brief createActions
     */
    void createActions() override;
    /**
     * @brief setColor
     */
    void setColor(QColor& color);
    /**
     * @brief insertVision
     */
    // void insertVision(CharacterItem* item);
    /**
     * @brief addFogPolygon
     * @param a
     */
    

    void markDirty();
    // FogSingularity* addFogPolygon(QPolygonF* a, bool adding);
    virtual void updateItemFlags();
    virtual void endOfGeometryChange() override;
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
    // void removeVision(CharacterItem* item);

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QPointer<vmap::SightController> m_sightCtrl;
    CharacterVision::SHAPE m_defaultShape;
    qreal m_defaultAngle;
    qreal m_defaultRadius;
    QMap<QString, CharacterItem*>* m_characterItemMap;
};

#endif // SIGHTITEM_H
