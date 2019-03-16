/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
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

#include <QOpenGLWidget>

#include "data/character.h"
#include "data/mediacontainer.h"
#include "data/person.h"
#include "rgraphicsview.h"
#include "userlist/rolisteammimedata.h"

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "items/ruleitem.h"
#include "undoCmd/changesizevmapitem.h"

RGraphicsView::RGraphicsView(VMap* vmap, QWidget* parent)
    : QGraphicsView(vmap, parent), m_vmap(vmap), m_centerOnItem(nullptr)
{
    m_counterZoom= 0;

    if(nullptr != m_vmap)
    {
        connect(m_vmap, SIGNAL(mapChanged()), this, SLOT(sendOffMapChange()));
    }
    setAcceptDrops(true);
    m_preferences= PreferencesManager::getInstance();
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // setViewport(new QOpenGLWidget());
    // fitInView(sceneRect(),Qt::KeepAspectRatio);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    createAction();

    vmap->initScene();
}

void RGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if(m_currentTool == VToolsBar::HANDLER)
    {
        if(event->button() == Qt::LeftButton)
        {
            QList<QGraphicsItem*> list= items(event->pos());
            if(nullptr != m_vmap)
            {
                // list.removeAll(m_vmap->getFogItem());
                list.erase(std::remove_if(list.begin(), list.end(),
                               [this](const QGraphicsItem* item) { return !m_vmap->isNormalItem(item); }),
                    list.end());
                bool rubber= true;
                if(!list.isEmpty())
                {
                    rubber= false;
                }
                if(!rubber)
                {
                    setDragMode(QGraphicsView::NoDrag);
                }
                else
                {
                    setDragMode(QGraphicsView::RubberBandDrag);
                }
            }
        }
        else if(event->button() == Qt::RightButton)
        {
            return;
        }
    }
    QGraphicsView::mousePressEvent(event);
}
void RGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    m_lastPoint= QPoint();
    QGraphicsView::mouseReleaseEvent(event);
    if(dragMode() == QGraphicsView::ScrollHandDrag)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
}
void RGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    if((VToolsBar::HANDLER == m_currentTool) && (event->modifiers() & Qt::ShiftModifier)
        && (event->buttons() & Qt::LeftButton) && (dragMode() == QGraphicsView::RubberBandDrag))
    {
        if(!m_lastPoint.isNull())
        {
            QRectF rect= sceneRect();
            int dx= event->x() - m_lastPoint.x();
            int dy= event->y() - m_lastPoint.y();
            rect.translate(-dx, -dy);
            m_lastPoint= event->pos();
            setSceneRect(rect);
        }
        m_lastPoint= event->pos();
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}
void RGraphicsView::focusInEvent(QFocusEvent* event)
{
    QGraphicsView::focusInEvent(event);
}
void RGraphicsView::wheelEvent(QWheelEvent* event)
{
    if(event->modifiers() & Qt::ShiftModifier)
    {
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Scale the view / do the zoom
        double scaleFactor= 1.1;

        if((event->delta() > 0) && (m_counterZoom < 20))
        {
            scale(scaleFactor, scaleFactor);
            ++m_counterZoom;
            RuleItem::setZoomLevel(scaleFactor);
            // ChildPointItem::setZoomLevel(scaleFactor);
        }
        else if(m_counterZoom > -20)
        {
            --m_counterZoom;
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
            RuleItem::setZoomLevel(1.0 / scaleFactor);
            // ChildPointItem::setZoomLevel(1.0 / scaleFactor);
        }

        setResizeAnchor(QGraphicsView::NoAnchor);
        setTransformationAnchor(QGraphicsView::NoAnchor);
    }
    else
        QGraphicsView::wheelEvent(event);
}
void RGraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
    if(nullptr == m_vmap)
        return;

    bool licenseToModify= false;

    m_menuPoint= event->pos();
    if((m_vmap->getOption(VisualItem::LocalIsGM).toBool())
        || (m_vmap->getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL))
    {
        licenseToModify= true;
    }

    if(m_vmap->isIdle())
    {
        auto extractVisualItem= [this](QList<QGraphicsItem*> itemList) -> QList<VisualItem*> {
            if(!m_vmap)
                return {};
            QList<VisualItem*> list;
            for(QGraphicsItem* item : itemList)
            {
                VisualItem* vItem= dynamic_cast<VisualItem*>(item);
                if(nullptr != vItem && !list.contains(vItem) && m_vmap->isNormalItem(vItem))
                {
                    list.append(vItem);
                }
                else
                {
                    ChildPointItem* childItem= dynamic_cast<ChildPointItem*>(item);
                    if(nullptr != childItem)
                    {
                        QGraphicsItem* item2= childItem->parentItem();
                        VisualItem* vItem= dynamic_cast<VisualItem*>(item2);
                        if(nullptr != vItem && !list.contains(vItem) && m_vmap->isNormalItem(vItem))
                        {
                            list.append(vItem);
                        }
                    }
                }
            }
            return list;
        };
        QList<QGraphicsItem*> itemList= scene()->selectedItems();
        auto itemUnderMouse= items(m_menuPoint);

        auto list= extractVisualItem(itemList);
        auto visulItemUnderMouse= extractVisualItem(itemUnderMouse);
        // remove none visual item

        std::for_each(visulItemUnderMouse.begin(), visulItemUnderMouse.end(), [&list](VisualItem* item) {
            if(!list.contains(item))
                list.append(item);
        });
        // list.append(visulItemUnderMouse);

        QMenu menu;
        auto parentWid= dynamic_cast<MediaContainer*>(parentWidget());
        // Empty list

        QAction* resetRotationAct= nullptr;
        QAction* rightRotationAct= nullptr;
        QAction* leftRotationAct= nullptr;
        QAction* angleRotationAct= nullptr;
        QAction* removeAction= nullptr;

        menu.addSection(tr("Map"));

        switch(m_vmap->getCurrentLayer())
        {
        case VisualItem::OBJECT:
            m_editObjectLayer->setChecked(true);
            break;
        case VisualItem::GROUND:
            m_editGroundLayer->setChecked(true);
            break;
        case VisualItem::CHARACTER_LAYER:
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

            QMenu* changeVibility= menu.addMenu(tr("Change Visibility"));
            changeVibility->addAction(m_hiddenVisibility);
            changeVibility->addAction(m_characterVisibility);
            changeVibility->addAction(m_allVisibility);

            QMenu* rollInit= menu.addMenu(tr("Roll Init"));
            rollInit->addAction(m_rollInitOnAllNpc);
            rollInit->addAction(m_rollInitOnAllCharacter);
            if(!list.isEmpty())
                rollInit->addAction(m_rollInitOnSelection);

            QMenu* cleanInit= menu.addMenu(tr("Clean Init"));
            cleanInit->addAction(m_cleanInitOnAllNpc);
            cleanInit->addAction(m_cleanInitOnAllCharacter);
            if(!list.isEmpty())
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

        if(licenseToModify && !list.isEmpty())
        {
            menu.addSection(tr("Item(s)"));

            auto overlapping= menu.addMenu(tr("Overlapping"));
            overlapping->addAction(m_backOrderAction);
            overlapping->addAction(m_frontOrderAction);
            overlapping->addAction(m_lowerAction);
            overlapping->addAction(m_raiseAction);

            QMenu* rotationMenu= menu.addMenu(tr("Rotate"));
            resetRotationAct= rotationMenu->addAction(tr("To 360"));
            rightRotationAct= rotationMenu->addAction(tr("Right"));
            leftRotationAct= rotationMenu->addAction(tr("Left"));
            angleRotationAct= rotationMenu->addAction(tr("Set Angle…"));

            QMenu* setLayerMenu= menu.addMenu(tr("Set Layer"));
            setLayerMenu->addAction(m_putGroundLayer);
            setLayerMenu->addAction(m_putObjectLayer);
            setLayerMenu->addAction(m_putCharacterLayer);

            QMenu* harmonizeMenu= menu.addMenu(tr("Normalize Size"));
            harmonizeMenu->addAction(m_normalizeSizeAverage);
            harmonizeMenu->addAction(m_normalizeSizeUnderMouse);
            harmonizeMenu->addAction(m_normalizeSizeBigger);
            harmonizeMenu->addAction(m_normalizeSizeSmaller);

            menu.addAction(m_lockSize);
            removeAction= menu.addAction(tr("Remove"));
        }

        QAction* selectedAction= menu.exec(event->globalPos());

        if(removeAction == selectedAction)
        {
            deleteItem(list);
        }
        else if(resetRotationAct == selectedAction)
        {
            setRotation(list, 0);
        }
        else if(selectedAction == rightRotationAct)
        {
            setRotation(list, 90);
        }
        else if(selectedAction == leftRotationAct)
        {
            setRotation(list, 270);
        }
        else if(selectedAction == angleRotationAct)
        {
            int angle= QInputDialog::getInt(
                this, tr("Rotation Value ?"), tr("Please, set the rotation angle you want [0-360]"), 0, 0, 360);
            setRotation(list, angle);
        }
        else if((m_backOrderAction == selectedAction) || (m_frontOrderAction == selectedAction)
                || (m_lowerAction == selectedAction) || (m_raiseAction == selectedAction))
        {
            changeZValue(list, static_cast<VisualItem::StackOrder>(selectedAction->data().toInt()));
        }
        else if((selectedAction == m_putCharacterLayer) || (selectedAction == m_putObjectLayer)
                || (selectedAction == m_putGroundLayer))
        {
            setItemLayer(list, static_cast<VisualItem::Layer>(selectedAction->data().toInt()));
        }
        /* }
         else // only one item
         {
             QGraphicsView::contextMenuEvent(event);
         }*/
    }
    else
    {
        QGraphicsView::contextMenuEvent(event);
    }
}
void RGraphicsView::centerOnItem()
{
    if(nullptr != m_centerOnItem)
    {
        QRectF rect= m_centerOnItem->mapToScene(m_centerOnItem->boundingRect()).boundingRect();
        QRectF rect2= mapToScene(sceneRect().toRect()).boundingRect();

        if(!rect2.contains(rect))
        {
            qreal dx= rect.center().x() - rect2.center().x();
            qreal dy= rect.center().y() - rect2.center().y();

            rect2.translate(dx, dy);
            setSceneRect(rect2);
        }
    }
}
void RGraphicsView::setRotation(QList<VisualItem*> list, int value)
{
    for(VisualItem* item : list)
    {
        item->setRotation(value);
        item->sendRotationMsg();
    }
}
void RGraphicsView::normalizeSize(Method method)
{
    if(nullptr != m_vmap)
    {
        m_vmap->addCommand(new ChangeSizeVmapItemCommand(m_vmap, method, mapToScene(m_menuPoint)));
    }
}

