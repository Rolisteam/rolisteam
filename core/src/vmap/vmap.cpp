#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>

#include "data/character.h"
#include "items/anchoritem.h"
#include "items/characteritem.h"
#include "items/ellipsitem.h"
#include "items/griditem.h"
#include "items/highlighteritem.h"
#include "items/imageitem.h"
#include "items/lineitem.h"
#include "items/pathitem.h"
#include "items/rectitem.h"
#include "items/ruleitem.h"
#include "items/textitem.h"
#include "vmap.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/cleveruri.h"
#include "data/cleverurimimedata.h"
#include "media/mediatype.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "userlist/rolisteammimedata.h"

#include "vmap/controller/characteritemcontroller.h"
#include "vmap/controller/ellipsecontroller.h"
#include "vmap/controller/imagecontroller.h"
#include "vmap/controller/linecontroller.h"
#include "vmap/controller/rectcontroller.h"
#include "vmap/controller/sightcontroller.h"
#include "vmap/controller/textcontroller.h"

#include "vmap/manager/characteritemcontrollermanager.h"
#include "vmap/manager/ellipscontrollermanager.h"
#include "vmap/manager/imagecontrollermanager.h"
#include "vmap/manager/linecontrollermanager.h"
#include "vmap/manager/pathcontrollermanager.h"
#include "vmap/manager/rectcontrollermanager.h"
#include "vmap/manager/textcontrollermanager.h"

#include "worker/iohelper.h"

// Undo management
#include "undoCmd/addvmapitem.h"
#include "undoCmd/deletevmapitem.h"
#include "undoCmd/movevmapitem.h"

VMap::VMap(VectorialMapController* ctrl, QObject* parent) : QGraphicsScene(parent), m_ctrl(ctrl)
{
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

    connect(m_ctrl, &VectorialMapController::toolChanged, this, [this]() { m_currentPath= nullptr; });

    // item Managers
    connect(m_ctrl->rectManager(), &RectControllerManager::rectControllerCreated, this, &VMap::addRectItem);
    connect(m_ctrl->ellipseManager(), &EllipsControllerManager::ellipsControllerCreated, this, &VMap::addEllipseItem);
    connect(m_ctrl->lineManager(), &LineControllerManager::LineControllerCreated, this, &VMap::addLineItem);
    connect(m_ctrl->imageManager(), &ImageControllerManager::imageControllerCreated, this, &VMap::addImageItem);
    connect(m_ctrl->pathManager(), &PathControllerManager::pathControllerCreated, this, &VMap::addPathItem);
    connect(m_ctrl->textManager(), &TextControllerManager::textControllerCreated, this, &VMap::addTextItem);
    connect(m_ctrl->characterManager(), &CharacterItemControllerManager::characterControllerCreated, this,
            &VMap::addCharaterItem);

    // initialization
    setBackgroundBrush(m_ctrl->backgroundColor());

    // setSceneRect(1000, 1000);
    // setSceneRect(0, 0, 1000, 1000);

    auto grid= m_ctrl->gridController();

    m_gridItem= new GridItem(grid);
    addItem(m_gridItem);
    m_gridItem->initChildPointItem();
    m_gridItem->setPos(grid->pos());
    m_gridItem->setZValue(2);

    auto sight= m_ctrl->sightController();
    m_sightItem= new SightItem(sight, m_characterItemMap);
    addItem(m_sightItem);
    m_sightItem->initChildPointItem();
    m_sightItem->setPos(0, 0);
    m_sightItem->setZValue(1);

    m_ctrl->loadItems();
    m_gridItem->setVisible(grid->visible());
    m_sightItem->setVisible(sight->visible());
    computePattern();
}

/*VMap::VMap(int width, int height, QString& title, QColor& bgColor, QObject* parent)
    : QGraphicsScene(0, 0, width, height, parent), m_ctrl->layer()(Core::Layer::GROUND)
{
    m_title= title;

    m_ctrl->backgroundColor()= bgColor;

    initMap();
}*/

