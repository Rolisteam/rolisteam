#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>

#include "data/character.h"
#include "graphicsItems/anchoritem.h"
#include "graphicsItems/characteritem.h"
#include "graphicsItems/ellipsitem.h"
#include "graphicsItems/griditem.h"
#include "graphicsItems/highlighteritem.h"
#include "graphicsItems/imageitem.h"
#include "graphicsItems/lineitem.h"
#include "graphicsItems/pathitem.h"
#include "graphicsItems/rectitem.h"
#include "graphicsItems/ruleitem.h"
#include "graphicsItems/sightitem.h"
#include "graphicsItems/textitem.h"

#include "vmap.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/cleveruri.h"
#include "data/cleverurimimedata.h"
#include "data/rolisteammimedata.h"
#include "media/mediatype.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/ellipsecontroller.h"
#include "controller/item_controllers/imageitemcontroller.h"
#include "controller/item_controllers/linecontroller.h"
#include "controller/item_controllers/rectcontroller.h"
#include "controller/item_controllers/sightcontroller.h"
#include "controller/item_controllers/textcontroller.h"

#include "worker/iohelper.h"

// Undo management
#include "commands/movevmapitem.h"
#include "undoCmd/addvmapitem.h"
#include "undoCmd/deletevmapitem.h"

void addCharacterItem(VectorialMapController* ctrl, const QPointF& pos, Character* character)
{
    if(!character)
        return;

    std::map<QString, QVariant> params;
    params.insert({Core::vmapkeys::KEY_CHARACTER, QVariant::fromValue(character)});
    params.insert({Core::vmapkeys::KEY_POS, pos});
    params.insert({Core::vmapkeys::KEY_TOOL, character->isNpc() ? Core::SelectableTool::NonPlayableCharacter :
                                                                  Core::SelectableTool::PlayableCharacter});

    ctrl->insertItemAt(params);
}

void addImageIntoController(VectorialMapController* ctrl, const QPointF& pos, CleverURI* uri)
{
    std::map<QString, QVariant> params;
    params.insert({Core::vmapkeys::KEY_POS, pos});
    params.insert({Core::vmapkeys::KEY_TOOL, Core::SelectableTool::IMAGE});
    if(uri->exists())
        params.insert({Core::vmapkeys::KEY_PATH, uri->path()});

    if(uri->hasData())
        params.insert({Core::vmapkeys::KEY_DATA, uri->data()});
    ctrl->insertItemAt(params);
}

VMap::VMap(VectorialMapController* ctrl, QObject* parent) : QGraphicsScene(parent), m_ctrl(ctrl)
{
    if(!m_ctrl)
        return;

    // connection to signal
    connect(m_ctrl, &VectorialMapController::backgroundColorChanged, this,
            [this]() { setBackgroundBrush(m_ctrl->backgroundColor()); });
    connect(m_ctrl, &VectorialMapController::gridSizeChanged, this, &VMap::computePattern);
    connect(m_ctrl, &VectorialMapController::gridPatternChanged, this, &VMap::computePattern);
    connect(m_ctrl, &VectorialMapController::gridScaleChanged, this, &VMap::computePattern);
    connect(m_ctrl, &VectorialMapController::gridAboveChanged, this, &VMap::computePattern);
    connect(m_ctrl, &VectorialMapController::gridVisibilityChanged, this, &VMap::computePattern);
    connect(m_ctrl, &VectorialMapController::gridColorChanged, this, &VMap::computePattern);
    connect(m_ctrl, &VectorialMapController::backgroundColorChanged, this, &VMap::computePattern);
    connect(m_ctrl, &VectorialMapController::toolChanged, this, [this]() {
        m_currentPath= nullptr;
        m_currentItem= nullptr;
    });
    connect(m_ctrl, &VectorialMapController::highLightAt, this,
            [this](const QPointF& p, const qreal& penSize, const QColor& color) {
                auto hitem= new HighlighterItem(p, penSize, color);
                addAndInit(hitem);
                hitem->setPos(p);
            });

    // item Managers

    // initialization
    connect(m_ctrl, &VectorialMapController::visualItemControllerCreated, this, &VMap::addVisualItem);

    setBackgroundBrush(m_ctrl->backgroundColor());

    auto grid= m_ctrl->gridController();

    m_gridItem= new GridItem(grid);
    addItem(m_gridItem);

    m_gridItem->setPos(grid->pos());
    m_gridItem->setZValue(2);

    auto sight= m_ctrl->sightController();
    m_sightItem= new SightItem(sight);
    addItem(m_sightItem);

    m_sightItem->setPos(0, 0);
    m_sightItem->setZValue(1);

    // m_ctrl->loadItems();
    m_gridItem->setVisible(grid->visible());
    m_sightItem->setVisible(sight->visible());
    computePattern();

    addExistingItems();
}

