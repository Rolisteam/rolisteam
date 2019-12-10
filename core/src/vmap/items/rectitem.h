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
#ifndef RECTITEM_H
#define RECTITEM_H

#include "visualitem.h"
namespace vmap
{
class RectController;
}
/**
 * @brief displays and manages rectangle on map, part of QGraphicsScene/view.
 */
class RectItem : public VisualItem
{
    Q_OBJECT
public:
    RectItem(vmap::RectController* ctrl);
    // RectItem(const QPointF& topleft, const QPointF& buttomright, bool filled, quint16 penSize, const QColor&
    // penColor,
    //    QGraphicsItem* parent= nullptr);
    /**
     * @brief paint the current rectangle into the scene.
     * @see Qt documentation
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;

    /**
     * @brief gives bounding rect. Return rect geometry into the QRectF
     */
    virtual QRectF boundingRect() const override;

    /**
     * @brief defines new end point.
     */
    virtual void setNewEnd(const QPointF& nend) override;
    /**
     * @brief serialization function to write data.
     */
    virtual void writeData(QDataStream& out) const override;
    /**
     * @brief serialization function to read data from stream.
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
    /**
     * @brief shape
     * @return
     */
    virtual QPainterPath shape() const override;
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

    void endOfGeometryChange() override;

protected:
    /**
     * @brief updateChildPosition
     */
    virtual void updateChildPosition() override;

private:
    QPointer<vmap::RectController> m_rectCtrl;
};

#endif // RECTITEM_H
