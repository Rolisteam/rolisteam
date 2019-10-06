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

#include <QGraphicsObject>

#include "childpointitem.h"
#include <QAction>
#include <QVector>

class NetworkMessageWriter;
class NetworkMessageReader;

/**
 * @brief abstract class which defines interface for all map items.
 */
class VisualItem : public QGraphicsObject
{
    Q_OBJECT
public:
    /**
     * @brief The ItemType enum
     */
    enum ItemType
    {
        PATH,
        LINE,
        ELLISPE,
        CHARACTER,
        TEXT,
        RECT,
        RULE,
        IMAGE,
        SIGHT,
        ANCHOR,
        GRID,
        HIGHLIGHTER
    };
    Q_ENUM(ItemType)
    /**
     * @brief The Layer enum
     */
    enum Layer
    {
        GROUND,
        OBJECT,
        CHARACTER_LAYER,
        FOG,
        GRIDLAYER,
        NONE
    };
    Q_ENUM(Layer)
    /**
     * @brief The StackOrder enum
     */
    enum StackOrder
    {
        FRONT,
        RAISE,
        LOWER,
        BACK
    };
    Q_ENUM(StackOrder)
    enum TransformType
    {
        NoTransform,
        KeepRatio,
        Sticky
    };
    Q_ENUM(TransformType)
    /**
     * @brief The Properties enum
     */
    enum Properties
    {
        ShowNpcName,
        ShowPcName,
        ShowNpcNumber,
        ShowHealthStatus,
        ShowInitScore,
        ShowGrid,
        LocalIsGM,
        GridPattern,
        GridColor,
        GridSize,
        Scale,
        Unit,
        EnableCharacterVision,
        PermissionMode,
        FogOfWarStatus,
        CollisionStatus,
        GridAbove,
        HideOtherLayers,
        VisibilityMode,
        ShowHealthBar,
        MapLayer
    };
    Q_ENUM(Properties)
    /**
     * @brief VisualItem default constructor
     */
    VisualItem();
    /**
     * @brief VisualItem constructor with parameters
     * @param penColor color
     * @param editable edition status
     * @param parent
     */
    VisualItem(const QColor& penColor, int size, QGraphicsItem* parent= nullptr);
    /**
     * @brief ~VisualItem
     */
    virtual ~VisualItem();

    /**
     * @brief setNewEnd
     * @param nend
     */
    virtual void setNewEnd(QPointF& nend)= 0;
    /**
     * @brief setPenColor
     * @param penColor
     */
    virtual void setPenColor(QColor& penColor);
    /**
     * @brief getColor
     * @return
     */
    virtual QColor getColor();
    /**
     * @brief writeData
     * @param out
     */
    virtual void writeData(QDataStream& out) const= 0;
    /**
     * @brief readData
     * @param in
     */
    virtual void readData(QDataStream& in)= 0;

    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType() const= 0;
    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg)= 0;
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg)= 0;
    /**
     * @brief setId
     * @param id
     */
    virtual void setId(QString id);
    /**
     * @brief getId
     * @return
     */
    virtual QString getId();
    /**
     * @brief setMapId
     * @param id
     */
    virtual void setMapId(QString id);
    /**
     * @brief getMapId
     * @return
     */
    virtual QString getMapId();

    /**
     * @brief resizeContents
     * @param rect
     * @param keepRatio
     */
    virtual void resizeContents(const QRectF& rect, TransformType transformType= KeepRatio);

    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId, QPointF& pos)= 0;
    /**
     * @brief setRectSize
     * @param w
     * @param h
     */
    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h);
    /**
     * @brief endOfGeometryChange
     */
    virtual void endOfGeometryChange();

    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem()= 0;
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
     * @brief operator <<
     * @param os
     * @return
     */
    friend QDataStream& operator<<(QDataStream& os, const VisualItem&);
    /**
     * @brief operator >>
     * @param is
     * @return
     */
    friend QDataStream& operator>>(QDataStream& is, VisualItem&);

    /**
     * @brief getLayer
     * @return
     */
    virtual VisualItem::Layer getLayer() const;
    /**
     * @brief setLayer
     */
    virtual void setLayer(VisualItem::Layer);
    /**
     * @brief promoteTo
     * @return
     */
    virtual VisualItem* promoteTo(VisualItem::ItemType);
    /**
     * @brief getLayerToText static method to translate layer to its name.
     * @param id
     * @return
     */
    static QString getLayerToText(VisualItem::Layer id);
    /**
     * @brief setPropertiesHash
     * @param hash
     */
    void setPropertiesHash(QHash<VisualItem::Properties, QVariant>* hash);
    /**
     * @brief getOption
     * @param pop
     * @return
     */
    QVariant getOption(VisualItem::Properties pop) const;
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
    void readOpacityMsg(NetworkMessageReader* msg);
    bool isHoldSize() const;
    virtual void setHoldSize(bool holdSize);

    virtual void readLayerMsg(NetworkMessageReader* msg);
    virtual void readMovePointMsg(NetworkMessageReader* msg);
    virtual bool isLocal() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);

    quint16 getPenWidth() const;
    void setPenWidth(const quint16& penWidth);

    static int getHighlightWidth();
    static void setHighlightWidth(int highlightWidth);

    static QColor getHighlightColor();
    static void setHighlightColor(const QColor& highlightColor);
    virtual bool itemAndMapOnSameLayer() const;
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
    void promoteItemTo(VisualItem*, VisualItem::ItemType);
    /**
     * @brief selectStateChange
     */
    void selectStateChange(bool);
    /**
     * @brief changeStackPosition
     */
    void changeStackPosition(VisualItem*, VisualItem::StackOrder);

public slots:
    /**
     * @brief sendPositionMsg
     */
    virtual void sendPositionMsg();
    /**
     * @brief readPositionMsg
     * @param msg
     */
    virtual void readPositionMsg(NetworkMessageReader* msg);

    virtual void sendOpacityMsg();
    void posChange();
    void sendItemLayer();
    void readRectGeometryMsg(NetworkMessageReader* msg);
    void sendRectGeometryMsg();
    void readRotationMsg(NetworkMessageReader* msg);
    void sendRotationMsg();
    void readZValueMsg(NetworkMessageReader* msg);
    void sendZValueMsg();
    void rectChange();
    void rotationChange();

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

protected:
    QColor m_color;
    static QColor m_highlightColor;
    static int m_highlightWidth;
    QString m_id;
    QString m_mapId;
    // bool m_editable = false;
    QVector<ChildPointItem*>* m_child= nullptr;
    quint16 m_penWidth= 1;
    QRectF m_rect;
    QPoint m_menuPos;

    /// QAction*
    QAction* m_duplicateAct= nullptr;
    QAction* m_putGroundLayer= nullptr;
    QAction* m_putObjectLayer= nullptr;
    QAction* m_putCharacterLayer= nullptr;

    VisualItem::Layer m_layer= VisualItem::NONE;
    QVector<ItemType> m_promoteTypeList;
    QList<QPointF> m_pointList;
    bool m_resizing= false;
    bool m_rotating= false;
    bool m_receivingZValue= false;

    QHash<VisualItem::Properties, QVariant>* m_propertiesHash= nullptr;
    bool m_holdSize= false;

private slots:
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

private:
    static QStringList s_type2NameList;
    static QStringList s_layerName;
};

#endif // VISUALITEM_H
