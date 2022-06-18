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
#include "rwidgets/graphicsItems/griditem.h"
#include "rwidgets/graphicsItems/pathitem.h"
#include "rwidgets/graphicsItems/sightitem.h"
#include "rwidgets/graphicsItems/visualitem.h"
#include "rwidgets/toolbars/vtoolbar.h"
#include "rwidgets_global.h"

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
class RWIDGET_EXPORT VMap : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit VMap(VectorialMapController* ctrl, QObject* parent= nullptr);
    QRectF itemsBoundingRectWithoutSight();
    void manageAnchor();
    bool isIdle() const;
    void setAnchor(QGraphicsItem* child, QGraphicsItem* parent);
    void updateLayer();
    const QString& getLocalUserId() const;
    int getCurrentNpcNumber() const;
    void removeItemFromData(VisualItem* item);
    void setUndoStack(QUndoStack* undoStack);
    void addCommand(QUndoCommand* cmd);
    bool isNormalItem(const QGraphicsItem* item);

public slots:
    void computePattern();
    void duplicateItem(VisualItem* item);
    QList<CharacterItem*> getCharacterOnMap(QString id);
    void setCurrentItemOpacity(qreal);
    void selectionHasChanged();
    void ownerHasChangedForCharacterItem(Character* item, CharacterItem* cItem);
    void showTransparentItems();
    // void cleanUpInit(Core::CharacterScope zone);
    // void rollInit(Core::CharacterScope zone);
signals:
    void mapChanged();
    void mapStatutChanged();
    void currentItemOpacity(qreal);
    void runDiceCommandForCharacter(QString cmd, QString uuid);
private slots:
    void updateItem(const QPointF& end);
    void checkItemLayer(VisualItem*);
    void promoteItemInType(VisualItem*, vmap::VisualItemController::ItemType);
    void insertCharacterInMap(CharacterItem* item);

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
    virtual void keyPressEvent(QKeyEvent* event);
    void insertItem(const QPointF& end);
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
    void dropEvent(QGraphicsSceneDragDropEvent* event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
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
    void addExistingItems();

private:
    QPointer<GridItem> m_gridItem;
    QPointer<SightItem> m_sightItem;
    QPointer<VectorialMapController> m_ctrl;
    QPointer<VisualItem> m_currentItem;
    QPointer<AnchorItem> m_parentItemAnchor;
    QPointer<RuleItem> m_ruleItem;
    QPointer<PathItem> m_currentPath= nullptr;
    QMap<QString, VisualItem*>* m_itemMap= nullptr;
    QStringList m_sortedItemList; // sorted by time of insertion
    QList<VisualItem*> m_orderedItemList;
    QMultiMap<QString, CharacterItem*>* m_characterItemMap= nullptr;
    QImage m_computedPattern;
    QPointer<QUndoStack> m_undoStack;
    AddVmapItemCommand* m_currentAddCmd= nullptr;
    QList<VisualItem*> m_movingItems;
    QList<QPointF> m_oldPos;
};
#endif // VMAP_H