void VMap::addExistingItems()
{
    auto model= m_ctrl->model();
    auto const& items= model->items();
    std::for_each(std::begin(items), std::end(items),
                  [this](vmap::VisualItemController* item) { addVisualItem(item); });
}

void VMap::updateLayer()
{
    auto const& values= m_itemMap->values();
    for(auto& item : values)
    {
        item->updateItemFlags();
    }
}
void VMap::addAndInit(QGraphicsItem* item)
{
    addItem(item);
}
void VMap::addVisualItem(vmap::VisualItemController* ctrl)
{
    switch(ctrl->itemType())
    {
    case vmap::VisualItemController::RECT:
        addRectItem(dynamic_cast<vmap::RectController*>(ctrl), !ctrl->initialized());
        break;
    case vmap::VisualItemController::LINE:
        addLineItem(dynamic_cast<vmap::LineController*>(ctrl), !ctrl->initialized());
        break;
    case vmap::VisualItemController::IMAGE:
        addImageItem(dynamic_cast<vmap::ImageItemController*>(ctrl));
        break;
    case vmap::VisualItemController::PATH:
        addPathItem(dynamic_cast<vmap::PathController*>(ctrl), !ctrl->initialized());
        break;
    case vmap::VisualItemController::TEXT:
        addTextItem(dynamic_cast<vmap::TextController*>(ctrl));
        break;
    case vmap::VisualItemController::ELLIPSE:
        addEllipseItem(dynamic_cast<vmap::EllipseController*>(ctrl), !ctrl->initialized());
        break;
    case vmap::VisualItemController::CHARACTER:
        addCharaterItem(dynamic_cast<vmap::CharacterItemController*>(ctrl));
        break;
    default:
        break;
    }
    update();
}
void VMap::addLineItem(vmap::LineController* lineCtrl, bool editing)
{
    if(!lineCtrl)
        return;
    auto item= new LineItem(lineCtrl);
    addAndInit(item);
    item->setPos(lineCtrl->pos());
    if(editing)
        m_currentItem= item;
}

void VMap::addImageItem(vmap::ImageItemController* imgCtrl)
{
    if(!imgCtrl)
        return;
    auto img= new ImageItem(imgCtrl);
    addAndInit(img);
    img->setPos(imgCtrl->pos());
}

void VMap::addRectItem(vmap::RectController* rectCtrl, bool editing)
{
    if(!rectCtrl)
        return;
    qDebug() << "add rect Item editing:" << editing;
    auto item= new RectItem(rectCtrl);
    addAndInit(item);
    item->setPos(rectCtrl->pos());
    if(editing)
        m_currentItem= item;
}

void VMap::addEllipseItem(vmap::EllipseController* ellisCtrl, bool editing)
{
    if(!ellisCtrl)
        return;
    auto item= new EllipsItem(ellisCtrl);
    addAndInit(item);
    item->setPos(ellisCtrl->pos());
    if(editing)
        m_currentItem= item;
}

void VMap::addTextItem(vmap::TextController* textCtrl)
{
    if(!textCtrl)
        return;
    auto tmp= new TextItem(textCtrl);
    addAndInit(tmp);
    tmp->setPos(textCtrl->pos());
}

