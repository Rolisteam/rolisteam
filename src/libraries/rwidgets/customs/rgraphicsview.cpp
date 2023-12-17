/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
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

#include <QDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QOpenGLWidget>

#include "controller/view_controller/imageselectorcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "mediacontainers/mediacontainer.h"
#include "rgraphicsview.h"
#include "rwidgets/dialogs/imageselectordialog.h"

bool isNormalItem(const vmap::VisualItemController* itemCtrl)
{
    if(!itemCtrl)
        return false;

    auto layer= itemCtrl->layer();
    return !(layer == Core::Layer::GRIDLAYER || layer == Core::Layer::FOG);
}

RGraphicsView::RGraphicsView(VectorialMapController* ctrl, QWidget* parent)
    : QGraphicsView(parent), m_ctrl(ctrl), m_centerOnItem(nullptr)
{
    m_counterZoom= 0;

    setAcceptDrops(true);
    setAlignment((Qt::AlignLeft | Qt::AlignTop));
    m_preferences= PreferencesManager::getInstance();
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // fitInView(sceneRect(),Qt::KeepAspectRatio);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    createAction();
}

void RGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if(m_ctrl->tool() != Core::HANDLER)
    {
        setDragMode(QGraphicsView::NoDrag);
        QGraphicsView::mousePressEvent(event);
        return;
    }

    // if(event->button() == Qt::RightButton)
    //        return;

    if(event->button() == Qt::LeftButton)
    {
        QList<QGraphicsItem*> list= items(event->pos());

        list.erase(std::remove_if(list.begin(), list.end(),
                                  [](const QGraphicsItem* item)
                                  { return !isNormalItem(dynamic_cast<const vmap::VisualItemController*>(item)); }),
                   list.end());
        if(!list.isEmpty() && event->modifiers() == Qt::NoModifier)
        {
            setDragMode(QGraphicsView::NoDrag);
        }
        else if(event->modifiers() & Qt::ShiftModifier)
        {
            setDragMode(QGraphicsView::ScrollHandDrag);
            // m_lastPoint= mapToScene(event->pos());
        }
        else
        {
            setDragMode(QGraphicsView::RubberBandDrag);
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void RGraphicsView::wheelEvent(QWheelEvent* event)
{
    if(event->modifiers() & Qt::ShiftModifier)
    {
        double angle= event->angleDelta().y();
        double factor= qPow(1.0015, angle);

        auto targetViewportPos= event->position();
        auto targetScenePos= event->scenePosition();
        m_ctrl->setZoomLevel(m_ctrl->zoomLevel() * factor);
        scale(factor, factor);
        centerOn(targetScenePos);
        QPointF deltaViewportPos= targetViewportPos - QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);
        QPointF viewportCenter= mapFromScene(targetScenePos) - deltaViewportPos;
        centerOn(mapToScene(viewportCenter.toPoint()));

        return;
    }
    else
        QGraphicsView::wheelEvent(event);
}

QList<ItemToControllerInfo> extractVisualItem(const QList<QGraphicsItem*>& itemList, bool upChild= true)
{
    QList<ItemToControllerInfo> list;
    for(QGraphicsItem* item : itemList)
    {
        VisualItem* vItem= dynamic_cast<VisualItem*>(item);
        if(!vItem && upChild)
        {
            ChildPointItem* childItem= dynamic_cast<ChildPointItem*>(item);
            if(nullptr != childItem)
            {
                QGraphicsItem* item2= childItem->parentItem();
                vItem= dynamic_cast<VisualItem*>(item2);
            }
        }

        if(nullptr == vItem)
            continue;

        auto ctrl= vItem->controller();
        bool isNormal= isNormalItem(ctrl);

        auto it= std::find_if(std::begin(list), std::end(list),
                              [ctrl](const ItemToControllerInfo& item) { return ctrl == item.ctrl; });

        if(isNormal && (it == std::end(list)))
        {
            list.append({ctrl, vItem});
        }
    }
    return list;
}

QList<vmap::VisualItemController*> convertInfoToCtrl(const QList<ItemToControllerInfo>& selectedItems)
{
    QList<vmap::VisualItemController*> ctrls;
    std::transform(std::begin(selectedItems), std::end(selectedItems), std::back_inserter(ctrls),
                   [](const ItemToControllerInfo& item) { return item.ctrl; });

    return ctrls;
}

QList<ItemToControllerInfo> RGraphicsView::selection()
{
    return extractVisualItem(scene()->selectedItems());
}

void RGraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
    bool licenseToModify= false;

    m_menuPoint= event->pos();

    if((m_ctrl->localGM()) || (m_ctrl->permission() == Core::PC_ALL))
    {
        licenseToModify= true;
    }

    if(!m_ctrl->idle())
        QGraphicsView::contextMenuEvent(event);

    auto selectedItems= selection();
    QList<vmap::VisualItemController*> underMouseCtrls;
    QList<ItemToControllerInfo> visualItemUnderMouse;
    if(selectedItems.isEmpty())
    {
        auto itemUnderMouse= items(m_menuPoint);
        visualItemUnderMouse= extractVisualItem(itemUnderMouse);

        if(!visualItemUnderMouse.isEmpty())
        {
            selectedItems.append(visualItemUnderMouse.first());
        }
        std::transform(std::begin(visualItemUnderMouse), std::end(visualItemUnderMouse),
                       std::back_inserter(underMouseCtrls), [](const ItemToControllerInfo& item) { return item.ctrl; });
    }
    QMenu menu;
    auto parentWid= dynamic_cast<MediaContainer*>(parentWidget());
    // Empty list

    QAction* resetRotationAct= nullptr;
    QAction* rightRotationAct= nullptr;
    QAction* leftRotationAct= nullptr;
    QAction* angleRotationAct= nullptr;

    if(licenseToModify && !selectedItems.isEmpty())
    {
        int n= selectedItems.size();
        menu.addSection(tr("%n item(s)", "", n));

        bool groundL= false, objectL= false, characterL= false, gmLayer= false;
        if(selectedItems.size() == 1)
        {
            auto item= selectedItems.first();
            item.item->addActionContextMenu(menu);

            auto layer= item.ctrl->layer();
            switch(layer)
            {
            case Core::Layer::GROUND:
                groundL= true;
                break;
            case Core::Layer::OBJECT:
                objectL= true;
                break;
            case Core::Layer::CHARACTER_LAYER:
                characterL= true;
                break;
            case Core::Layer::GAMEMASTER_LAYER:
                gmLayer= true;
                break;
            default:
                break;
            }
        }

        menu.addAction(m_dupplicate);

        auto overlapping= menu.addMenu(tr("Overlapping"));
        overlapping->addAction(m_backOrderAction);
        overlapping->addAction(m_lowerAction);
        overlapping->addAction(m_raiseAction);
        overlapping->addAction(m_frontOrderAction);

        QMenu* rotationMenu= menu.addMenu(tr("Rotate"));
        resetRotationAct= rotationMenu->addAction(tr("To 360"));
        rightRotationAct= rotationMenu->addAction(tr("Right"));
        leftRotationAct= rotationMenu->addAction(tr("Left"));
        angleRotationAct= rotationMenu->addAction(tr("Set Angle…"));

        QMenu* setLayerMenu= menu.addMenu(tr("Set Layer"));
        setLayerMenu->addAction(m_putGroundLayer);
        m_putGroundLayer->setChecked(groundL);
        setLayerMenu->addAction(m_putObjectLayer);
        m_putObjectLayer->setChecked(objectL);
        setLayerMenu->addAction(m_putCharacterLayer);
        m_putCharacterLayer->setChecked(characterL);
        setLayerMenu->addAction(m_putGMLayer);
        m_putGMLayer->setChecked(gmLayer);

        QMenu* harmonizeMenu= menu.addMenu(tr("Normalize Size"));
        harmonizeMenu->addAction(m_normalizeSizeAverage);
        harmonizeMenu->addAction(m_normalizeSizeUnderMouse);
        harmonizeMenu->addAction(m_normalizeSizeBigger);
        harmonizeMenu->addAction(m_normalizeSizeSmaller);

        menu.addAction(m_lockSize);
        menu.addAction(m_removeSelection);
    }

    menu.addSection(tr("Map"));

    switch(m_ctrl->layer())
    {
    case Core::Layer::OBJECT:
        m_editObjectLayer->setChecked(true);
        break;
    case Core::Layer::GROUND:
        m_editGroundLayer->setChecked(true);
        break;
    case Core::Layer::CHARACTER_LAYER:
        m_editCharacterLayer->setChecked(true);
        break;
    default:
        break;
    }
    if(licenseToModify)
    {
        QMenu* editLayer= menu.addMenu(tr("Edit Layer"));
        editLayer->addAction(m_editGroundLayer);
        editLayer->addAction(m_editObjectLayer);
        editLayer->addAction(m_editCharacterLayer);
        editLayer->addAction(m_editGameMasterLayer);

        QMenu* changeVibility= menu.addMenu(tr("Change Visibility"));
        changeVibility->addAction(m_hiddenVisibility);
        changeVibility->addAction(m_characterVisibility);
        changeVibility->addAction(m_allVisibility);

        QMenu* rollInit= menu.addMenu(tr("Roll Init"));
        rollInit->addAction(m_rollInitOnAllNpc);
        rollInit->addAction(m_rollInitOnAllCharacter);
        if(!selectedItems.isEmpty())
            rollInit->addAction(m_rollInitOnSelection);

        QMenu* cleanInit= menu.addMenu(tr("Clear Init"));
        cleanInit->addAction(m_cleanInitOnAllNpc);
        cleanInit->addAction(m_cleanInitOnAllCharacter);
        if(!selectedItems.isEmpty())
            cleanInit->addAction(m_cleanInitOnSelection);
    }
    auto zoom= menu.addMenu(tr("Zoom"));
    zoom->addAction(m_zoomIn);
    zoom->addAction(m_zoomOut);
    zoom->addAction(m_zoomInMax);
    zoom->addAction(m_zoomNormal);
    zoom->addAction(m_zoomOutMax);
    zoom->addAction(m_zoomCenterOnItem);
    m_centerOnItem= itemAt(m_menuPoint);
    if(nullptr == m_centerOnItem)
    {
        m_zoomCenterOnItem->setVisible(false);
    }
    else
    {
        m_zoomCenterOnItem->setVisible(true);
    }
    if(licenseToModify)
    {
        menu.addSeparator();
        menu.addAction(m_importImage);
        menu.addSeparator();
    }

    if(nullptr != parentWid)
    {
        parentWid->addActionToMenu(menu);
    }
    if(licenseToModify)
    {
        menu.addAction(m_properties);
    }

    QAction* selectedAction= menu.exec(event->globalPos());

    if(nullptr == selectedAction)
        return;

    if(selectedItems.empty())
        return;

    QList<vmap::VisualItemController*> ctrls= convertInfoToCtrl(selectedItems);

    if(resetRotationAct == selectedAction)
    {
        setRotation(ctrls, 0);
    }
    else if(selectedAction == rightRotationAct)
    {
        setRotation(ctrls, 90);
    }
    else if(selectedAction == leftRotationAct)
    {
        setRotation(ctrls, 270);
    }
    else if(m_dupplicate == selectedAction)
    {
        m_ctrl->dupplicateItem(ctrls);
    }
    else if(selectedAction == m_lockSize)
    {
        lockItems(ctrls);
    }
    else if(selectedAction == angleRotationAct)
    {
        int angle= QInputDialog::getInt(this, tr("Rotation Value ?"),
                                        tr("Please, set the rotation angle you want [0-360]"), 0, 0, 360);
        setRotation(ctrls, angle);
    }
    else if(m_normalizeSizeBigger == selectedAction || m_normalizeSizeAverage == selectedAction
            || m_normalizeSizeUnderMouse == selectedAction || m_normalizeSizeSmaller == selectedAction)
    {
        m_ctrl->normalizeSize(ctrls, static_cast<VectorialMapController::Method>(selectedAction->data().toInt()),
                              mapToScene(m_menuPoint));
    }
    else if((m_backOrderAction == selectedAction) || (m_frontOrderAction == selectedAction)
            || (m_lowerAction == selectedAction) || (m_raiseAction == selectedAction))
    {
        if(!selectedItems.isEmpty())
        {
            auto first= selectedItems[0];
            auto order= static_cast<VectorialMapController::StackOrder>(selectedAction->data().toInt());
            auto under= items(first.item->boundingRect().toRect().translated(first.item->pos().toPoint()));
            auto visualItem= extractVisualItem(under);

            if(order == VectorialMapController::StackOrder::RAISE)
            {
                bool startChanges= false;
                QList<ItemToControllerInfo> res;
                for(auto it= visualItem.rbegin(); it != visualItem.rend(); ++it)
                {
                    auto i= *it;
                    if(i.item == first.item)
                    {
                        startChanges= true;
                        continue;
                    }
                    if(!startChanges)
                        continue;
                    res << i;
                    // i.item->stackBefore(first.item);
                }
                stackBefore({res}, {first});
            }
            else if(order == VectorialMapController::StackOrder::LOWER)
            {
                QList<ItemToControllerInfo> res;
                for(auto i : visualItem)
                {
                    if(i.item == first.item)
                        continue;
                    // first.item->stackBefore(i.item);
                    res << i;
                }
                stackBefore({first}, {res});
            }
            else if(order == VectorialMapController::StackOrder::BACK)
            {
                auto sortedItems= extractVisualItem(items(), false);
                if(sortedItems.empty())
                    return;
                stackBefore({first}, {sortedItems.last()});
                // first.item->stackBefore(sortedItems.last().item);
            }
            else if(order == VectorialMapController::StackOrder::FRONT)
            {
                auto sortedItems= extractVisualItem(items(), false);
                if(sortedItems.empty())
                    return;
                bool startChanges= false;
                QList<ItemToControllerInfo> res;
                for(auto it= sortedItems.rbegin(); it != sortedItems.rend(); ++it)
                {
                    auto i= *it;
                    if(i.item == first.item)
                    {
                        startChanges= true;
                        continue;
                    }
                    if(!startChanges)
                        continue;
                    // i.item->stackBefore(first.item);
                    res << i;
                }
                stackBefore({res}, {first});
            }
        }
    }
    else if((selectedAction == m_putCharacterLayer) || (selectedAction == m_putObjectLayer)
            || (selectedAction == m_putGroundLayer) || (selectedAction == m_putGMLayer))
    {
        setItemLayer(ctrls, static_cast<Core::Layer>(selectedAction->data().toInt()));
    }
}
void RGraphicsView::centerOnItem()
{
    if(!m_centerOnItem)
        return;

    QRectF rect= m_centerOnItem->mapToScene(m_centerOnItem->boundingRect()).boundingRect();
    QRectF rect2= mapToScene(sceneRect().toRect()).boundingRect();

    if(rect2.contains(rect))
        return;

    qreal dx= rect.center().x() - rect2.center().x();
    qreal dy= rect.center().y() - rect2.center().y();

    rect2.translate(dx, dy);
    setSceneRect(rect2);
}

