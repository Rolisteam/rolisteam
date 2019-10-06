/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
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
#ifndef ELLIPSITEM_H
#define ELLIPSITEM_H
#include "visualitem.h"
/**
 * @brief displays an ellipse on maps.
 */
class EllipsItem : public VisualItem
{
public:
    EllipsItem();
    /**
     * @brief constructor with parameters
     * @param center first point clicked by the user
     * @param either the shape is filled or not
     * @param color used for drawing it.
     */
    EllipsItem(const QPointF& center, bool filled, int penSize, const QColor& penColor, QGraphicsItem* parent= nullptr);
    /**
     * @brief paint the ellipse at the correct position
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;

    virtual QPainterPath shape() const override;
    /**
     * @brief modifies the ellipse size and shape.
     */
    virtual void setNewEnd(QPointF& nend) override;

    /**
     * @brief gives the bounding rect of the ellipse
     */
    virtual QRectF boundingRect() const override;
    /**
     * @brief serialisation writing
     */
    virtual void writeData(QDataStream& out) const override;
    /**
     * @brief serialisation reading
     */
    virtual void readData(QDataStream& in) override;
    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType() const override;

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
    void setGeometryPoint(qreal pointId, QPointF& pos) override;
    virtual void initChildPointItem() override;
    virtual VisualItem* getItemCopy() override;

    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h) override;

    void setHoldSize(bool holdSize) override;

private:
    /**
     * @brief bounding rect
     */
    // QRectF m_rect;

    qreal m_rx;

    qreal m_ry;
    /**
     * @brief ellipse center
     */
    QPointF m_center;
    /**
     * @brief stores the filled state
     */
    bool m_filled;
};

#endif // ELLIPSITEM_H
