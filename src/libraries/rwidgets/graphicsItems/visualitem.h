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
    VisualItem(vmap::VisualItemController* ctrl);
    virtual ~VisualItem();

    vmap::VisualItemController* controller() const;
    virtual void setNewEnd(const QPointF& nend)= 0;
    virtual vmap::VisualItemController::ItemType getType() const;
    virtual void resizeContents(const QRectF& rect, int pointId,
                                Core::TransformType transformType= Core::TransformType::KeepRatio);
    virtual void endOfGeometryChange(ChildPointItem::Change change);
    virtual void addActionContextMenu(QMenu&);
    bool hasFocusOrChild();
    virtual void setModifiers(Qt::KeyboardModifiers modifiers);
    virtual VisualItem* promoteTo(vmap::VisualItemController::ItemType);
    virtual void setSize(QSizeF size);
    virtual void updateItemFlags();
    virtual bool isLocal() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    quint16 getPenWidth() const;
    void setPenWidth(const quint16& penWidth);

    QString uuid()const;

    QColor color() const;
    virtual bool canBeMoved() const;

    static int getHighlightWidth();
    static void setHighlightWidth(int highlightWidth);

    static QColor getHighlightColor();
    static void setHighlightColor(const QColor& highlightColor);

signals:
    void itemGeometryChanged(VisualItem*);
    void itemRemoved(QString, bool, bool);
    void itemLayerChanged(VisualItem*);
    void promoteItemTo(VisualItem*, vmap::VisualItemController::ItemType);
    void selectStateChange(bool);

public slots:
    void setColor(QColor color);
    void evaluateVisible();

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    void init();
    virtual void updateChildPosition();
    virtual void setChildrenVisible(bool b);
    bool hasPermissionToMove(bool allowCharacter= true) const;

protected:
    QPointF computeClosePoint(QPointF pos);
    void addPromoteItemMenu(QMenu*);
    void promoteItem();

protected:
    QPointer<vmap::VisualItemController> m_ctrl;
    static QColor m_highlightColor;
    static QColor m_selectedColor;
    static int m_highlightWidth;
    QVector<ChildPointItem*> m_children;
    QPoint m_menuPos;
    QVector<vmap::VisualItemController::ItemType> m_promoteTypeList;

private:
    static QStringList s_type2NameList;
};

#endif // VISUALITEM_H