/*void VMap::initMap()
{
    //PlayerModel* list= PlayerModel::instance();
    //connect(list, SIGNAL(characterDeleted(Character*)), this, SLOT(characterHasBeenDeleted(Character*)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionHasChanged()));

    m_currentItem= nullptr;
    m_currentPath= nullptr;
    m_fogItem= nullptr;
    m_itemMap= new QMap<QString, VisualItem*>;
    m_characterItemMap= new QMap<QString, CharacterItem*>();
    setItemIndexMethod(QGraphicsScene::NoIndex);

    m_sightItem= new SightItem(m_ctrl, m_characterItemMap);
    m_gridItem= new GridItem(m_ctrl);

    // Set default value
}*/

void VMap::updateLayer()
{
    auto const& values= m_itemMap->values();
    for(auto& item : values)
    {
        item->updateItemFlags();
    }
}
void VMap::addLineItem(vmap::LineController* lineCtrl, bool editing)
{
    auto item= new LineItem(lineCtrl);
    addItem(item);
    item->setPos(lineCtrl->pos());
    if(editing)
        m_currentItem= item;
}

void VMap::addImageItem(vmap::ImageController* imgCtrl)
{
    auto img= new ImageItem(imgCtrl);
    addItem(img);
    img->setPos(imgCtrl->pos());
}

void VMap::addRectItem(vmap::RectController* rectCtrl, bool editing)
{
    qDebug() << "add rect Item editing:" << editing;
    auto item= new RectItem(rectCtrl);
    addItem(item);
    item->setPos(rectCtrl->pos());
    if(editing)
        m_currentItem= item;
}

void VMap::addEllipseItem(vmap::EllipseController* ellisCtrl, bool editing)
{
    auto item= new EllipsItem(ellisCtrl);
    addItem(item);
    item->setPos(ellisCtrl->pos());
    if(editing)
        m_currentItem= item;
}

void VMap::addTextItem(vmap::TextController* textCtrl)
{
    auto tmp= new TextItem(textCtrl);
    addItem(tmp);
    tmp->setPos(textCtrl->pos());
}

void VMap::addCharaterItem(vmap::CharacterItemController* itemCtrl)
{
    auto tmp= new CharacterItem(itemCtrl);
    addItem(tmp);
    tmp->setPos(itemCtrl->pos());
}

void VMap::addPathItem(vmap::PathController* pathCtrl, bool editing)
{
    auto path= new PathItem(pathCtrl);

    if(editing)
    {
        m_currentItem= path;
        if(!pathCtrl->penLine())
        {
            m_currentPath= path;
        }
    }
    addItem(path);
    path->setPos(pathCtrl->pos());
}

/*void VMap::initScene()
{
    addNewItem(new AddVmapItemCommand(m_sightItem, true, this), false);
    m_sightItem->initChildPointItem();
    m_sightItem->monitorView();
    m_sightItem->setPos(0, 0);
    m_sightItem->setZValue(1);
    m_sightItem->setVisible(false);


}

void VMap::setBackGroundColor(QColor bgcolor)
{
    if(m_ctrl->backgroundColor() == bgcolor)
        return;
    m_ctrl->backgroundColor()= bgcolor;
    computePattern();
    update();
    emit mapChanged();
}*/

void VMap::characterHasBeenDeleted(Character* character)
{
    QList<CharacterItem*> list= getCharacterOnMap(character->uuid());
    for(CharacterItem* item : list)
    {
        // removeItemFromScene(item->getId());
    }
}

