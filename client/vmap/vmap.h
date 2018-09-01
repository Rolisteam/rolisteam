/***************************************************************************
    *   Copyright (C) 2010 by Renaud Guezennec                                *
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
#ifndef VMAP_H
#define VMAP_H

#include <QGraphicsScene>
#include <QColor>
#include <QUndoStack>
#include "vtoolbar.h"
#include "map/map.h"
#include "data/person.h"
#include "vmap/items/visualitem.h"
#include "vmap/items/sightitem.h"
#include "vmap/items/griditem.h"


/**
 * @page VMap
 * @tableofcontents
 * @section Intro Introduction
 * VMap is vectorial map system to draw map on the fly.<br/>
 * It has been introduced in rolisteam v1.8.
 *
 *
 * @section item Type of Items:
 * @section action Actions on Items:
 * @section Selection Selection:
 * @section Vision Vision:
 * @section fow Fog of war:
 *
 *
 */
class CharacterItem;
class AddVmapItemCommand;
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
    enum VisibilityMode {HIDDEN,FOGOFWAR,ALL};
    enum APPLY_ON_CHARACTER {SelectionOnly, AllCharacter, AllNPC};
    /**
    * @brief default constructor
    */
    VMap(QObject * parent = nullptr);
    /**
    * @brief constructor with parameters
    * @param witdh of the map
    * @param height of the map
    * @param title of the subwindow, will be used for saving the map into file.
    * @param bgcolor is the backgound color of the map (commonly white).
    */
    VMap(int width,int height,QString& title,QColor& m_bgColor,QObject * parent = nullptr);
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
     * @brief itemsBoundingRectWithoutSight
     * @return
     */
    QRectF itemsBoundingRectWithoutSight();
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
     * @brief processOpacityMessage
     * @param msg
     */
    void processOpacityMessage(NetworkMessageReader* msg);
    /**
     * @brief processSetParentItem
     * @param msg
     */
    void processSetParentItem(NetworkMessageReader* msg);
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
     * @brief processMovePointMsg
     * @param msg
     */
    void processMovePointMsg(NetworkMessageReader* msg);
    /**
     * @brief manageAnchor
     */
    void manageAnchor();
	/**
	 * @brief getPermissionMode
	 * @return
	 */
    Map::PermissionMode getPermissionMode();
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
    bool setOption(VisualItem::Properties pop,QVariant value);
    /**
     * @brief getOption
     * @param pop
     * @return
     */
    QVariant getOption(VisualItem::Properties pop);
    /**
     * @brief setAnchor
     * @param child
     * @param parent
     * @param send
     */
    void setAnchor(QGraphicsItem* child,QGraphicsItem* parent,bool send = true);
    /**
     * @brief currentLayer
     * @return
     */
    VisualItem::Layer currentLayer() const;
    /**
     * @brief setCurrentLayer
     * @param currentLayer
     */
    void setCurrentLayer(const VisualItem::Layer &currentLayer);
    /**
     * @brief getSelectedtool
     * @return
     */
  VToolsBar::SelectableTool getSelectedtool() const;
    /**
     * @brief processZValueMsg
     * @param msg
     */
    void processZValueMsg(NetworkMessageReader *msg);
    /**
     * @brief processRotationMsg
     * @param msg
     */
    void processRotationMsg(NetworkMessageReader *msg);
    /**
     * @brief processRectGeometryMsg
     * @param msg
     */
    void processRectGeometryMsg(NetworkMessageReader *msg);
    /**
     * @brief processCharacterStateHasChanged
     * @param msg
     */
    void processCharacterStateHasChanged(NetworkMessageReader &msg);
    /**
     * @brief processCharacterHasChanged
     * @param msg
     */
    void processCharacterHasChanged(NetworkMessageReader &msg);
    /**
     * @brief processVisionMsg
     * @param msg
     */
    void processVisionMsg(NetworkMessageReader *msg);
    void processColorMsg(NetworkMessageReader* msg);
    const QString& getLocalUserId() const;
    int getCurrentNpcNumber() const;

    QString getCurrentNpcName() const;

    void removeItemFromData(VisualItem* item);
    void addItemFromData(VisualItem* item);
    void insertItemFromData(VisualItem* item,int pos);

    QUndoStack* getUndoStack() const;
    void setUndoStack(QUndoStack* undoStack);

    QHash<VisualItem::Properties, QVariant> *getPropertiesHash() const;
    void setPropertiesHash(QHash<VisualItem::Properties, QVariant> *propertiesHash);

    void addImageItem(QImage img);
    void addCommand(QUndoCommand *cmd);
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
    /**
     * @brief setId
     * @param id
     */
    void setId(QString id);
    /**
     * @brief removeItemFromScene
     */
    void removeItemFromScene(QString ,bool sendToAll = true, bool undoable = true);
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
    /**
     * @brief setEditionMode
     */
    void setEditionMode(VToolsBar::EditionMode);
    /**
     * @brief getEditionMode
     * @return
     */
    VToolsBar::EditionMode getEditionMode();
    /**
     * @brief cleanFogEdition
     */
    void cleanFogEdition();
    /**
     * @brief changeStackOrder triggered when VisualItem should change its z order. It recompute the zvalue of all items.
     * @param item to move
     * @param op operation to be done.
     */
    void changeStackOrder(VisualItem* item,VisualItem::StackOrder op);
    /**
     * @brief addImageItem
     * @param file
     */
    void addImageItem(QString file);
    /**
     * @brief setCurrentItemOpacity
     */
    void setCurrentItemOpacity(qreal);
    /**
     * @brief selectionHasChanged
     */
    void selectionHasChanged();
    /**
     * @brief processLayerMessage
     * @param msg
     */
    void processLayerMessage(NetworkMessageReader *msg);
    /**
     * @brief ownerHasChangedForCharacterItem
     * @param item
     * @param cItem
     */
    void ownerHasChangedForCharacterItem(Character *item, CharacterItem *cItem);
    /**
     * @brief selectionPositionHasChanged
     */
    //void selectionPositionHasChanged();
    void showTransparentItems();
    //void selectionPositionAboutToChange();
    void cleanUpInit(APPLY_ON_CHARACTER zone);
    void rollInit(APPLY_ON_CHARACTER zone);
