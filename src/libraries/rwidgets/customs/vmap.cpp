#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>

#include "commands/movevmapitem.h"
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
#include "worker/mediahelper.h"

#include "utils/logcategories.h"

void setNewParent(VisualItem* child, VisualItem* parent)
{
    auto pos= child->scenePos();
    child->setParentItem(parent);
    auto ctrl= child->controller();
    if(parent)
        ctrl->setPos(parent->mapFromScene(pos));
    else
        ctrl->setPos(pos);
}

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
    connect(m_ctrl, &VectorialMapController::toolChanged, this,
            [this]()
            {
                m_currentPath= nullptr;
                m_currentItem= nullptr;
            });
    connect(m_ctrl, &VectorialMapController::highLightAt, this,
            [this](const QPointF& p, const qreal& penSize, const QColor& color)
            {
                auto hitem= new HighlighterItem(m_ctrl->preferences(), p, penSize, color);
                addAndInit(hitem);
                hitem->setPos(p);
            });

    // item Managers

    // initialization
    connect(m_ctrl, &VectorialMapController::visualItemControllerCreated, this, &VMap::addVisualItem);

    setBackgroundBrush(m_ctrl->backgroundColor());

    auto grid= m_ctrl->gridController();

    m_gridItem.reset(new GridItem(grid));
    addItem(m_gridItem.get());

    m_gridItem->setPos(grid->pos());

    auto sight= m_ctrl->sightController();
    m_sightItem= new SightItem(sight);
    addItem(m_sightItem);

    m_sightItem->setPos(0, 0);

    // m_ctrl->loadItems();
    m_gridItem->setVisible(grid->visible());
    m_sightItem->setVisible(sight->visible());
    computePattern();

    addExistingItems();
}

void VMap::addExistingItems()
{
    auto model= m_ctrl->model();
    auto const& modelItems= model->items();
    std::for_each(std::begin(modelItems), std::end(modelItems),
                  [this](vmap::VisualItemController* item) { addVisualItem(item); });

    // management of parenting
    QHash<QString, VisualItem*> hash;
    QHash<QString, VisualItem*> needParent;
    for(auto& item : items())
    {
        auto vItem= getNormalItem(item);
        if(!vItem)
            continue;

        hash.insert(vItem->uuid(), vItem);

        if(!vItem->controller()->parentUuid().isEmpty())
            needParent.insert(needParent);
    }

    for(auto const& item : std::as_const(needParent))
    {
        auto parentId= item->controller()->parentUuid();
        if(hash.contains(parentId))
        {
            item->setParentItem(hash.value(parentId));
        }
    }
}

