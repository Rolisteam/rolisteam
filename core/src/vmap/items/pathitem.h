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
#ifndef PATHITEM_H
#define PATHITEM_H

#include "visualitem.h"
#include "vmap/controller/pathcontroller.h"
#include <QPen>

namespace vmap
{
class VisualItemController;
}
/**
 * @brief map item to paint a path on the scene/map
 */
class PathItem : public VisualItem
{
    Q_OBJECT
public:
    PathItem(vmap::PathController* ctrl);
    /**
     * @brief override function to paint itself.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
    /**
     * @brief defines the  boundrect of path. Useful for mouse selection or collision detection
     */
    virtual QRectF boundingRect() const override;
    /**
     * @brief adds new point at the end
     */
    virtual void setNewEnd(const QPointF& nend) override;
    /**
     * @brief accessor to the shape of path, better definition than boudingRect
     */
    virtual QPainterPath shape() const override;
    /**
     * @brief writing serialisation method
     */
    virtual void writeData(QDataStream& out) const override;
    /**
     * @brief reading serialisation method
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
     * @brief setStartPoint
     */
    void setStartPoint(QPointF);
    /**
     * @brief release
     */
    void release();

    /**
     * @brief addActionContextMenu
     */
    virtual void addActionContextMenu(QMenu&) override;
    /**
     * @brief endOfGeometryChange
     */
    virtual void endOfGeometryChange() override;
    /**
     * @brief readMovePointMsg
     * @param msg
     */
    virtual void readMovePointMsg(NetworkMessageReader* msg) override;
    void setHoldSize(bool holdSize) override;
    void addPoint(const QPointF& point);
    void addChild(const QPointF& point, int i);
protected slots:
    void sendPointPosition();

private:
    void initRealPoints();
    void createActions() override;

private:
    QPointer<vmap::PathController> m_pathCtrl;
    QAction* m_closeAct;
    QAction* m_fillAct;
};

#endif // PATHITEM_H