signals:
    /**
     * @brief npcAdded
     */
    void npcAdded();
	/**
	 * @brief mapChanged is emitted after some changes on map has been made.
	 */
	void mapChanged();
    /**
     * @brief mapStatutChanged
     */
    void mapStatutChanged();

    /**
     * @brief currentItemOpacity
     */
    void currentItemOpacity(qreal);
    /**
     * @brief colorPipette
     */
    void colorPipette(QColor);

    /**
     * @brief runCommandForCharacter
     * @param cmd
     * @param uuid
     */
    void runDiceCommandForCharacter(QString cmd, QString uuid);
private slots:
    /**
     * @brief sendItemToAll
     */
    void sendItemToAll(VisualItem*);
    /**
     * @brief sendOffItem
     * @param item
     */
    void sendOffItem(VisualItem* item, bool doInitPoint = true);
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
     * @brief characterHasBeenDeleted
     */
    void characterHasBeenDeleted(Character* );
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
     * @param item to add
     * @param fromNetwork, true when item is added from network, false by default.
     */
    void addNewItem(AddVmapItemCommand* item, bool undoable, bool fromNetwork = false);
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
    /**
     * @brief hideOtherLayers
     * @param b
     */
    void hideOtherLayers(bool b);
    /**
     * @brief ensureFogAboveAll
     */
    void ensureFogAboveAll();
    bool isNormalItem(QGraphicsItem *item);
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
    VisualItem* m_currentItem = nullptr;
    /**
     * @brief m_currentPath
     */
    VisualItem* m_currentPath = nullptr;
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
    QMap<QString,VisualItem*>* m_itemMap = nullptr;
    /**
     * @brief m_sortedItemList
     */
    QStringList m_sortedItemList;//sorted by time of insertion
    /**
     * @brief m_orderedItemList is used to store zorder of all items.
     */
    QList<VisualItem*> m_orderedItemList;
    /**
    * @brief Items list which are part of the map.
    */
    QMap<QString,CharacterItem*>* m_characterItemMap = nullptr;
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
     * @brief m_currentLayer
     */
    VisualItem::Layer m_currentLayer;

    SightItem* m_sightItem;
    GridItem* m_gridItem;

    VToolsBar::EditionMode m_editionMode;

    VisualItem* m_fogItem;
    FogSingularity* m_currentFog;
    quint64 m_zIndex;

    QHash<VisualItem::Properties,QVariant>* m_propertiesHash = nullptr;
    QUndoStack* m_undoStack = nullptr;
    AddVmapItemCommand* m_currentAddCmd = nullptr;
    QList<VisualItem*> m_movingItems;
    QList<QPointF> m_oldPos;

    friend QDataStream& operator<<(QDataStream& os,const VMap&);
    friend QDataStream& operator>>(QDataStream& is,VMap&);
};
#endif // VMAP_H
