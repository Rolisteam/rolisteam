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
class ImageItemController;
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

    void manageAnchor();
    const QString& getLocalUserId() const;
    bool isNormalItem(const QGraphicsItem* item) const;
    VisualItem* getNormalItem(QGraphicsItem* item);

    GridItem* gridItem() const;

public slots:
    void computePattern();

private slots:
    void updateItem(const QPointF& end);
    void promoteItemInType(VisualItem*, vmap::VisualItemController::ItemType);

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void dropEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

    void insertItem(const QPointF& end);
    void addAndInit(QGraphicsItem* item);
    void addVisualItem(vmap::VisualItemController* ctrl);
    void addExistingItems();
    void addRectItem(vmap::RectController* rectCtrl, bool editing);
    void addEllipseItem(vmap::EllipseController* ellisCtrl, bool editing);
    void addLineItem(vmap::LineController* lineCtrl, bool editing);
    void addImageItem(vmap::ImageItemController* imgCtrl);
    void addPathItem(vmap::PathController* pathCtrl, bool editing);
    void addTextItem(vmap::TextController* textCtrl);
    void addCharaterItem(vmap::CharacterItemController* itemCtrl);

private:
    VisualItem* visualItemUnder(const QPointF& pos);
    void resetCurrentPath();

private:
    std::unique_ptr<GridItem> m_gridItem;
    QPointer<SightItem> m_sightItem;
    QPointer<VectorialMapController> m_ctrl;
    QPointer<VisualItem> m_currentItem;
    QPointer<AnchorItem> m_parentItemAnchor;
    QPointer<RuleItem> m_ruleItem;
    QPointer<PathItem> m_currentPath;
    QList<VisualItem*> m_movingItems;
    QList<QPointF> m_oldPos;
};
#endif // VMAP_H