void VMap::fill(NetworkMessageWriter& msg)
{
    if(m_sightItem == nullptr || m_itemMap == nullptr)
        return;

    msg.string8(getId());
    msg.string16(m_ctrl->name());
    // msg.rgb(getBackGroundColor().rgb());
    msg.uint8(static_cast<quint8>(m_ctrl->layer()));
    // msg.string8(m_sightItem->getId());
    msg.uint8(static_cast<quint8>(m_ctrl->permission()));
    msg.uint8(static_cast<quint8>(m_ctrl->visibility()));
    msg.uint64(m_zIndex);
    msg.uint8(m_ctrl->characterVision());
    msg.uint8(static_cast<quint8>(m_ctrl->gridPattern()));
    msg.uint8(m_ctrl->gridVisibility());
    msg.uint32(static_cast<quint8>(m_ctrl->gridSize()));
    msg.uint8(m_ctrl->gridAbove());
    msg.uint8(m_ctrl->collision());
    msg.rgb(m_ctrl->gridColor().rgb());
    msg.uint64(static_cast<quint64>(m_itemMap->size()));
    m_sightItem->fillMessage(&msg);
}
void VMap::readMessage(NetworkMessageReader& msg, bool readCharacter)
{
    if(nullptr == m_sightItem)
        return;

    m_id= msg.string8();
    m_ctrl->setName(msg.string16());
    // m_ctrl->backgroundColor()= msg.rgb();
    Core::Layer layer= static_cast<Core::Layer>(msg.uint8());
    QString idSight= msg.string8();
    // m_sightItem->setId(idSight);
    Core::PermissionMode permissionMode= static_cast<Core::PermissionMode>(msg.uint8());
    Core::VisibilityMode mode= static_cast<Core::VisibilityMode>(msg.uint8());
    m_zIndex= msg.uint64();
    quint8 enableCharacter= msg.uint8();

    // Grid
    Core::GridPattern gridP= static_cast<Core::GridPattern>(msg.uint8());
    bool showGrid= static_cast<bool>(msg.uint8());
    int gridSize= static_cast<int>(msg.uint32());
    bool gridAbove= static_cast<bool>(msg.uint8());
    bool collision= static_cast<bool>(msg.uint8());
    QColor colorGrid= msg.rgb();

    quint64 itemCount= msg.uint64();

    m_sightItem->readItem(&msg);

    if(readCharacter)
    {
        for(quint64 i= 0; i < itemCount; ++i)
        {
            // processAddItemMessage(&msg);
        }
    }

    blockSignals(true);
    /*setCurrentLayer(layer);
    setPermissionMode(permissionMode);
    setOption(Core::EnableCharacterVision, enableCharacter);
    setOption(Core::GridPatternProperty, gridP);
    setOption(Core::GridAbove, gridAbove);
    setOption(Core::GridColor, colorGrid);
    setOption(Core::GridSize, gridSize);
    setOption(Core::ShowGrid, showGrid);
    setOption(Core::CollisionStatus, collision);
    setVisibilityMode(mode);
    blockSignals(false);*/
    emit mapStatutChanged();
}

void VMap::sendAllItems(NetworkMessageWriter& msg)
{
    for(QString& key : m_sortedItemList)
    {
        VisualItem* item= m_itemMap->value(key);
        if(nullptr != item)
        {
            msg.uint8(item->getType());
            item->fillMessage(&msg);
        }
    }
}

void VMap::cleanFogEdition()
{
    if(nullptr == m_fogItem)
        return;

    if(nullptr != m_fogItem->scene())
    {
        removeItem(m_fogItem);
    }
    m_fogItem= nullptr;
}