void VMap::addCharaterItem(vmap::CharacterItemController* itemCtrl)
{
    if(!itemCtrl)
        return;
    auto tmp= new CharacterItem(itemCtrl);
    addAndInit(tmp);
    tmp->setPos(itemCtrl->pos());
}

void VMap::addPathItem(vmap::PathController* pathCtrl, bool editing)
{
    if(!pathCtrl)
        return;
    auto path= new PathItem(pathCtrl);

    if(editing)
    {
        m_currentItem= path;
        if(!pathCtrl->penLine())
        {
            m_currentPath= path;
        }
    }
    addAndInit(path);
    path->setPos(pathCtrl->pos());
}

void VMap::updateItem(const QPointF& end)
{
    switch(m_ctrl->tool())
    {
    case Core::PATH:
    {
        m_currentPath->addPoint(m_currentPath->mapFromScene(end));
        update();
    }
    break;
    default:
        break;
    }
}

void VMap::setCurrentItemOpacity(qreal a)
{
    QList<QGraphicsItem*> selection= selectedItems();
    for(auto item : selection)
    {
        VisualItem* vItem= dynamic_cast<VisualItem*>(item);
        if(nullptr != vItem)
        {
            vItem->setOpacity(a);
        }
    }
}

void VMap::selectionHasChanged()
{
    QList<QGraphicsItem*> items= selectedItems();
    for(auto item : items)
    {
        VisualItem* vItem= dynamic_cast<VisualItem*>(item);
        if(nullptr != vItem)
        {
            emit currentItemOpacity(vItem->opacity());
            break;
        }
    }
}

void VMap::insertItem(const QPointF& pos)
{
    std::map<QString, QVariant> params;
    params.insert({Core::vmapkeys::KEY_POS, pos});
    params.insert({Core::vmapkeys::KEY_COLOR, m_ctrl->toolColor()});
    params.insert({Core::vmapkeys::KEY_PENWIDTH, m_ctrl->penSize()});
    params.insert({Core::vmapkeys::KEY_TOOL, m_ctrl->tool()});
    m_ctrl->insertItemAt(params);
}

