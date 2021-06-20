/***************************************************************************
 *   Copyright (C) 2010 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include <QColor>
#include <QGraphicsScene>
#include <QUndoStack>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/person.h"
#include "media/mediatype.h"
#include "vmap/items/griditem.h"
#include "vmap/items/pathitem.h"
#include "vmap/items/sightitem.h"
#include "vmap/items/visualitem.h"
#include "vtoolbar.h"

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
class AnchorItem;
class RuleItem;
class AddVmapItemCommand;
namespace vmap
{
class RectController;
class EllipseController;
class LineController;
class ImageController;
class PathController;
class TextController;
} // namespace vmap
/**
 * @brief allows users to draw a map on the fly. It manages several kinds of items (VisualItem): rect, line...
 * It is using the QGraphicsScene from Qt.
 */
class VMap : public QGraphicsScene
{
    Q_OBJECT
public:
    /**
     * @brief default constructor
     */
    explicit VMap(VectorialMapController* ctrl, QObject* parent= nullptr);
    /**
     * @brief saveFile
     */
    void saveFile(QDataStream&);
    /**
     * @brief openFile
     */
    void openFile(QDataStream&);
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

    /**
     * @brief manageAnchor
     */
    void manageAnchor();
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
     * @brief isIdle
     * @return
     */
    bool isIdle() const;
    /**
     * @brief setAnchor
     * @param child
     * @param parent
     * @param send
     */
    void setAnchor(QGraphicsItem* child, QGraphicsItem* parent, bool send= true);
    /**
     * @brief setCurrentLayer
     * @param currentLayer
     */
    void updateLayer();

    /**
     * @brief processVisionMsg
     * @param msg
     */

    const QString& getLocalUserId() const;
    int getCurrentNpcNumber() const;

    void removeItemFromData(VisualItem* item);
    void addItemFromData(VisualItem* item);
    void insertItemFromData(VisualItem* item, int pos);

    QUndoStack* getUndoStack() const;
    void setUndoStack(QUndoStack* undoStack);

    // void addImageItem(const QString& imgPath);
    // void addImageItem(const QImage& img);
    void addCommand(QUndoCommand* cmd);

    int getItemCount();
    int getSortedItemCount();
    int getOrderedItemCount();
    /**
     * @brief getFogItem
     * @return
     */
    SightItem* getFogItem() const;
    bool isNormalItem(const QGraphicsItem* item);
    GridItem* getGridItem() const;

public slots:
    /**
     * @brief defines the current color for painting
     * @param new color
     */
    // void setCurrentChosenColor(QColor&);
    /**
     * @brief setId
     * @param id
     */
    void setId(QString id);
    /**
     * @brief removeItemFromScene
     */
    void removeItemFromScene(QString, bool sendToAll= true, bool undoable= true);
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
    void readMessage(NetworkMessageReader& msg, bool readCharacter= true);
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
     * @brief cleanFogEdition
     */
    void cleanFogEdition();
    /**
     * @brief changeStackOrder triggered when VisualItem should change its z order. It recompute the zvalue of all
     * items.
     * @param item to move
     * @param op operation to be done.
     */
    // void changeStackOrder(VisualItem* item, VectorialMapController::StackOrder op);
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
    // void processLayerMessage(NetworkMessageReader* msg);
    /**
     * @brief ownerHasChangedForCharacterItem
     * @param item
     * @param cItem
     */
    void ownerHasChangedForCharacterItem(Character* item, CharacterItem* cItem);
    /**
     * @brief selectionPositionHasChanged
     */
    // void selectionPositionHasChanged();
    void showTransparentItems();
    // void selectionPositionAboutToChange();
    void cleanUpInit(Core::CharacterScope zone);
    void rollInit(Core::CharacterScope zone);
    // void addItem(VisualItem* item);
signals:
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
    void sendOffItem(VisualItem* item, bool doInitPoint= true);
    /**
     * @brief updateItem
     */
    void updateItem(const QPointF& end);
    /**
     * @brief checkItemLayer
     */
    void checkItemLayer(VisualItem*);
    /**
     * @brief promoteItemInType
     */
    void promoteItemInType(VisualItem*, vmap::VisualItemController::ItemType);
    /**
     * @brief insertCharacterInMap
     * @param item
     */
    void insertCharacterInMap(CharacterItem* item);
    /**
     * @brief characterHasBeenDeleted
     */
    void characterHasBeenDeleted(Character*);

protected:
    /**
     * @brief catches move event with the mouse, useful for allowing move of item
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    /**
     * @brief catches mouse Press event, defines the first point of the next added item.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    /**
     * @brief  catches release event, defines the last point of the next added item.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
    /**
     * @brief keyPressEvent
     * @param event
     */
    virtual void keyPressEvent(QKeyEvent* event);
    /**
     * @brief adds item depending of the current tool.
     */
    void insertItem(const QPointF& end);

    /**
     * @brief dragEnterEvent
     * @param event
     */
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent(QGraphicsSceneDragDropEvent* event);
    /**
     * @brief dragMoveEvent
     * @param event
     */
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
    /**
     * @brief ensureFogAboveAll
     */
    void ensureFogAboveAll();
    bool isItemStorable(VisualItem* item);

    void addRectItem(vmap::RectController* rectCtrl, bool editing);
    void addEllipseItem(vmap::EllipseController* ellisCtrl, bool editing);
    void addLineItem(vmap::LineController* lineCtrl, bool editing);
    void addImageItem(vmap::ImageController* imgCtrl);
    void addPathItem(vmap::PathController* pathCtrl, bool editing);
    void addTextItem(vmap::TextController* textCtrl);
    void addCharaterItem(vmap::CharacterItemController* itemCtrl);

    void addAndInit(QGraphicsItem* item);

    void addVisualItem(vmap::VisualItemController* ctrl);

private:
    QPointer<VectorialMapController> m_ctrl;
    /**
     * @brief current selected item
     */
    QPointer<VisualItem> m_currentItem;
    QPointer<AnchorItem> m_parentItemAnchor;
    QPointer<RuleItem> m_ruleItem;
    /**
     * @brief m_currentPath
     */
    QPointer<PathItem> m_currentPath= nullptr;
    /**
     * @brief Items list which are part of the map.
     */
    QMap<QString, VisualItem*>* m_itemMap= nullptr;
    /**
     * @brief m_sortedItemList
     */
    QStringList m_sortedItemList; // sorted by time of insertion
    /**
     * @brief m_orderedItemList is used to store zorder of all items.
     */
    QList<VisualItem*> m_orderedItemList;
    /**
     * @brief Items list which are part of the map.
     */
    QMap<QString, CharacterItem*>* m_characterItemMap= nullptr;
    /**
     * @brief m_computedPattern
     */
    QImage m_computedPattern;
    /**
     * @brief m_id
     */
    QString m_id;
    /**
     * @brief m_localUserId
     */
    QString m_localUserId;

    SightItem* m_sightItem= nullptr;
    GridItem* m_gridItem= nullptr;
    VisualItem* m_fogItem= nullptr;
    quint64 m_zIndex;

    QPointer<QUndoStack> m_undoStack;
    AddVmapItemCommand* m_currentAddCmd= nullptr;
    QList<VisualItem*> m_movingItems;
    QList<QPointF> m_oldPos;
};
#endif // VMAP_H
