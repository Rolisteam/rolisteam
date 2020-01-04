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

#ifndef CHILDPOINTITEM_H
#define CHILDPOINTITEM_H

#include <QFlags>
#include <QGraphicsObject>
#include <QPointer>

namespace vmap
{
class VisualItemController;
}
class VisualItem;
/**
 * @brief The ChildPointItem class controls and allows geometry transforms to its parent from user inputs.
 */
class ChildPointItem : public QGraphicsObject
{
public:
    /**
     * @brief The MOTION enum, ALL means all axis, MOUSE means the changes are control by mouse event instead of
     * geometry event on item. In this case, the ChildPointItem does not mouse, it receives mouse event and ask its
     * parent to change its geometry.
     */
    enum MOTION
    {
        NONE= 0x0,
        X_AXIS= 0x1,
        Y_AXIS= 0x2,
        MOVE= 0x4,
        ROTATION= 0x8,
        MOUSE= MOVE | ROTATION,
        ALL= MOUSE | MOVE
    };
    Q_ENUM(MOTION)
    Q_DECLARE_FLAGS(MOTIONS, MOTION)
    /**
     * @brief The PLACEMENT enum
     */
    enum PLACEMENT
    {
        TopLeft,
        TopRight,
        TopCenter,
        MiddelLeft,
        MiddleRight,
        Center,
        ButtomLeft,
        ButtomRight,
        ButtomCenter
    };

    enum Change
    {
        None,
        Rotation,
        Resizing,
        Moving
    };

    /**
     * @brief ChildPointItem
     * @param point
     * @param parent
     */
    ChildPointItem(vmap::VisualItemController* ctrl, int point, VisualItem* parent, bool isVision= false);
    /**
     * @brief ~ChildPointItem
     */
    virtual ~ChildPointItem();
    /**
     * @brief itemChange
     * @param change
     * @param value
     * @return
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    /**
     * @brief boundingRect
     * @return
     */
    QRectF boundingRect() const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    /**
     * @brief setMotion
     * @param m
     */
    void setMotion(ChildPointItem::MOTION m);
    /**
     * @brief setPlacement
     * @param p
     */
    void setPlacement(ChildPointItem::PLACEMENT p);
    ChildPointItem::PLACEMENT placement() const;
    /**
     * @brief setRotationEnable, set to true if you want to allow rotation without activating the mouse control.
     */
    void setRotationEnable(bool);
    /**
     * @brief setEditableItem
     * @param b
     */
    void setEditableItem(bool b);

    /**
     * @brief setPointID
     */
    void setPointID(int);
    /**
     * @brief getPointID
     * @return
     */
    int getPointID() const;
    /**
     * @brief isVisionHandler
     * @return
     */
    bool isVisionHandler();
    /**
     * @brief setVisionHandler
     */
    void setVisionHandler(bool);

protected:
    /**
     * @brief ChildPointItem::mouseMoveEvent
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    /**
     * @brief ChildPointItem::mouseReleaseEvent
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    QPointer<vmap::VisualItemController> m_ctrl;
    Change m_currentChange= None;
    int m_pointId;
    QPointF m_startPoint;
    VisualItem* m_parent;
    MOTION m_currentMotion;
    bool m_allowRotation;
    bool m_editable;
    bool m_vision;
    PLACEMENT m_placement;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ChildPointItem::MOTIONS)
#endif // CHILDPOINTITEM_H