void VMap::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    auto leftButton= (mouseEvent->button() == Qt::LeftButton);
    if(m_ctrl->tool() == Core::HANDLER && leftButton)
    {
        const QList<QGraphicsItem*> itemList= selectedItems();
        for(auto item : itemList)
        {
            VisualItem* vitem= dynamic_cast<VisualItem*>(item);
            if(nullptr != vitem)
            {
                if(isNormalItem(vitem))
                {
                    m_movingItems.append(vitem);
                    m_oldPos.append(vitem->pos());
                }
            }
        }
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
    else if(Core::PIPETTE == m_ctrl->tool() && leftButton)
    {
        QList<QGraphicsItem*> itemList= items(mouseEvent->scenePos());
        itemList.removeAll(m_gridItem);
        itemList.removeAll(m_sightItem);
        if(!itemList.isEmpty())
        {
            VisualItem* item= dynamic_cast<VisualItem*>(itemList.at(0));
            if(nullptr != item)
            {
                if(item->getType() != vmap::VisualItemController::ItemType::IMAGE)
                {
                    QColor color= item->color();
                    if(color.isValid())
                    {
                        m_ctrl->setToolColor(color);
                    }
                }
            }
        }
    }
    else if(Core::HIGHLIGHTER == m_ctrl->tool() && leftButton)
    {
        m_ctrl->addHighLighter(mouseEvent->scenePos());
    }
    else if(Core::BUCKET == m_ctrl->tool() && leftButton)
    {
        QList<QGraphicsItem*> itemList= items(mouseEvent->scenePos());
        itemList.removeAll(m_gridItem);
        itemList.removeAll(m_sightItem);
        if(!itemList.isEmpty())
        {
            VisualItem* item= dynamic_cast<VisualItem*>(itemList.at(0));
            if(nullptr != item)
            {
                if(item->getType() != vmap::VisualItemController::ItemType::IMAGE)
                {
                    // auto cmd= new ChangeColorItemCmd(item->controller(), m_ctrl->toolColor());
                    m_ctrl->askForColorChange(item->controller());
                }
            }
        }
    }
    else if(Core::ANCHOR == m_ctrl->tool() && leftButton)
    {
        m_parentItemAnchor= new AnchorItem();
        addAndInit(m_parentItemAnchor);
        m_parentItemAnchor->setPos(mouseEvent->scenePos());
    }
    else if(Core::RULE == m_ctrl->tool() && leftButton)
    {
        m_ruleItem= new RuleItem(m_ctrl);
        addAndInit(m_ruleItem);
        m_ruleItem->setPos(mouseEvent->scenePos());
    }
    else if(leftButton)
    {
        if(m_currentPath == nullptr)
        {
            insertItem(mouseEvent->scenePos());
        }
        else
        {
            updateItem(mouseEvent->scenePos());
        }
    }
    else if(mouseEvent->button() == Qt::RightButton)
    {
        m_currentPath= nullptr;
    }
}
void VMap::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if(!m_currentItem.isNull())
    {
        m_currentItem->setModifiers(mouseEvent->modifiers());
        mouseEvent->accept();
        m_currentItem->setNewEnd(mouseEvent->scenePos() - mouseEvent->lastScenePos()); // mouseEvent->scenePos()
        update();
    }
    else if(!m_ruleItem.isNull())
    {
        mouseEvent->accept();
        m_ruleItem->setNewEnd(mouseEvent->scenePos() - mouseEvent->lastScenePos(),
                              mouseEvent->modifiers() & Qt::ControlModifier);
        update();
    }
    else if(!m_parentItemAnchor.isNull())
    {
        mouseEvent->accept();
        m_parentItemAnchor->setNewEnd(mouseEvent->scenePos() - mouseEvent->lastScenePos());
        update();
    }
    if((m_ctrl->tool() == Core::HANDLER) || (m_ctrl->tool() == Core::TEXT) || (m_ctrl->tool() == Core::TEXTBORDER))
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
void VMap::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if(m_parentItemAnchor)
    {
        manageAnchor();
        removeItem(m_parentItemAnchor);
        delete m_parentItemAnchor;
        m_parentItemAnchor.clear();
    }
    if(m_ruleItem)
    {
        removeItem(m_ruleItem);
        delete m_ruleItem;
        m_ruleItem.clear();
    }
    if(!m_currentItem.isNull())
    {
        if(Core::EditionMode::Painting == m_ctrl->editionMode())
        {
            m_currentItem->endOfGeometryChange(ChildPointItem::Resizing);
        }
        else
        {
            auto poly= m_currentItem->shape().toFillPolygon();
            poly= poly.translated(m_currentItem->pos());
            m_ctrl->changeFogOfWar(poly, (Core::EditionMode::Mask == m_ctrl->editionMode()));
            if(nullptr == m_currentPath)
            {
                removeItem(m_currentItem);
            }
        }
    }
    else if((nullptr != m_currentPath) && (Core::EditionMode::Painting != m_ctrl->editionMode()))
    {
        auto poly= m_currentPath->shape().toFillPolygon();
        m_ctrl->changeFogOfWar(poly, (Core::EditionMode::Mask == m_ctrl->editionMode()));
        update();
    }
    m_ctrl->setIdle(true);
    m_currentItem= nullptr;
    if(m_ctrl->tool() == Core::HANDLER)
    {
        if(!m_movingItems.isEmpty())
        {
            if(m_movingItems.first() != nullptr && mouseEvent->button() == Qt::LeftButton)
            {
                if(m_oldPos.first() != m_movingItems.first()->pos())
                {
                    if(!m_undoStack.isNull())
                        m_undoStack->push(new MoveItemCommand(m_movingItems, m_oldPos));
                }
                m_movingItems.clear();
                m_oldPos.clear();
            }
        }
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}