void VMap::addAndInit(QGraphicsItem* item)
{
    addItem(item);
}
void VMap::addVisualItem(vmap::VisualItemController* ctrl)
{
    if(!ctrl)
        return;

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
    connect(ctrl, &vmap::CharacterItemController::parentUuidChanged, ctrl,
            [ctrl, this]()
            {
                auto graphicItems= items();
                auto parentId= ctrl->parentUuid();

                QPointer<VisualItem> child;
                QPointer<VisualItem> parent;
                for(auto& item : graphicItems)
                {
                    auto vItem= getNormalItem(item);
                    if(!vItem)
                        continue;

                    if(vItem->controller() == ctrl)
                        child= vItem;

                    if(vItem->uuid() == parentId)
                        parent= vItem;

                    if((child && parent) || (child && (parentId.isEmpty())))
                        break;
                }

                if(!child)
                    return;

                setNewParent(child, parent.data());
            });
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
    if(!m_currentPath)
        return;

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

void VMap::insertItem(const QPointF& pos)
{
    std::map<QString, QVariant> params;
    params.insert({Core::vmapkeys::KEY_POS, pos});
    params.insert({Core::vmapkeys::KEY_COLOR, m_ctrl->toolColor()});
    params.insert({Core::vmapkeys::KEY_PENWIDTH, m_ctrl->penSize()});
    params.insert({Core::vmapkeys::KEY_TOOL, m_ctrl->tool()});
    params.insert({Core::vmapkeys::KEY_NUMBER, m_ctrl->npcNumber()});
    params.insert({Core::vmapkeys::KEY_CHARAC_NAME, m_ctrl->npcName()});
    m_ctrl->insertItemAt(params);
}

VisualItem* VMap::visualItemUnder(const QPointF& pos)
{
    QList<QGraphicsItem*> itemList= items(pos);
    itemList.removeAll(gridItem());
    itemList.removeAll(m_sightItem);
    if(itemList.isEmpty())
        return nullptr;

    return dynamic_cast<VisualItem*>(itemList.at(0));
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
        auto item= visualItemUnder(mouseEvent->scenePos());

        if(nullptr == item)
        {
            qCInfo(logCategory::map) << "no item under the pipette";
            return;
        }

        if(item->getType() != vmap::VisualItemController::ItemType::IMAGE)
        {
            QColor color= item->color();
            if(color.isValid())
            {
                m_ctrl->setToolColor(color);
            }
        }
    }
    else if(Core::HIGHLIGHTER == m_ctrl->tool() && leftButton)
    {
        m_ctrl->addHighLighter(mouseEvent->scenePos());
    }
    else if(Core::BUCKET == m_ctrl->tool() && leftButton)
    {
        auto item= visualItemUnder(mouseEvent->scenePos());
        if(nullptr == item)
        {
            qCInfo(logCategory::map) << "no item under the bucket";
            return;
        }

        if(item->getType() != vmap::VisualItemController::ItemType::IMAGE)
        {
            // auto cmd= new ChangeColorItemCmd(item->controller(), m_ctrl->toolColor());
            m_ctrl->askForColorChange(item->controller());
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
        auto item= visualItemUnder(mouseEvent->scenePos());
        if(item)
        {
            if(!(mouseEvent->modifiers() & Qt::CTRL) && !item->isSelected())
                clearSelection();
            item->setSelected(true);
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
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
            auto ctrl= m_currentItem->controller();
            m_ctrl->changeFogOfWar(poly, ctrl, (Core::EditionMode::Mask == m_ctrl->editionMode()));
        }
    }
    else if((nullptr != m_currentPath) && (Core::EditionMode::Painting != m_ctrl->editionMode()))
    {
        auto poly= m_currentPath->shape().toFillPolygon();
        m_ctrl->changeFogOfWar(poly, m_currentPath->controller(), (Core::EditionMode::Mask == m_ctrl->editionMode()));
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
                    m_ctrl->addCommand(new MoveItemCommand(m_movingItems, m_oldPos));
                }
                m_movingItems.clear();
                m_oldPos.clear();
            }
        }
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}

bool VMap::isNormalItem(const QGraphicsItem* item) const
{
    if(!item)
        return false;

    if((item == m_gridItem.get()) || (item == m_sightItem))
    {
        return false;
    }
    auto vItem= dynamic_cast<const VisualItem*>(item);
    if(!vItem)
        return false;
    return true;
}

VisualItem* VMap::getNormalItem(QGraphicsItem* item)
{
    if(!item)
        return nullptr;

    if((item == m_gridItem.get()) || (item == m_sightItem))
    {
        return nullptr;
    }
    auto vItem= dynamic_cast<VisualItem*>(item);
    return vItem;
}

GridItem* VMap::gridItem() const
{
    return m_gridItem.get();
}

void VMap::manageAnchor()
{
    if(m_parentItemAnchor.isNull())
        return;

    vmap::VisualItemController* child= nullptr;
    vmap::VisualItemController* parent= nullptr;
    QList<QGraphicsItem*> item1= items(m_parentItemAnchor->getStart());
    for(QGraphicsItem* item : item1)
    {
        if(item == nullptr)
            continue;

        qDebug() << item->boundingRect();

        if(!isNormalItem(item))
            continue;

        auto itemChild= getNormalItem(item);
        if(itemChild)
            child= itemChild->controller();
    }
    QList<QGraphicsItem*> item2= items(m_parentItemAnchor->getEnd());
    for(QGraphicsItem* item : item2)
    {
        if(item == nullptr)
            continue;

        if(!isNormalItem(item))
            continue;

        auto parentChild= getNormalItem(item);
        if(parentChild)
            parent= parentChild->controller();
    }
    m_ctrl->setParent(child, parent);
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

        QList<ResourcesNode*> resourcesList= resourcesData->getList();
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
                    auto path= media->path();
                    auto data= media->data();
                    auto params= MediaHelper::addImageIntoController(event->scenePos(), path, data);
                    m_ctrl->insertItemAt(params);
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