void VMap::updateFog()
{
    m_sightItem->markDirty();
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

// void VMap::addImageItem(const QString& imgPath)
//{
/*  ImageItem* led= new ImageItem(m_ctrl);
  led->setImageUri(imgPath);
  // addNewItem(new AddVmapItemCommand(led, true, this), true);
  led->setPos(QPointF(0, 0));
  sendOffItem(led);*/
//}

// void VMap::addImageItem(const QImage& img)
//{
/*  ImageItem* led= new ImageItem(m_ctrl);
  led->setImage(img);
  // addNewItem(new AddVmapItemCommand(led, true, this), true);
  led->setPos(QPointF(0, 0));
  sendOffItem(led);*/
//}

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
    params.insert({QStringLiteral("position"), pos});
    params.insert({QStringLiteral("color"), m_ctrl->toolColor()});
    params.insert({QStringLiteral("penWidth"), m_ctrl->penSize()});
    params.insert({QStringLiteral("tool"), m_ctrl->tool()});
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
        auto hitem= new HighlighterItem(mouseEvent->scenePos(), m_ctrl->penSize(), m_ctrl->toolColor());
        addItem(hitem);
        hitem->setPos(mouseEvent->scenePos());
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
        addItem(m_parentItemAnchor);
        m_parentItemAnchor->setPos(mouseEvent->scenePos());
    }
    else if(Core::RULE == m_ctrl->tool() && leftButton)
    {
        m_ruleItem= new RuleItem(m_ctrl);
        addItem(m_ruleItem);
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
        cleanFogEdition();
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
    if((!m_currentPath.isNull()) && ((Core::EditionMode::Painting == m_ctrl->editionMode())))
    {
        if(vmap::VisualItemController::ItemType::PATH == m_currentPath->getType())
        {
            PathItem* itm= dynamic_cast<PathItem*>(m_currentPath.data());
            if(nullptr != itm)
            {
                itm->release();
            }
        }
    }
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
            sendOffItem(m_currentItem);
        }
        else
        {
            auto poly= m_currentItem->shape().toFillPolygon();
            poly= poly.translated(m_currentItem->pos());
            m_ctrl->changeFogOfWar(poly, (Core::EditionMode::Mask == m_ctrl->editionMode()));
            /*QPolygonF* poly= new QPolygonF();
            m_currentFog= m_sightItem->addFogPolygon(poly, (Core::EditionMode::Mask == m_ctrl->editionMode()));
            sendItemToAll(m_sightItem);*/
            if(nullptr == m_currentPath)
            {
                removeItem(m_currentItem);
            }
        }
    }
    else if((nullptr != m_currentPath) && (Core::EditionMode::Painting != m_ctrl->editionMode()))
    {
        /*QPolygonF* poly= new QPolygonF();
        *poly= m_currentPath->shape().toFillPolygon();
        m_currentFog->setPolygon(poly);*/
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
void VMap::setAnchor(QGraphicsItem* child, QGraphicsItem* parent, bool send)
{
    if(nullptr == child)
        return;

    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::SetParentItem);
    msg.string8(m_id);
    VisualItem* childItem= dynamic_cast<VisualItem*>(child);

    if(nullptr != childItem)
    {
        // msg.string8(childItem->getId());
    }

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
    VisualItem* paItem= dynamic_cast<VisualItem*>(parent);
    if(nullptr != paItem)
    {
        // msg.string8(paItem->getId());
    }
    else
    {
        msg.string8("nullptr");
    }
    bool hasMoved= false;
    if(nullptr == parent)
    {
        if(send)
        {
            msg.sendToServer();
        }
        child->setParentItem(parent);
        hasMoved= true;
    }
    else if(parent->parentItem() != child)
    {
        if(send)
        {
            msg.sendToServer();
        }
        child->setParentItem(parent);
        hasMoved= true;
    }
    if(!(pos2.isNull() && parent == nullptr) && hasMoved)
    {
        child->setPos(pos2);
    }
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

void VMap::sendOffItem(VisualItem* item, bool doInitPoint)
{
    if(m_undoStack.isNull())
        return;

    /*  if((nullptr != m_currentAddCmd) && (m_currentAddCmd->getItem() == item))
      {
          m_currentAddCmd->setInitPoint(doInitPoint);
          if(m_currentAddCmd->isUndoable())
          {
              m_undoStack->push(m_currentAddCmd);
          }
          else
          {
              m_currentAddCmd->redo();
              if(m_currentAddCmd->hasToBeDeleted())
              {
                  delete m_currentAddCmd;
                  m_currentAddCmd= nullptr;
              }
          }
      }*/
}

QString VMap::getId() const
{
    return m_id;
}
void VMap::setId(QString id)
{
    m_id= id;
}
// write
void VMap::saveFile(QDataStream& out)
{
    if(m_itemMap == nullptr)
        return;

    out << m_ctrl->name();
    out << m_ctrl->backgroundColor();
    out << m_zIndex;

    /*auto map= m_ctrl;
    out << static_cast<quint64>(map.size());

    for(auto pair : map)
    {
        out << static_cast<int>(pair.first);
        out << pair.second;
    }

    // FOG
    out << *m_sightItem;

    out << m_sortedItemList.size();
    for(QString& key : m_sortedItemList) // m_itemMap->values()
    {
        VisualItem* tmp= m_itemMap->value(key);
        if(nullptr != tmp)
        {
            out << static_cast<int>(tmp->getType()) << *tmp << tmp->pos().x() << tmp->pos().y();
        }
    }*/
}
// read
void VMap::openFile(QDataStream& in)
{
    /* if(m_itemMap == nullptr)
         return;

     in >> m_width;
     in >> m_height;

     setSceneRect();
     in >> m_title;
     in >> m_ctrl->backgroundColor();
     in >> m_zIndex;

     int propertyCount;
     in >> propertyCount;

     for(int i= 0; i < propertyCount; ++i)
     {
         int pro;
         in >> pro;
         QVariant var;
         in >> var;
         setOption(static_cast<Core::Properties>(pro), var);
     }
     setCurrentLayer(getOption(Core::MapLayer).value<Core::Layer>());

     // FOG
     in >> *m_sightItem;

     int numberOfItem;
     in >> numberOfItem;

     for(int i= 0; i < numberOfItem; ++i)
     {
         VisualItem* item= nullptr;
         CharacterItem* charItem= nullptr;
         int tmptype;
         in >> tmptype;
         VisualItem::ItemType type= static_cast<VisualItem::ItemType>(tmptype);

         switch(type)
         {
         case Core::TEXT:
             item= new TextItem();
             break;
         case Core::CHARACTER:
             charItem= new CharacterItem();
             item= charItem;
             break;
         case Core::LINE:
             item= new LineItem();

             break;
         case Core::RECT:
             item= new RectItem();
             break;
         case Core::ELLISPE:
             item= new EllipsItem();

             break;
         case Core::PATH:
             item= new PathItem();
             break;
         case Core::SIGHT:
             // remove the old one
             m_orderedItemList.removeAll(m_sightItem);
             m_sortedItemList.removeAll(m_sightItem->getId());
             m_itemMap->remove(m_sightItem->getId());
             // add new
             item= m_sightItem;
             break;
         case Core::GRID:
             // remove the old one
             m_orderedItemList.removeAll(m_gridItem);
             m_sortedItemList.removeAll(m_gridItem->getId());
             m_itemMap->remove(m_gridItem->getId());
             // add new
             item= m_gridItem;
             break;
         case Core::IMAGE:
             item= new ImageItem();
             break;
         default:
             break;
         }
         if(nullptr != item)
         {
             in >> *item;

             qreal x, y;
             in >> x;
             in >> y;
             addNewItem(new AddVmapItemCommand(item, false, this), false);
             item->setPos(x, y);
             item->initChildPointItem();
             if(nullptr != charItem)
             {
                 insertCharacterInMap(charItem);
             }
         }
     }
     ensureFogAboveAll();
     computePattern();
     update();*/
}

void VMap::addCharacter(Character* p, QPointF pos)
{
    /*  CharacterItem* item= new CharacterItem(m_ctrl); //,p, pos
      if(nullptr != item)
      {
          item->setZValue(m_zIndex);
          item->initChildPointItem();
          // addNewItem(new AddVmapItemCommand(item, true, this), true);
          insertCharacterInMap(item);
          sendOffItem(item);
      }*/
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

QUndoStack* VMap::getUndoStack() const
{
    return m_undoStack;
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

void VMap::addItemFromData(VisualItem* item)
{
    if(nullptr == item)
        return;

    /*auto id= item->getId();
    if(isItemStorable(item))
    {
        m_itemMap->insert(id, item);
        if(!m_sortedItemList.contains(id))
            m_sortedItemList.append(id);
    }*/
}

void VMap::insertItemFromData(VisualItem*, int) {}

bool VMap::isItemStorable(VisualItem* item)
{
    if((item->getType() == vmap::VisualItemController::ItemType::ANCHOR)
       || (item->getType() == vmap::VisualItemController::ItemType::GRID)
       || (item->getType() == vmap::VisualItemController::ItemType::SIGHT)
       || (item->getType() == vmap::VisualItemController::ItemType::RULE)
       || (item->getType() == vmap::VisualItemController::ItemType::HIGHLIGHTER))
    {
        return false;
    }
    else
    {
        return true;
    }
}

// void VMap::addItem(VisualItem* item)
//{
// QGraphicsScene::addItem(item);
/* if((nullptr != itemCmd) && (!itemCmd->hasError()))
 {
     itemCmd->setUndoable(undoable);
     m_currentAddCmd= itemCmd;
     VisualItem* item= m_currentAddCmd->getItem();

     if(isItemStorable(item))
     {
         m_orderedItemList.append(item);
     }

     item->setZValue(m_zIndex);

     if((Core::EditionMode::Painting == m_ctrl->editionMode()) || (fromNetwork))
     {
         if(isItemStorable(item))
         {
             auto id= item->getId();
             m_itemMap->insert(id, item);
             if(!m_sortedItemList.contains(id))
                 m_sortedItemList << id;
         }
     }

     if(itemCmd->isNpc())
     {
         emit npcAdded();
     }
 }*/
//}
QList<CharacterItem*> VMap::getCharacterOnMap(QString id)
{
    QList<CharacterItem*> result;
    auto const& values= m_characterItemMap->values();
    for(auto& item : values)
    {
        if(nullptr != item)
        {
            if(item->getCharacterId() == id)
            {
                result.append(item);
            }
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
        bis->initChildPointItem();
    }
}

void VMap::removeItemFromScene(QString id, bool sendToAll, bool undoable)
{
    /* if(m_sightItem->getId() == id || m_gridItem->getId() == id)
     {
         return;
     }*/
    VisualItem* item= m_itemMap->value(id);

    if(nullptr != item)
    {
        if(undoable)
        {
            // m_undoStack->push(new DeleteVmapItemCommand(this, item, sendToAll));
        }
        else
        {
            //  DeleteVmapItemCommand cmd(this, item, sendToAll);
            //  cmd.redo();
        }
    }
}

void VMap::sendItemToAll(VisualItem* item)
{
    /*NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::GeometryItemChanged);
    msg.string8(m_id);
    msg.string16(item->getId());
    item->fillMessage(&msg);
    msg.sendToServer();*/
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
        for(auto& id : idListToRemove)
        {
            removeItemFromScene(id);
        }
        setFocusItem(nullptr);
        event->accept();
        //  return;
    }
    QGraphicsScene::keyPressEvent(event);
}
// void VMap::setPermissionMode(Core::PermissionMode mode)
//{
/* if(m_ctrl->permission() == mode)
     return;

 m_ctrl->setOption(Core::PermissionModeProperty, mode);
 if(!m_ctrl->getOption(Core::LocalIsGM).toBool())
 {
     auto const& values= m_itemMap->values();
     for(auto& item : values)
     {
         item->updateItemFlags();
     }
 }*/
//}
/*QString VMap::getPermissionModeText()
{
    QStringList permissionData;
    permissionData << tr("No Right") << tr("His character") << tr("All Permissions");
    return permissionData.at(m_ctrl->permission());
}
Core::PermissionMode VMap::getPermissionMode()
{
    return m_ctrl->permission();
}*/

void VMap::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    event->acceptProposedAction();
}
void VMap::setLocalId(QString id)
{
    m_localUserId= id;
}
const QString& VMap::getLocalUserId() const
{
    return m_localUserId;
}

void VMap::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    auto data= event->mimeData();
    if(data->hasFormat("rolisteam/userlist-item"))
    {
        qInfo() << "VMAP dropEvent character";
        const RolisteamMimeData* rolisteamData= qobject_cast<const RolisteamMimeData*>(data);
        Person* item= rolisteamData->getData();
        Character* character= dynamic_cast<Character*>(item);
        if(character)
        {
            addCharacter(character, event->scenePos());
        }
    }
    else if(data->hasFormat("rolisteam/cleverurilist"))
    {
        const CleverUriMimeData* resourcesData= qobject_cast<const CleverUriMimeData*>(data);
        if(nullptr != resourcesData)
        {
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
                        // ImageItem* led= new ImageItem(m_ctrl);
                        // led->setImageUri(media->getUri());

                        std::map<QString, QVariant> params;
                        params.insert({QStringLiteral("position"), event->scenePos()});
                        params.insert({QStringLiteral("tool"), Core::SelectableTool::IMAGE});
                        if(media->exists())
                            params.insert({QStringLiteral("path"), m_ctrl->tool()});

                        if(media->hasData())
                            params.insert({QStringLiteral("data"), media->data()});
                        m_ctrl->insertItemAt(params);
                        //  addNewItem(new AddVmapItemCommand(m_ctrl, true, this), true);
                        // led->setPos(event->scenePos());
                        // sendOffItem(led);
                    }
                }
                else if(resource->type() == ResourcesNode::Person)
                {
                    qInfo() << "VMAP dropEvent: Character from resources list";
                    Person* item= dynamic_cast<Character*>(resource);
                    Character* character= dynamic_cast<Character*>(item);
                    if(character)
                    {
                        addCharacter(character, event->scenePos());
                    }
                }
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
                if(IOHelper::loadToken(url.toLocalFile(), params))
                {
                    params.insert({QStringLiteral("position"), event->scenePos()});
                    params.insert({QStringLiteral("color"), m_ctrl->toolColor()});
                    params.insert({QStringLiteral("penWidth"), m_ctrl->penSize()});
                    params.insert({QStringLiteral("tool"), Core::SelectableTool::NonPlayableCharacter});
                    m_ctrl->insertItemAt(params);
                }
            }
            else if(url.isLocalFile())
            {
                qInfo() << "VMAP dropEvent: Image from file";
                std::map<QString, QVariant> params;
                params.insert({QStringLiteral("position"), event->scenePos()});
                params.insert({QStringLiteral("tool"), Core::SelectableTool::IMAGE});
                params.insert({QStringLiteral("path"), url.toLocalFile()});
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
        copy->initChildPointItem();
        // copy->setLayer(item->getLayer());
        //  addNewItem(new AddVmapItemCommand(copy, false, this), true);
        copy->setPos(item->pos());
        clearSelection();
        item->clearFocus();
        copy->clearFocus();
        clearFocus();
        setFocusItem(copy);
        update();
        sendOffItem(copy, false);
    }
}
bool VMap::isIdle() const
{
    return m_currentItem.isNull();
}
void VMap::ownerHasChangedForCharacterItem(Character* item, CharacterItem* cItem)
{
    if(nullptr != item)
    {
        // Get all item with the key
        QList<CharacterItem*> list= m_characterItemMap->values(item->uuid());

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
    }
}