void VMap::setAnchor(QGraphicsItem* child, QGraphicsItem* parent)
{
    if(nullptr == child)
        return;

    if(!parent)
    {
        // TODO: make command
        QPointF pos= parent->mapFromScene(child->pos());
        child->setParentItem(nullptr);
        child->setPos(pos);
    }

    /*VisualItem* childItem= dynamic_cast<VisualItem*>(child);
    VisualItem* paItem= dynamic_cast<VisualItem*>(parent);

    if(!childItem)
        return;

    QPointF pos= child->pos();
    QPointF pos2;
    if(nullptr != parent)
    {
        pos2= parent->mapFromScene(pos);
    }
    else
    {
        if(nullptr != child->parentItem())
        {
            pos2= child->parentItem()->mapToScene(pos);
        }
    }

    if(nullptr != paItem)
    {
        // msg.string8(paItem->getId());
    }
    else
    {
        //    msg.string8("nullptr");
    }
    bool hasMoved= false;
    if(nullptr == parent)
    {
        child->setParentItem(parent);
        hasMoved= true;
    }
    else if(parent->parentItem() != child)
    {
        child->setParentItem(parent);
        hasMoved= true;
    }
    if(!(pos2.isNull() && parent == nullptr) && hasMoved)
    {
        child->setPos(pos2);
    }*/
}

bool VMap::isNormalItem(const QGraphicsItem* item)
{
    if(!item)
        return false;

    if((item == m_gridItem) || (item == m_sightItem))
    {
        return false;
    }
    auto vItem= dynamic_cast<const VisualItem*>(item);
    if(!vItem)
        return false;
    return true;
}

void VMap::manageAnchor()
{
    if(m_parentItemAnchor.isNull())
        return;

    QGraphicsItem* child= nullptr;
    QGraphicsItem* parent= nullptr;
    QList<QGraphicsItem*> item1= items(m_parentItemAnchor->getStart());
    for(QGraphicsItem* item : item1)
    {
        if((nullptr == child) && (isNormalItem(item)))
        {
            child= item;
        }
    }
    QList<QGraphicsItem*> item2= items(m_parentItemAnchor->getEnd());
    for(QGraphicsItem* item : item2)
    {
        if((nullptr == parent) && (isNormalItem(item)))
        {
            parent= item;
        }
    }
    setAnchor(child, parent);
}

void VMap::checkItemLayer(VisualItem* item)
{
    if(nullptr == item)
        return;

    item->updateItemFlags();
}

void VMap::computePattern()
{
    setBackgroundBrush(m_ctrl->backgroundColor());

    if(!m_ctrl->gridAbove() && m_ctrl->gridVisibility() && (m_ctrl->gridPattern() != Core::GridPattern::NONE))
    {
        auto ctrl= m_ctrl->gridController();
        setBackgroundBrush(QPixmap::fromImage(ctrl->gridPattern()));
    }
}

void VMap::ensureFogAboveAll()
{
    QList<VisualItem*> list= m_itemMap->values();
    std::sort(list.begin(), list.end(),
              [](const VisualItem* item, const VisualItem* meti) { return meti->zValue() > item->zValue(); });
    m_orderedItemList= list;
    VisualItem* highest= nullptr;
    for(auto& item : m_orderedItemList)
    {
        if(item != m_sightItem)
        {
            highest= item;
        }
    }
    if(nullptr != highest)
    {
        auto z= highest->zValue();
        m_sightItem->setZValue(z + 1);
        m_gridItem->setZValue(z + 2);
    }
}

void VMap::setUndoStack(QUndoStack* undoStack)
{
    m_undoStack= undoStack;
}

void VMap::removeItemFromData(VisualItem* item)
{
    if(nullptr == item)
        return;

    // m_sortedItemList.removeAll(item->getId());
    // m_itemMap->remove(item->getId());
    auto character= dynamic_cast<CharacterItem*>(item);
    if(nullptr != character)
    {
        m_characterItemMap->remove(character->getCharacterId());
    }
}

bool VMap::isItemStorable(VisualItem* item)
{
    if(!item)
        return false;

    QSet<vmap::VisualItemController::ItemType> allowed{
        vmap::VisualItemController::ItemType::ANCHOR, vmap::VisualItemController::ItemType::GRID,
        vmap::VisualItemController::ItemType::SIGHT, vmap::VisualItemController::ItemType::RULE,
        vmap::VisualItemController::ItemType::HIGHLIGHTER};
    return !allowed.contains(item->getType());
}

