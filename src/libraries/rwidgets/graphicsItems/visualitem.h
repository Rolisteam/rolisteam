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
#ifndef VISUALITEM_H
#define VISUALITEM_H

#include <QAction>
#include <QGraphicsObject>
#include <QPointer>
#include <QVector>

#include "childpointitem.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "media/mediatype.h"
#include "rwidgets_global.h"

class NetworkMessageWriter;
class NetworkMessageReader;

/**
 * @brief abstract class which defines interface for all map items.
 */
class RWIDGET_EXPORT VisualItem : public QGraphicsObject
{
    Q_OBJECT
public:
    /**
     * @brief VisualItem default constructor
     */
    VisualItem(vmap::VisualItemController* ctrl);
    /**
     * @brief ~VisualItem
     */
    virtual ~VisualItem();

    vmap::VisualItemController* controller() const;
    /**
     * @brief setNewEnd
     * @param nend
     */
    virtual void setNewEnd(const QPointF& nend)= 0;
    /**
     * @brief getType
     * @return
     */
    virtual vmap::VisualItemController::ItemType getType() const;
    /**
     * @brief resizeContents
     * @param rect
     * @param keepRatio
     */
    virtual void resizeContents(const QRectF& rect, int pointId, Core::TransformType transformType= Core::TransformType::KeepRatio);
    /**
     * @brief setRectSize
     * @param w
     * @param h
     */
    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h);
    /**
     * @brief endOfGeometryChange
     */
    virtual void endOfGeometryChange(ChildPointItem::Change change);
    /**
     * @brief addActionContextMenu
     */
    virtual void addActionContextMenu(QMenu&);
    /**
     * @brief hasFocusOrChild
     * @return
     */
    bool hasFocusOrChild();
    /**
     * @brief setModifiers - default implementatino do nothing.
     * @param modifiers
     */
    virtual void setModifiers(Qt::KeyboardModifiers modifiers);

    /**
     * @brief getItemCopy pure method to return a copy of this item.
     * @return the copy
     */
    virtual VisualItem* getItemCopy()= 0;
    /**
     * @brief promoteTo
     * @return
     */
    virtual VisualItem* promoteTo(vmap::VisualItemController::ItemType);
    /**
     * @brief setSize
     * @param size
     */
    virtual void setSize(QSizeF size);

    // bool isEditable() const;
    /**
     * @brief setEditableItem
     */
    virtual void updateItemFlags();
    virtual bool isLocal() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    quint16 getPenWidth() const;
    void setPenWidth(const quint16& penWidth);

    QColor color() const;

    static int getHighlightWidth();
    static void setHighlightWidth(int highlightWidth);

    static QColor getHighlightColor();
    static void setHighlightColor(const QColor& highlightColor);
    /**
     * @brief canBeMove
     * @return
     */
    virtual bool canBeMoved() const;

signals:
    /**
     * @brief itemGeometryChanged
     */
    void itemGeometryChanged(VisualItem*);
    /**
     * @brief itemRemoved
     */
    void itemRemoved(QString, bool, bool);
    /**
     * @brief duplicateItem
     */
    void duplicateItem(VisualItem*);
    /**
     * @brief itemLayerChanged
     */
    void itemLayerChanged(VisualItem*);
    /**
     * @brief promoteItemTo
     */
    void promoteItemTo(VisualItem*, vmap::VisualItemController::ItemType);
    /**
     * @brief selectStateChange
     */
    void selectStateChange(bool);
    // void changeStackPosition(VisualItem*, VisualItem::StackOrder);

public slots:
    void setColor(QColor color);
    void evaluateVisible();

protected:
    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    /**
     * @brief mousePressEvent
     * @param event
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    /**
     * @brief mouseMoveEvent
     * @param event
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    /**
     * @brief keyPressEvent
     * @param event
     */
    virtual void keyPressEvent(QKeyEvent* event);
    /**
     * @brief init
     */
    void init();
    /**
     * @brief updateChildPosition
     */
    virtual void updateChildPosition();
    /**
     * @brief setChildrenVisible
     * @param b
     */
    virtual void setChildrenVisible(bool b);
    /**
     * @brief createActions
     */
    virtual void createActions();
    /**
     * @brief hasPermissionToMove
     * @return
     */
    bool hasPermissionToMove(bool allowCharacter= true) const;

protected:
    QPointF computeClosePoint(QPointF pos);
    /**
     * @brief manageAction
     */
    void manageAction();
    /**
     * @brief addPromoteItemMenu
     */
    void addPromoteItemMenu(QMenu*);
    /**
     * @brief promoteItem
     */
    void promoteItem();

protected:
    QPointer<vmap::VisualItemController> m_ctrl;
    static QColor m_highlightColor;
    static int m_highlightWidth;
    QVector<ChildPointItem*> m_children;
    QPoint m_menuPos;

    /// QAction*
    QAction* m_duplicateAct= nullptr;
    QAction* m_putGroundLayer= nullptr;
    QAction* m_putObjectLayer= nullptr;
    QAction* m_putCharacterLayer= nullptr;
    QVector<vmap::VisualItemController::ItemType> m_promoteTypeList;

private:
    static QStringList s_type2NameList;
};

#endif // VISUALITEM_H