void VMap::insertCharacterInMap(CharacterItem* item)
{
    if((nullptr != m_characterItemMap) && (nullptr != item))
    {
        m_characterItemMap->insertMulti(item->getCharacterId(), item);
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
        }
    }
}

/*bool VMap::setVisibilityMode(Core::VisibilityMode mode)
{
    bool result= false;
    m_ctrl->setVisibility(mode);
    bool visibilitySight= false;
    bool visibilityItems= false;
    if(!m_ctrl->localGM())
    {
        if(mode == Core::HIDDEN)
        {
            visibilityItems= false;
        }
        else if(mode == Core::ALL)
        {
            visibilityItems= true;
        }
        else if(Core::FOGOFWAR == mode)
        {
            visibilitySight= true;
            visibilityItems= true;
        }
    }
    else
    {
        visibilityItems= true;
        if(Core::FOGOFWAR == mode)
        {
            visibilitySight= true;
        }
        else
        {
            visibilitySight= false;
        }
    }
    auto const& values= m_itemMap->values();
    for(auto& item : values)
    {
        item->setVisible(visibilityItems);
    }
    if(nullptr != m_sightItem)
    {
        m_sightItem->setVisible(visibilitySight);*/
/* m_ctrl->propertyHashRef()->insert(Core::FogOfWarStatus, visibilitySight);
 if((visibilitySight) && (m_ctrl->propertyHashRef()->value(Core::LocalIsGM).toBool() == false))
 {
     for(auto& item : values)
     {
         if(item->getType() == Core::CHARACTER)
         {
             item->setToolTip("");
         }
     }
 }*/