QList<CharacterItem*> VMap::getCharacterOnMap(QString id)
{
    QList<CharacterItem*> result;
    auto const& values= m_characterItemMap->values();
    for(auto& item : values)
    {
        if(nullptr == item)
            continue;

        if(item->getCharacterId() == id)
        {
            result.append(item);
        }
    }
    return result;
}

void VMap::promoteItemInType(VisualItem* item, vmap::VisualItemController::ItemType type)
{
    if(nullptr != item)
    {
        VisualItem* bis= item->promoteTo(type);
        if(bis == nullptr)
            return;
        // bis->setLayer(item->getLayer());
        // removeItemFromScene(item->getId());
        // addNewItem(new AddVmapItemCommand(bis, false, this), true);
        // bis->initChildPointItem();
    }
}

void VMap::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete)
    {
        QStringList idListToRemove;
        for(auto& item : selectedItems())
        {
            VisualItem* itemV= dynamic_cast<VisualItem*>(item);
            if(nullptr != itemV)
            {
                if(itemV->canBeMoved())
                {
                    // idListToRemove << itemV->getId();
                }
            }
        }
        /*for(auto& id : idListToRemove)
        {
            removeItemFromScene(id);
        }*/
        setFocusItem(nullptr);
        event->accept();
        //  return;
    }
    QGraphicsScene::keyPressEvent(event);
}

void VMap::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    event->acceptProposedAction();
}

void VMap::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    auto data= event->mimeData();
    if(data->hasFormat("rolisteam/userlist-item"))
    {
        qInfo() << "VMAP dropEvent character";
        const RolisteamMimeData* rolisteamData= qobject_cast<const RolisteamMimeData*>(data);
        Person* item= rolisteamData->person();
        addCharacterItem(m_ctrl, event->scenePos(), dynamic_cast<Character*>(item));
    }
    else if(data->hasFormat("rolisteam/cleverurilist"))
    {
        const CleverUriMimeData* resourcesData= qobject_cast<const CleverUriMimeData*>(data);
        if(!resourcesData)
            return;

        QList<ResourcesNode*> resourcesList= resourcesData->getList().values();
        for(ResourcesNode* resource : resourcesList)
        {
            if(resource->type() == ResourcesNode::Cleveruri)
            {
                qInfo() << "VMAP dropEvent: image from resources list";
                auto media= dynamic_cast<CleverURI*>(resource);
                if(nullptr == media)
                    continue;

                if(media->contentType() == Core::ContentType::PICTURE)
                {
                    addImageIntoController(m_ctrl, event->scenePos(), media);
                }
            }
            else if(resource->type() == ResourcesNode::Person)
            {
                qInfo() << "VMAP dropEvent: Character from resources list";
                addCharacterItem(m_ctrl, event->scenePos(), dynamic_cast<Character*>(resource));
            }
        }
    }
    else
    {
        if(!data->hasUrls())
            return;

        for(QUrl& url : data->urls())
        {
            if(url.isLocalFile() && url.fileName().endsWith("rtok"))
            {
                qInfo() << "VMAP dropEvent: rtok from file";
                std::map<QString, QVariant> params;
                /*if(IOHelper::loadToken(url.toLocalFile(), params))
                {
                    params.insert({QStringLiteral("position"), event->scenePos()});
                    params.insert({QStringLiteral("color"), m_ctrl->toolColor()});
                    params.insert({QStringLiteral("penWidth"), m_ctrl->penSize()});
                    params.insert({QStringLiteral("tool"), Core::SelectableTool::NonPlayableCharacter});
                    m_ctrl->insertItemAt(params);
                }*/
            }
            else if(url.isLocalFile())
            {
                qInfo() << "VMAP dropEvent: Image from file";
                std::map<QString, QVariant> params;
                params.insert({Core::vmapkeys::KEY_POS, event->scenePos()});
                params.insert({Core::vmapkeys::KEY_TOOL, Core::SelectableTool::IMAGE});
                params.insert({Core::vmapkeys::KEY_PATH, url.toLocalFile()});
                m_ctrl->insertItemAt(params);
            }
            // if(nullptr != item)
            {
                // addNewItem(new AddVmapItemCommand(item, true, this), true);
                // item->setPos(event->scenePos());
                // sendOffItem(item);
            }
        }
    }
}
void VMap::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(nullptr != data)
    {
        if(data->hasFormat("rolisteam/userlist-item"))
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        const QMimeData* mimeData= event->mimeData();
        if(mimeData->hasUrls())
        {
            event->acceptProposedAction();
        }
    }
}
void VMap::duplicateItem(VisualItem* item)
{
    VisualItem* copy= item->getItemCopy();
    if(nullptr != copy)
    {
        // copy->initChildPointItem();
        // copy->setLayer(item->getLayer());
        //  addNewItem(new AddVmapItemCommand(copy, false, this), true);
        copy->setPos(item->pos());
        clearSelection();
        item->clearFocus();
        copy->clearFocus();
        clearFocus();
        setFocusItem(copy);
        update();
        // sendOffItem(copy, false);
    }
}
bool VMap::isIdle() const
{
    return m_currentItem.isNull();
}
void VMap::ownerHasChangedForCharacterItem(Character* person, CharacterItem* cItem)
{
    if(nullptr == person)
        return;
    // TODO make it with controller
    /*
        // Remove them from the map
        m_characterItemMap->remove(item->uuid());
        // remove the changed characterItem
        list.removeOne(cItem);
        // add the others
        for(CharacterItem* itemC : list)
        {
            m_characterItemMap->insertMulti(itemC->getCharacterId(), itemC);
        }
        // add item with its new key
        m_characterItemMap->insertMulti(cItem->getCharacterId(), cItem);
        if((cItem->isPlayableCharacter()) && (!m_ctrl->localGM()) && (cItem->isLocal()))
        {
            // changeStackOrder(cItem, VectorialMapController::FRONT);
        }


        // add item with its new key
        m_characterItemMap->insert(person->uuid(), cItem);=*/
}