void RGraphicsView::setRotation(const QList<vmap::VisualItemController*>& list, int value)
{
    std::for_each(list.begin(), list.end(), [value](vmap::VisualItemController* ctrl) { ctrl->setRotation(value); });
}

void RGraphicsView::lockItems(const QList<vmap::VisualItemController*>& list)
{
    auto locked= std::all_of(list.begin(), list.end(), [](vmap::VisualItemController* ctrl) { return ctrl->locked(); });
    bool val= locked ? false : true;
    std::for_each(list.begin(), list.end(), [val](vmap::VisualItemController* ctrl) { ctrl->setLocked(val); });
}

void RGraphicsView::setItemLayer(const QList<vmap::VisualItemController*>& list, Core::Layer layer)
{
    std::for_each(list.begin(), list.end(), [layer](vmap::VisualItemController* ctrl) { ctrl->setLayer(layer); });
}

void RGraphicsView::createAction()
{
    // ZOOM MANAGEMENT
    m_zoomNormal= new QAction(tr("Zoom to Normal"), this);
    m_zoomInMax= new QAction(tr("Zoom In Max"), this);
    m_zoomOutMax= new QAction(tr("Zoom Out Max"), this);
    m_zoomCenterOnItem= new QAction(tr("Center on Item"), this);

    m_zoomIn= new QAction(tr("Zoom In"), this);
    m_zoomIn->setShortcut(QKeySequence("+"));

    m_zoomOut= new QAction(tr("Zoom Out"), this);
    m_zoomOut->setShortcut(QKeySequence("-"));

    m_removeSelection= new QAction(tr("Remove"), this);
    m_removeSelection->setShortcut(QKeySequence(QKeySequence::Delete));
    addAction(m_removeSelection);
    connect(m_removeSelection, &QAction::triggered, this,
            [this]()
            {
                auto selec= selection();
                m_ctrl->aboutToRemove(convertInfoToCtrl(selec));
            });

    m_importImage= new QAction(tr("Import Image"), this);

    connect(m_zoomNormal, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_zoomCenterOnItem, SIGNAL(triggered(bool)), this, SLOT(centerOnItem()));
    connect(m_zoomInMax, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_zoomOutMax, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_importImage, SIGNAL(triggered()), this, SLOT(addImageToMap()));
    connect(m_zoomOut, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_zoomIn, SIGNAL(triggered()), this, SLOT(setZoomFactor()));

    m_backOrderAction= new QAction(tr("Back"));
    m_backOrderAction->setIcon(QIcon::fromTheme("action-order-back"));
    m_backOrderAction->setData(VectorialMapController::BACK);

    m_frontOrderAction= new QAction(tr("Front"));
    m_frontOrderAction->setIcon(QIcon::fromTheme("action-order-front"));
    m_frontOrderAction->setData(VectorialMapController::FRONT);

    m_lowerAction= new QAction(tr("Lower"));
    m_lowerAction->setIcon(QIcon::fromTheme("action-order-lower"));
    m_lowerAction->setData(VectorialMapController::LOWER);

    m_raiseAction= new QAction(tr("Raise"));
    m_raiseAction->setIcon(QIcon::fromTheme("action-order-raise"));
    m_raiseAction->setData(VectorialMapController::RAISE);

    m_normalizeSizeAverage= new QAction(tr("Average"), this);
    m_normalizeSizeAverage->setData(VectorialMapController::Average);

    m_normalizeSizeUnderMouse= new QAction(tr("As undermouse item"), this);
    m_normalizeSizeUnderMouse->setData(VectorialMapController::UnderMouse);

    m_normalizeSizeBigger= new QAction(tr("As the Bigger"), this);
    m_normalizeSizeBigger->setData(VectorialMapController::Bigger);

    m_normalizeSizeSmaller= new QAction(tr("As the Smaller"), this);
    m_normalizeSizeSmaller->setData(VectorialMapController::Smaller);

    m_lockSize= new QAction(tr("Lock/Unlock Item Geometry"), this);
    m_dupplicate= new QAction(tr("Duplicate Item"), this);

    addAction(m_zoomNormal);
    addAction(m_zoomInMax);
    addAction(m_zoomOutMax);
    addAction(m_zoomOut);
    addAction(m_zoomIn);
    addAction(m_dupplicate);

    // PROPERTIES
    m_properties= new QAction(tr("Properties"), this);
    // connect(m_properties, &QAction::triggered, this, &RGraphicsView::showMapProperties);

    // Layers
    QActionGroup* group= new QActionGroup(this);
    m_editGroundLayer= new QAction(tr("Ground"), this);
    m_editGroundLayer->setData(static_cast<int>(Core::Layer::GROUND));
    m_editGroundLayer->setCheckable(true);
    m_editObjectLayer= new QAction(tr("Object"), this);
    m_editObjectLayer->setData(static_cast<int>(Core::Layer::OBJECT));
    m_editObjectLayer->setCheckable(true);
    m_editCharacterLayer= new QAction(tr("Character"), this);
    m_editCharacterLayer->setData(static_cast<int>(Core::Layer::CHARACTER_LAYER));
    m_editCharacterLayer->setCheckable(true);
    m_editGameMasterLayer= new QAction(tr("GameMaster"), this);
    m_editGameMasterLayer->setData(static_cast<int>(Core::Layer::GAMEMASTER_LAYER));
    m_editGameMasterLayer->setCheckable(true);

    group->addAction(m_editGroundLayer);
    group->addAction(m_editObjectLayer);
    group->addAction(m_editCharacterLayer);
    group->addAction(m_editGameMasterLayer);

    m_putGroundLayer= new QAction(tr("Ground"), this);
    m_putGroundLayer->setData(static_cast<int>(Core::Layer::GROUND));
    m_putGroundLayer->setCheckable(true);
    m_putObjectLayer= new QAction(tr("Object"), this);
    m_putObjectLayer->setData(static_cast<int>(Core::Layer::OBJECT));
    m_putObjectLayer->setCheckable(true);
    m_putCharacterLayer= new QAction(tr("Character"), this);
    m_putCharacterLayer->setData(static_cast<int>(Core::Layer::CHARACTER_LAYER));
    m_putCharacterLayer->setCheckable(true);
    m_putGMLayer= new QAction(tr("GameMaster"), this);
    m_putGMLayer->setData(static_cast<int>(Core::Layer::GAMEMASTER_LAYER));
    m_putGMLayer->setCheckable(true);

    connect(m_editGroundLayer, &QAction::triggered, this, &RGraphicsView::changeLayer);
    connect(m_editObjectLayer, &QAction::triggered, this, &RGraphicsView::changeLayer);
    connect(m_editCharacterLayer, &QAction::triggered, this, &RGraphicsView::changeLayer);
    connect(m_editGameMasterLayer, &QAction::triggered, this, &RGraphicsView::changeLayer);

    QActionGroup* group2= new QActionGroup(this);
    m_allVisibility= new QAction(tr("All"), this);
    m_allVisibility->setCheckable(true);
    m_allVisibility->setData(Core::ALL);
    m_hiddenVisibility= new QAction(tr("Hidden"), this);
    m_hiddenVisibility->setCheckable(true);
    m_hiddenVisibility->setChecked(true);
    m_hiddenVisibility->setData(Core::HIDDEN);
    m_characterVisibility= new QAction(tr("Fog Of War"), this);
    m_characterVisibility->setData(Core::FOGOFWAR);
    m_characterVisibility->setCheckable(true);

    group2->addAction(m_allVisibility);
    group2->addAction(m_hiddenVisibility);
    group2->addAction(m_characterVisibility);

    connect(m_allVisibility, &QAction::triggered, this, &RGraphicsView::changeVisibility);
    connect(m_hiddenVisibility, &QAction::triggered, this, &RGraphicsView::changeVisibility);
    connect(m_characterVisibility, &QAction::triggered, this, &RGraphicsView::changeVisibility);

    m_rollInitOnAllNpc= new QAction(tr("All Npcs"), this);
    m_rollInitOnAllNpc->setToolTip(tr("Roll Initiative on All Npcs"));
    m_rollInitOnAllNpc->setData(Core::CharacterScope::AllNPC);
    connect(m_rollInitOnAllNpc, &QAction::triggered, this, &RGraphicsView::rollInit);

    m_rollInitOnSelection= new QAction(tr("Selection"), this);
    m_rollInitOnSelection->setToolTip(tr("Roll Initiative on Selection"));
    m_rollInitOnSelection->setData(Core::CharacterScope::SelectionOnly);
    connect(m_rollInitOnSelection, &QAction::triggered, this, &RGraphicsView::rollInit);

    m_rollInitOnAllCharacter= new QAction(tr("All Characters"), this);
    m_rollInitOnAllCharacter->setToolTip(tr("Roll Initiative on All Characters"));
    m_rollInitOnAllCharacter->setData(Core::CharacterScope::AllCharacter);
    connect(m_rollInitOnAllCharacter, &QAction::triggered, this, &RGraphicsView::rollInit);

    m_cleanInitOnAllNpc= new QAction(tr("All Npcs"), this);
    m_cleanInitOnAllNpc->setToolTip(tr("Clear Initiative on All Npcs"));
    m_cleanInitOnAllNpc->setData(Core::CharacterScope::AllNPC);
    connect(m_cleanInitOnAllNpc, &QAction::triggered, this, &RGraphicsView::cleanInit);

    m_cleanInitOnSelection= new QAction(tr("Selection"), this);
    m_cleanInitOnSelection->setToolTip(tr("Clear Initiative on Selection"));
    m_cleanInitOnSelection->setData(Core::CharacterScope::SelectionOnly);
    connect(m_cleanInitOnSelection, &QAction::triggered, this, &RGraphicsView::cleanInit);

    m_cleanInitOnAllCharacter= new QAction(tr("All Characters"), this);
    m_cleanInitOnAllCharacter->setToolTip(tr("Clear Initiative on All Characters"));
    m_cleanInitOnAllCharacter->setData(Core::CharacterScope::AllCharacter);
    connect(m_cleanInitOnAllCharacter, &QAction::triggered, this, &RGraphicsView::cleanInit);
}