void RGraphicsView::setItemLayer(QList<VisualItem*> list, VisualItem::Layer layer)
{
    for(VisualItem* item : list)
    {
        if(nullptr != item)
        {
            item->setLayer(layer);
        }
    }
}
void RGraphicsView::deleteItem(QList<VisualItem*> list)
{
    if(!m_vmap)
        return;

    std::for_each(list.begin(), list.end(), [this](VisualItem* vItem) { m_vmap->removeItemFromScene(vItem->getId()); });
}
void RGraphicsView::changeZValue(QList<VisualItem*> list, VisualItem::StackOrder order)
{
    for(VisualItem* item : list)
    {
        if((nullptr != m_vmap) && (item != nullptr))
        {
            m_vmap->changeStackOrder(item, order);
        }
    }
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

    m_importImage= new QAction(tr("Import Image"), this);

    connect(m_zoomNormal, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_zoomCenterOnItem, SIGNAL(triggered(bool)), this, SLOT(centerOnItem()));
    connect(m_zoomInMax, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_zoomOutMax, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_importImage, SIGNAL(triggered()), this, SLOT(addImageToMap()));
    connect(m_zoomOut, SIGNAL(triggered()), this, SLOT(setZoomFactor()));
    connect(m_zoomIn, SIGNAL(triggered()), this, SLOT(setZoomFactor()));

    m_backOrderAction= new QAction(tr("Back"));
    m_backOrderAction->setIcon(QIcon(":/resources/icons/action-order-back.png"));
    m_backOrderAction->setData(VisualItem::BACK);

    m_frontOrderAction= new QAction(tr("Front"));
    m_frontOrderAction->setIcon(QIcon(":/resources/icons/action-order-front.png"));
    m_frontOrderAction->setData(VisualItem::FRONT);

    m_lowerAction= new QAction(tr("Lower"));
    m_lowerAction->setIcon(QIcon(":/resources/icons/action-order-lower.png"));
    m_lowerAction->setData(VisualItem::LOWER);

    m_raiseAction= new QAction(tr("Raise"));
    m_raiseAction->setIcon(QIcon(":/resources/icons/action-order-raise.png"));
    m_raiseAction->setData(VisualItem::RAISE);

    m_normalizeSizeAverage= new QAction(tr("Average"), this);
    connect(m_normalizeSizeAverage, &QAction::triggered, this, [=]() { normalizeSize(Average); });
    m_normalizeSizeUnderMouse= new QAction(tr("As undermouse item"), this);
    connect(m_normalizeSizeUnderMouse, &QAction::triggered, this, [=]() { normalizeSize(UnderMouse); });
    m_normalizeSizeBigger= new QAction(tr("As the Bigger"), this);
    connect(m_normalizeSizeBigger, &QAction::triggered, this, [=]() { normalizeSize(Bigger); });
    m_normalizeSizeSmaller= new QAction(tr("As the Smaller"), this);
    connect(m_normalizeSizeSmaller, &QAction::triggered, this, [=]() { normalizeSize(Smaller); });

    m_lockSize= new QAction(tr("Lock Item Size"), this);

    addAction(m_zoomNormal);
    addAction(m_zoomInMax);
    addAction(m_zoomOutMax);
    addAction(m_zoomOut);
    addAction(m_zoomIn);
    // addAction(m_zoomCenterOnItem);

    // PROPERTIES
    m_properties= new QAction(tr("Properties"), this);
    connect(m_properties, SIGNAL(triggered()), this, SLOT(showMapProperties()));

    // Layers
    QActionGroup* group= new QActionGroup(this);
    m_editGroundLayer= new QAction(tr("Ground"), this);
    m_editGroundLayer->setData(VisualItem::GROUND);
    m_editGroundLayer->setCheckable(true);
    m_editObjectLayer= new QAction(tr("Object"), this);
    m_editObjectLayer->setData(VisualItem::OBJECT);
    m_editObjectLayer->setCheckable(true);
    m_editCharacterLayer= new QAction(tr("Character"), this);
    m_editCharacterLayer->setData(VisualItem::CHARACTER_LAYER);
    m_editCharacterLayer->setCheckable(true);

    group->addAction(m_editGroundLayer);
    group->addAction(m_editObjectLayer);
    group->addAction(m_editCharacterLayer);

    m_putGroundLayer= new QAction(tr("Ground"), this);
    m_putGroundLayer->setData(VisualItem::GROUND);
    m_putObjectLayer= new QAction(tr("Object"), this);
    m_putObjectLayer->setData(VisualItem::OBJECT);
    m_putCharacterLayer= new QAction(tr("Character"), this);
    m_putCharacterLayer->setData(VisualItem::CHARACTER_LAYER);

    connect(m_editGroundLayer, SIGNAL(triggered()), this, SLOT(changeLayer()));
    connect(m_editObjectLayer, SIGNAL(triggered()), this, SLOT(changeLayer()));
    connect(m_editCharacterLayer, SIGNAL(triggered()), this, SLOT(changeLayer()));

    QActionGroup* group2= new QActionGroup(this);
    m_allVisibility= new QAction(tr("All"), this);
    m_allVisibility->setCheckable(true);
    m_allVisibility->setData(VMap::ALL);
    m_hiddenVisibility= new QAction(tr("Hidden"), this);
    m_hiddenVisibility->setCheckable(true);
    m_hiddenVisibility->setChecked(true);
    m_hiddenVisibility->setData(VMap::HIDDEN);
    m_characterVisibility= new QAction(tr("Fog Of War"), this);
    m_characterVisibility->setData(VMap::FOGOFWAR);
    m_characterVisibility->setCheckable(true);

    group2->addAction(m_allVisibility);
    group2->addAction(m_hiddenVisibility);
    group2->addAction(m_characterVisibility);

    connect(m_allVisibility, SIGNAL(triggered()), this, SLOT(changeVisibility()));
    connect(m_hiddenVisibility, SIGNAL(triggered()), this, SLOT(changeVisibility()));
    connect(m_characterVisibility, SIGNAL(triggered()), this, SLOT(changeVisibility()));

    m_rollInitOnAllNpc= new QAction(tr("All Npcs"), this);
    m_rollInitOnAllNpc->setToolTip(tr("Roll Initiative on All Npcs"));
    connect(m_rollInitOnAllNpc, &QAction::triggered, this, &RGraphicsView::rollInit);

    m_rollInitOnSelection= new QAction(tr("Selection"), this);
    m_rollInitOnSelection->setToolTip(tr("Roll Initiative on Selection"));
    connect(m_rollInitOnSelection, &QAction::triggered, this, &RGraphicsView::rollInit);

    m_rollInitOnAllCharacter= new QAction(tr("All Characters"), this);
    m_rollInitOnAllCharacter->setToolTip(tr("Roll Initiative on All Characters"));
    connect(m_rollInitOnAllCharacter, &QAction::triggered, this, &RGraphicsView::rollInit);

    m_cleanInitOnAllNpc= new QAction(tr("All Npcs"), this);
    m_cleanInitOnAllNpc->setToolTip(tr("Clean Initiative on All Npcs"));
    connect(m_cleanInitOnAllNpc, &QAction::triggered, this, &RGraphicsView::cleanInit);

    m_cleanInitOnSelection= new QAction(tr("Selection"), this);
    m_cleanInitOnSelection->setToolTip(tr("Clean Initiative on Selection"));
    connect(m_cleanInitOnSelection, &QAction::triggered, this, &RGraphicsView::cleanInit);

    m_cleanInitOnAllCharacter= new QAction(tr("All Characters"), this);
    m_cleanInitOnAllCharacter->setToolTip(tr("Clean Initiative on All Characters"));
    connect(m_cleanInitOnAllCharacter, &QAction::triggered, this, &RGraphicsView::cleanInit);
}
void RGraphicsView::showMapProperties()
{
    MapWizzardDialog m_propertiesDialog;

    m_propertiesDialog.updateDataFrom(m_vmap);
    if(m_propertiesDialog.exec())
    {
        m_propertiesDialog.setAllMap(m_vmap);

        sendOffMapChange();
    }
}