void VMap::insertCharacterInMap(CharacterItem* item)
{
    if((nullptr == m_characterItemMap) && (nullptr == item))
        return;

    /*m_characterItemMap->insert(item->getCharacterId(), item);
    connect(item, &CharacterItem::ownerChanged, this, &VMap::ownerHasChangedForCharacterItem);
    connect(item, &CharacterItem::runDiceCommand, this, &VMap::runDiceCommandForCharacter);
    if(!item->isNpc())
    {
        // m_sightItem->insertVision(item);
    }
    else
    {
        // auto list= PlayerModel::instance();
        // list->addNpc(item->getCharacter());
        auto search= item->getName();
        auto items= m_characterItemMap->values();
        items.removeAll(item);
        QList<CharacterItem*> sameNameItems;
        std::copy_if(items.begin(), items.end(), std::back_inserter(sameNameItems),
                     [search](CharacterItem* item) { return item->getName() == search; });
        auto it= std::max_element(
            sameNameItems.begin(), sameNameItems.end(),
            [](const CharacterItem* a, const CharacterItem* b) { return a->getNumber() < b->getNumber(); });
        if(it != sameNameItems.end())
        {
            int max= (*it)->getNumber();
            item->setNumber(++max);
        }
    }*/

    /*     m_characterItemMap->insert(itemC->getCharacterId(), itemC);
  //    }
      // add item with its new key
      m_characterItemMap->insert(cItem->getCharacterId(), cItem);
      if((cItem->isPlayableCharacter()) && (!m_ctrl->localGM()) && (cItem->isLocal()))
      {
          // changeStackOrder(cItem, VectorialMapController::FRONT);
      }*/
}