void RGraphicsView::rollInit()
{
    auto act= qobject_cast<QAction*>(sender());
    m_ctrl->rollInit(act->data().value<Core::CharacterScope>());
}
void RGraphicsView::cleanInit()
{
    auto act= qobject_cast<QAction*>(sender());
    m_ctrl->cleanUpInit(act->data().value<Core::CharacterScope>());
}

void RGraphicsView::changeLayer()
{
    QAction* act= qobject_cast<QAction*>(sender());

    if(static_cast<Core::Layer>(act->data().toInt()) != m_ctrl->layer())
    {
        m_ctrl->setLayer(static_cast<Core::Layer>(act->data().toInt()));
    }
}
void RGraphicsView::changeVisibility()
{
    QAction* act= qobject_cast<QAction*>(sender());
    m_ctrl->setVisibility(static_cast<Core::VisibilityMode>(act->data().toInt()));
}

void RGraphicsView::setZoomFactor()
{
    QAction* senderAct= qobject_cast<QAction*>(sender());
    int destination= 0;
    int step= 0;
    if(senderAct == m_zoomInMax)
    {
        destination= 20;
        step= 1;
    }
    else if(senderAct == m_zoomNormal)
    {
        destination= 0;
        if(m_counterZoom > 0)
        {
            step= -1;
        }
        else
        {
            step= 1;
        }
    }
    else if(senderAct == m_zoomOutMax)
    {
        destination= -20;
        step= -1;
    }
    else if(senderAct == m_zoomIn)
    {
        destination= m_counterZoom + 1;
        step= 1;
    }
    else if(senderAct == m_zoomOut)
    {
        destination= m_counterZoom - 1;
        step= -1;
    }
    if(destination > 20)
    {
        destination= 20;
    }
    else if(destination < -20)
    {
        destination= -20;
    }
    double scaleFactor= 1.1;
    double realFactor= 1.0;
    while(destination != m_counterZoom)
    {
        if(step > 0)
        {
            realFactor= realFactor * scaleFactor;
        }
        else
        {
            realFactor= realFactor * (1.0 / scaleFactor);
        }
        m_counterZoom+= step;
    }
    scale(realFactor, realFactor);
    updateSizeToController();
}
void RGraphicsView::currentToolChanged(Core::SelectableTool selectedtool)
{
    m_currentTool= selectedtool;
}

void RGraphicsView::updateSizeToController()
{
    emit updateVisualZone();
    // auto rect= frameRect();
    // auto poly= mapToScene(rect);
    // m_ctrl->setVisualRect(poly.boundingRect());
}

void RGraphicsView::stackBefore(const QList<ItemToControllerInfo>& first, const QList<ItemToControllerInfo>& second)
{
    m_ctrl->addCommand(new ChangeStackOrderVMapCommand(m_ctrl, first, second));
}

void RGraphicsView::resizeEvent(QResizeEvent* event)
{
    updateSizeToController();

    setResizeAnchor(QGraphicsView::NoAnchor);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    QGraphicsView::resizeEvent(event);
}

void RGraphicsView::addImageToMap()
{
    ImageSelectorController ctrl(false, ImageSelectorController::All, ImageSelectorController::AnyShape,
                                 m_preferences->value("ImageDirectory", QDir::homePath()).toString());
    ImageSelectorDialog dialog(&ctrl, this);

    if(QDialog::Accepted != dialog.exec())
        return;

    // TODO use the image
}