/*    }
    return result;
}*/
void VMap::hideOtherLayers(bool b)
{
    auto const& values= m_itemMap->values();
    for(auto& item : values)
    {
        if(item == m_sightItem || item == m_gridItem)
            continue;

        /*if((b) && (item->getLayer() != m_ctrl->layer()))
        {
            item->setVisible(false);
        }
        else
        {
            item->setVisible(true);
        }*/
    }
}

/*QString VMap::getVisibilityModeText()
{
    QStringList visibilityData;
    visibilityData << tr("Hidden") << tr("Fog Of War") << tr("All visible");

    if(m_ctrl->visibility() < visibilityData.size())
    {
        return visibilityData.at(m_ctrl->visibility());
    }
    return {};
}*/
/*Core::EditionMode VMap::getEditionMode()
{
    return m_ctrl->editionMode();
}*/

SightItem* VMap::getFogItem() const
{
    return m_sightItem;
}
GridItem* VMap::getGridItem() const
{
    return m_gridItem;
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

void VMap::rollInit(Core::CharacterScope zone)
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
}

void VMap::cleanUpInit(Core::CharacterScope zone)
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
}
void VMap::addCommand(QUndoCommand* cmd)
{
    if(m_undoStack.isNull() || nullptr == cmd)
        return;
    m_undoStack->push(cmd);
}
int VMap::getItemCount()
{
    return m_itemMap->size();
}
int VMap::getSortedItemCount()
{
    return m_sortedItemList.size();
}
int VMap::getOrderedItemCount()
{
    return m_orderedItemList.size();
}
