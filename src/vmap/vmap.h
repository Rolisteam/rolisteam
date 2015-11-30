/***************************************************************************
    *   Copyright (C) 2010 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#ifndef VMAP_H
#define VMAP_H

#include <QGraphicsScene>
#include <QColor>
#include "vtoolbar.h"
#include "map/map.h"
#include "data/person.h"
#include "vmap/items/visualitem.h"
#include "vmap/items/sightitem.h"

class CharacterItem;
/**
    * @brief allows users to draw a map on the fly. It manages several kinds of items (VisualItem): rect, line...
    * It is using the QGraphicsScene from Qt.
    */
class VMap : public QGraphicsScene
{
    Q_OBJECT
public:
    enum GRID_PATTERN{NONE,SQUARE,HEXAGON,OCTOGON};
    enum SCALE_UNIT{M,KM,CM,MILE,YARD,INCH,FEET,PX};
    enum VisibilityMode {HIDDEN,CHARACTER,ALL};
    /**
    * @brief default constructor
    */
    VMap(QObject * parent = 0);
    /**
    * @brief constructor with parameters
    * @param witdh of the map
    * @param height of the map
    * @param title of the subwindow, will be used for saving the map into file.
    * @param bgcolor is the backgound color of the map (commonly white).
    */
    VMap(int width,int height,QString& title,QColor& m_bgColor,QObject * parent = 0);
    /**
    * @brief defines the width
    */
    void setWidth(int width);
    /**
    * @brief defines the height
    */
    void setHeight(int height);
    /**
    * @brief defines the title
    */
    void setTitle(QString title);
    /**
    * @brief defines the background color
    */
    void setBackGroundColor(QColor bgcolor);
    /**
    * @brief  unused ?
    * @todo check the relevance of this function
    */
    void setSceneRect();
    /**
    * @brief accessor to width
    */
    int mapWidth() const;
    /**
    * @brief accessor to height
    */
    int mapHeight() const;
    /**
    * @brief accessor to the title
    */
    const QString& getMapTitle() const;
    /**
    * @brief accessor to the background color
    */
    const QColor& mapColor() const;
    /**
     * @brief saveFile
     */
    void saveFile(QDataStream& );
    /**
     * @brief openFile
     */
    void openFile(QDataStream& );
    /**
     * @brief addCharacter
     * @param p
     * @param pos
     */
    void addCharacter(Character* p, QPointF pos);
    /**
     * @brief getId
     * @return
     */
    QString getId() const;
    /**
     * @brief processAddItemMessage
     * @param msg
     */
    void processAddItemMessage(NetworkMessageReader* msg);
    /**
     * @brief processMoveItemMessage
     * @param msg
     */
    void processMoveItemMessage(NetworkMessageReader* msg);
    /**
     * @brief processGeometryChangeItem
     * @param msg
     */
    void processGeometryChangeItem(NetworkMessageReader* msg);
	/**
	 * @brief setPermissionMode
	 * @param mode
	 */
	void setPermissionMode(Map::PermissionMode mode);
    /**
     * @brief setVisibilityMode
     * @param mode
     */
    bool setVisibilityMode(VMap::VisibilityMode mode);
    /**
     * @brief processDelItemMessage
     * @param msg
     */
    void processDelItemMessage(NetworkMessageReader* msg);
	/**
	 * @brief getPermissionMode
	 * @return
	 */
    Map::PermissionMode getPermissionMode();
    /**
     * @brief getVisibilityMode
     * @return
     */
    VMap::VisibilityMode getVisibilityMode();
    /**
     * @brief fill
     * @param msg
     */
    void fill(NetworkMessageWriter& msg);
    /**
     * @brief sendAllItems
     * @param msg
     */
    void sendAllItems(NetworkMessageWriter& msg);
	/**
	 * @brief getPermissionModeText
	 * @return
	 */
	QString getPermissionModeText();
	/**
	 * @brief getVisibilityModeText
	 * @return
	 */
	QString getVisibilityModeText();
	/**
	 * @brief getBackGroundColor
	 * @return
	 */
    QColor getBackGroundColor()const;
	/**
	 * @brief editLayer
	 * @param layer
	 * @return
	 */
    bool editLayer(VisualItem::Layer layer);
	/**
	 * @brief getCurrentLayer
	 * @return
	 */
    VisualItem::Layer getCurrentLayer() const;
	/**
	 * @brief isIdle
	 * @return
	 */
    bool isIdle() const ;
	/**
	 * @brief VMap::getCurrentLayerText
	 * @return
	 */
	QString getCurrentLayerText() const;
    /**
     * @brief initScene
     */
    void initScene();
    /**
     * @brief setOption
     * @param pop
     * @param value
     */
    void setOption(VisualItem::Properties pop,QVariant value);
    /**
     * @brief getOption
     * @param pop
     * @return
     */
    QVariant getOption(VisualItem::Properties pop);
public slots:
    /**
    * @brief defines the current tools
    * @param new tool
    */
    void setCurrentTool(VToolsBar::SelectableTool selectedtool);
    /**
    * @brief defines the current color for painting
    * @param new color
    */
    void setCurrentChosenColor(QColor&);
    /**
    * @brief defines the pen size (sent off by toolbar).
    */
    void setPenSize(int);
    /**
     * @brief setCurrentNpcName
     * @param text
     */
    void setCurrentNpcName(QString text);
    /**
     * @brief setCurrentNpcNumber
     * @param number
     */
    void setCurrentNpcNumber(int number);
    void setId(QString id);
    void removeItemFromScene(QString);
    /**
     * @brief VMap::computePattern
     */
    void computePattern();
    /**
     * @brief duplicateItem
     * @param item
     */
	void duplicateItem(VisualItem* item);
    /**
     * @brief readMessage
     * @param msg
     * @param readCharacter
     */
    void readMessage(NetworkMessageReader& msg,bool readCharacter = true);
    /**
     * @brief setLocalId
     */
    void setLocalId(QString);
    /**
     * @brief getCharacterOnMap
     * @param id
     * @return
     */
    QList<CharacterItem*> getCharacterOnMap(QString id);
    /**
     * @brief getFogItem
     * @return
     */
    SightItem* getFogItem() const;
signals:
    /**
     * @brief npcAdded
     */
    void npcAdded();
	/**
	 * @brief mapChanged is emitted after some changes on map has been made.
	 */
	void mapChanged();
private slots:
    /**
     * @brief sendItemToAll
     */
    void sendItemToAll(VisualItem*);
    /**
     * @brief sendOffItem
     * @param item
     */
    void sendOffItem(VisualItem* item);
    /**
     * @brief updateItem
     */
	void updateItem();
    /**
     * @brief checkItemLayer
     */
    void checkItemLayer(VisualItem*);
    /**
     * @brief promoteItemInType
     */
    void promoteItemInType(VisualItem*, VisualItem::ItemType);
    /**
     * @brief insertCharacterInMap
     * @param item
     */
    void insertCharacterInMap(CharacterItem* item);
    /**
     * @brief changeStackOrder triggered when VisualItem should change its z order. It recompute the zvalue of all items.
     * @param item to move
     * @param op operation to be done.
     */
    void changeStackOrder(VisualItem* item,VisualItem::StackOrder op);
protected:
    /**
    * @brief catches move event with the mouse, useful for allowing move of item
    */
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    /**
    * @brief catches mouse Press event, defines the first point of the next added item.
    */
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    /**
    * @brief  catches release event, defines the last point of the next added item.
    */
    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    /**
     * @brief keyPressEvent
     * @param event
     */
    virtual void keyPressEvent(QKeyEvent* event);
    /**
    * @brief adds item depending of the current tool.
    */
    void addItem();
    /**
     * @brief addNewItem
     * @param item
     */
    void addNewItem(VisualItem* item);
    /**
     * @brief dragEnterEvent
     * @param event
     */
	void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent ( QGraphicsSceneDragDropEvent * event );
    /**
     * @brief dragMoveEvent
     * @param event
     */
	void dragMoveEvent( QGraphicsSceneDragDropEvent * event );
    /**
     * @brief initMap
     */
    void initMap();
private:
    /**
    * @brief width of the map
    */
    int m_width;
    /**
    * @brief height of the map
    */
    int m_height;
    /**
    * @brief title of the map
    */
    QString m_title;
    /**
    * @brief background color
    */
    QColor m_bgColor;
    /**
    * @brief current tool
    */
    VToolsBar::SelectableTool m_selectedtool;
    /**
    * @brief first point of the next item
    */
    QPointF m_first;
    /**
    * @brief last point of the next item
    */
    QPointF m_end;
    /**
    * @brief current selected item
    */
    VisualItem* m_currentItem;
    /**
     * @brief m_currentPath
     */
	VisualItem* m_currentPath;
    /**
    * @brief color for items
    */
    QColor m_itemColor;
    /**
    * @brief pen size
    */
    int m_penSize;
    /**
    * @brief Items list which are part of the map.
    */
    QMap<QString,VisualItem*>* m_itemMap;
    /**
     * @brief m_orderedItemList is used to store zorder of all items.
     */
    QList<VisualItem*> m_orderedItemList;
    /**
    * @brief Items list which are part of the map.
    */
    QMap<QString,VisualItem*>* m_characterItemMap;
    /**
     * @brief m_computedPattern
     */
    QImage m_computedPattern;
    /**
     * @brief m_nameNPC
     */
    QString m_currentNpcName;
    /**
     * @brief m_currentNpcNumber
     */
    int m_currentNpcNumber;
    /**
     * @brief m_id
     */
    QString m_id;
    /**
     * @brief m_localUserId
     */
    QString m_localUserId;
    /**
     * @brief m_hasPermissionMode
     */
	bool m_hasPermissionMode;
    /**
     * @brief m_currentMode
     */
	Map::PermissionMode m_currentMode;
    /**
     * @brief m_currentVisibityMode
     */
    VMap::VisibilityMode m_currentVisibityMode;
    /**
     * @brief m_currentLayer
     */
    VisualItem::Layer m_currentLayer;

    QPolygonF* m_currentFogPolygon;

    SightItem* m_sightItem;

    QHash<VisualItem::Properties,QVariant>* m_propertiesHash;

    friend QDataStream& operator<<(QDataStream& os,const VMap&);
    friend QDataStream& operator>>(QDataStream& is,VMap&);
};
#endif // VMAP_H
