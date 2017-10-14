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

#include <QVector>
#include <QAction>
#include "childpointitem.h"

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
    enum ItemType{PATH,LINE,ELLISPE,CHARACTER,TEXT,RECT,RULE,IMAGE,SIGHT,ANCHOR,GRID};
    /**
     * @brief The Layer enum
     */
    enum Layer{GROUND,OBJECT,CHARACTER_LAYER,FOG,GRIDLAYER,NONE};
    /**
     * @brief The StackOrder enum
     */
    enum StackOrder{FRONT,RAISE,LOWER,BACK};
    /**
     * @brief The Properties enum
     */
    enum Properties {
        ShowNpcName,
        ShowPcName,
        ShowNpcNumber,
        ShowHealtStatus,
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
        VisibilityMode};
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
	VisualItem(QColor& penColor,bool editable,QGraphicsItem * parent = 0);
    /**
     * @brief ~VisualItem
     */
    virtual ~VisualItem();
    
	/**
	 * @brief setNewEnd
	 * @param nend
	 */
    virtual void setNewEnd(QPointF& nend)=0;
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
    virtual void writeData(QDataStream& out) const =0;
	/**
	 * @brief readData
	 * @param in
	 */
    virtual void readData(QDataStream& in)=0;
    
	/**
	 * @brief getType
	 * @return
	 */
    virtual VisualItem::ItemType getType() const =0;
	/**
	 * @brief fillMessage
	 * @param msg
	 */
    virtual void fillMessage(NetworkMessageWriter* msg)=0;
	/**
	 * @brief readItem
	 * @param msg
	 */
    virtual void readItem(NetworkMessageReader* msg)=0;
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
    virtual void resizeContents(const QRectF& rect, bool keepRatio = true);

    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId,QPointF& pos) = 0;
    /**
     * @brief setRectSize
     * @param w
     * @param h
     */
    virtual void setRectSize(qreal x,qreal y,qreal w,qreal h);
    /**
     * @brief endOfGeometryChange
     */
    virtual void endOfGeometryChange();

    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem() = 0;
    /**
     * @brief addActionContextMenu
     */
    virtual void addActionContextMenu(QMenu*);
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
    virtual VisualItem* getItemCopy() = 0;
	/**
	 * @brief operator <<
	 * @param os
	 * @return
	 */
	friend QDataStream& operator<<(QDataStream& os,const VisualItem&);
	/**
	 * @brief operator >>
	 * @param is
	 * @return
	 */
    friend QDataStream& operator>>(QDataStream& is,VisualItem&);

	/**
	 * @brief getLayer
	 * @return
	 */
    virtual VisualItem::Layer getLayer();
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
    void setPropertiesHash(QHash<VisualItem::Properties,QVariant>* hash);
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


    bool isEditable() const;
    /**
     * @brief setEditableItem
     */
    virtual void setEditableItem(bool);
    void readOpacityMsg(NetworkMessageReader* msg);
    bool getHoldSize() const;
    void setHoldSize(bool holdSize);

    virtual void readLayerMsg(NetworkMessageReader* msg);
    virtual void readMovePointMsg(NetworkMessageReader* msg);
    virtual bool isLocal() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

signals:
    /**
     * @brief itemGeometryChanged
     */
    void itemGeometryChanged(VisualItem*);
	/**
	 * @brief itemRemoved
	 */
    void itemRemoved(QString);
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
    void promoteItemTo(VisualItem*,VisualItem::ItemType);
    /**
     * @brief selectStateChange
     */
    void selectStateChange(bool);
    /**
     * @brief changeStackPosition
     */
    void changeStackPosition(VisualItem*,  VisualItem::StackOrder);

    /**
     * @brief itemPositionHasChanged
     */
    void itemPositionHasChanged();
    /**
     * @brief itemPositionAboutToChange
     */
    void itemPositionAboutToChange();



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
    void readRectGeometryMsg(NetworkMessageReader *msg);
    void sendRectGeometryMsg();
    void readRotationMsg(NetworkMessageReader *msg);
    void sendRotationMsg();
    void readZValueMsg(NetworkMessageReader *msg);
    void sendZValueMsg();
    void rectChange();
    void rotationChange();
protected:
	/**
	 * @brief mouseReleaseEvent
	 * @param event
	 */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	/**
	 * @brief mousePressEvent
	 * @param event
	 */
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
	/**
	 * @brief mouseMoveEvent
	 * @param event
	 */
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
	/**
	 * @brief keyPressEvent
	 * @param event
	 */
    virtual void keyPressEvent(QKeyEvent* event);
	/**
	 * @brief contextMenuEvent
	 * @param event
	 */
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
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
     * @brief canBeMove
     * @return
     */
    virtual bool canBeMoved() const;
    /**
     * @brief hasPermissionToMove
     * @return
     */
    bool hasPermissionToMove(bool allowCharacter = true) const;

protected:
    QColor m_color;
    //ItemType m_type;
    QString m_id;
    QString m_mapId;
	bool m_editable;
    QRectF m_rect;
    QPoint m_menuPos;


    /// QAction*
    QAction* m_duplicateAct;
    QAction* m_putGroundLayer;
    QAction* m_putObjectLayer;
    QAction* m_putCharacterLayer;



    VisualItem::Layer m_layer;
    QVector<ItemType> m_promoteTypeList;
    QList<QPointF> m_pointList;
    //QList<QRectF> m_sizeList;
    bool m_resizing;
    bool m_rotating;
    bool m_receivingZValue;

    QHash<VisualItem::Properties,QVariant>* m_propertiesHash;
    QVector<ChildPointItem*>* m_child;
    bool m_holdSize;


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
    static QStringList type2NameList;
	static QStringList s_layerName;
};

#endif // VISUALITEM_H