void RGraphicsView::rollInit()
{
    VMap::APPLY_ON_CHARACTER apply;
    if(sender() == m_rollInitOnAllNpc)
    {
        apply= VMap::AllNPC;
    }
    else if(sender() == m_rollInitOnAllCharacter)
    {
        apply= VMap::AllCharacter;
    }
    else
    {
        apply= VMap::SelectionOnly;
    }
    m_vmap->rollInit(apply);
}
void RGraphicsView::cleanInit()
{
    VMap::APPLY_ON_CHARACTER apply;
    if(sender() == m_cleanInitOnAllNpc)
    {
        apply= VMap::AllNPC;
    }
    else if(sender() == m_cleanInitOnAllCharacter)
    {
        apply= VMap::AllCharacter;
    }
    else
    {
        apply= VMap::SelectionOnly;
    }
    m_vmap->cleanUpInit(apply);
}

void RGraphicsView::sendOffMapChange()
{
    if((m_vmap->getOption(VisualItem::LocalIsGM).toBool()) || (m_vmap->getPermissionMode() == Map::PC_ALL))
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::vmapChanges);
        msg.string8(m_vmap->getId());
        m_vmap->fill(msg);
        msg.sendToServer();
    }
}

void RGraphicsView::changeLayer()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(m_vmap->editLayer(static_cast<VisualItem::Layer>(act->data().toInt())))
    {
        sendOffMapChange();
    }
}
void RGraphicsView::changeVisibility()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(m_vmap->setVisibilityMode(static_cast<VMap::VisibilityMode>(act->data().toInt())))
    {
        sendOffMapChange();
    }
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
}
void RGraphicsView::currentToolChanged(VToolsBar::SelectableTool selectedtool)
{
    m_currentTool= selectedtool;
}
void RGraphicsView::resizeEvent(QResizeEvent* event)
{
    // GM is the references

    if((nullptr != scene()) && (m_vmap->getOption(VisualItem::LocalIsGM).toBool()))
    {
        if((geometry().width() > scene()->sceneRect().width())
            || ((geometry().height() > scene()->sceneRect().height())))
        {
            scene()->setSceneRect(geometry());
            m_vmap->setWidth(geometry().width());
            m_vmap->setHeight(geometry().height());
            ensureVisible(geometry(), 0, 0);
        }

        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::GeometryViewChanged);
        msg.string8(m_vmap->getId());
        QRectF r= sceneRect();

        msg.real(r.x());
        msg.real(r.y());
        msg.real(r.width());
        msg.real(r.height());

        msg.sendToServer();
    }

    setResizeAnchor(QGraphicsView::NoAnchor);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    QGraphicsView::resizeEvent(event);
}
void RGraphicsView::readMessage(NetworkMessageReader* msg)
{
    /// @warning unread message
    qreal x= msg->real();
    qreal y= msg->real();
    qreal width= msg->real();
    qreal height= msg->real();
    Q_UNUSED(x)
    Q_UNUSED(y)
    Q_UNUSED(width)
    Q_UNUSED(height)

    // if(nullptr!=scene())
    //{
    // setSceneRect(x,y,width,height);
    //}
}
void RGraphicsView::addImageToMap()
{
    QString imageToLoad= QFileDialog::getOpenFileName(this, tr("Open image file"),
        m_preferences->value("ImageDirectory", QDir::homePath()).toString(),
        m_preferences->value("ImageFileFilter", "*.jpg *.jpeg *.png *.bmp *.svg").toString());

    if(nullptr != m_vmap)
    {
        m_vmap->addImageItem(QImage(imageToLoad));
    }
}