/*void VMap::changeStackOrder(VisualItem* item, VisualItem::StackOrder op)
{
    if(nullptr == item || m_sortedItemList.size() < 2)
        return;
    int index= m_sortedItemList.indexOf(item->getId());
    if(index < 0)
        return;

    if(VisualItem::FRONT == op)
    {
        m_sortedItemList.append(m_sortedItemList.takeAt(index));
    }
    else if(VisualItem::BACK == op)
    {
        m_sortedItemList.prepend(m_sortedItemList.takeAt(index));
    }
    else
    {
        // find out insertion indexes over the stacked items
        QList<QGraphicsItem*> stackedItems= items(item->sceneBoundingRect(), Qt::IntersectsItemShape);

        int maxIndex= 0;
        int minIndex= m_sortedItemList.size() - 1;
        for(QGraphicsItem* qitem : stackedItems)
        {
            // operate only on different Content and not on sightItem.
            VisualItem* vItem= dynamic_cast<VisualItem*>(qitem);
            if(!vItem || vItem == item || vItem == m_sightItem || vItem == m_gridItem)
                continue;

            auto id= vItem->getId();
            auto tmpIndex= m_sortedItemList.indexOf(id);
            maxIndex= std::max(maxIndex, tmpIndex);
            minIndex= std::min(minIndex, tmpIndex);
        }

        if(VisualItem::RAISE == op)
        {
            m_sortedItemList.insert(maxIndex, m_sortedItemList.takeAt(index));
            // element at  index must be after all element in list
        }
        else if(VisualItem::LOWER == op)
        {
            m_sortedItemList.insert(minIndex, m_sortedItemList.takeAt(index));
            // element at  index must be before all element in list
        }
    }

    quint64 z= 0;
    for(const QString& key : m_sortedItemList)
    {
        VisualItem* item= m_itemMap->value(key);
        if(nullptr != item)
        {
            item->setZValue(++z);
        }
    }

    // add grid and sight item
    m_zIndex= z;
    m_sightItem->setZValue(++z);
    m_gridItem->setZValue(++z);
}*/

void VMap::showTransparentItems()
{
    auto const& values= m_itemMap->values();
    for(auto& item : values)
    {
        if(qFuzzyCompare(item->opacity(), 0))
        {
            item->setOpacity(1);
        }
    }
}

QRectF VMap::itemsBoundingRectWithoutSight()
{
    QRectF result;
    auto const& values= m_itemMap->values();
    for(auto item : values)
    {
        if(item != m_sightItem && item != m_gridItem)
        {
            if(result.isNull())
            {
                result= item->boundingRect();
            }
            result= result.united(item->boundingRect());
        }
    }
    return result;
}

/*void VMap::rollInit(Core::CharacterScope zone)
{
    QList<CharacterItem*> list;
    if(zone == Core::AllCharacter)
    {
        list= m_characterItemMap->values();
    }
    else if(zone == Core::AllNPC)
    {
        auto const& all= m_characterItemMap->values();
        for(auto& i : all)
        {
            if(i->isNpc())
            {
                list.append(i);
            }
        }
    }
    else
    {
        auto selection= selectedItems();
        for(auto& sel : selection)
        {
            auto item= dynamic_cast<CharacterItem*>(sel);
            if(nullptr != item)
            {
                list.append(item);
            }
        }
    }

    for(auto& charac : list)
    {
        charac->runInit();
    }
}*/

/*void VMap::cleanUpInit(Core::CharacterScope zone)
{
    QList<CharacterItem*> list;
    if(zone == Core::AllCharacter)
    {
        list= m_characterItemMap->values();
    }
    else if(zone == Core::AllNPC)
    {
        auto all= m_characterItemMap->values();
        for(auto& i : all)
        {
            if(i->isNpc())
            {
                list.append(i);
            }
        }
    }
    else
    {
        auto selection= selectedItems();
        for(auto& sel : selection)
        {
            auto item= dynamic_cast<CharacterItem*>(sel);
            if(nullptr != item)
            {
                list.append(item);
            }
        }
    }

    for(auto& charac : list)
    {
        charac->cleanInit();
    }
}*/
void VMap::addCommand(QUndoCommand* cmd)
{
    if(m_undoStack.isNull() || nullptr == cmd)
        return;
    m_undoStack->push(cmd);
}
