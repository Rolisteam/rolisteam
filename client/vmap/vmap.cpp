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
#include "items/highlighteritem.h"
#include "items/imageitem.h"
#include "items/lineitem.h"
#include "items/pathitem.h"
#include "items/rectitem.h"
#include "items/ruleitem.h"
#include "items/textitem.h"
#include "vmap.h"

#include "data/cleveruri.h"
#include "data/cleverurimimedata.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "userlist/rolisteammimedata.h"

// Undo management
#include "undoCmd/addvmapitem.h"
#include "undoCmd/changecoloritem.h"
#include "undoCmd/deletevmapitem.h"
#include "undoCmd/movevmapitem.h"

VMap::VMap(QObject* parent) : QGraphicsScene(parent), m_currentLayer(VisualItem::GROUND)
{
    initMap();
}

VMap::VMap(int width, int height, QString& title, QColor& bgColor, QObject* parent)
    : QGraphicsScene(0, 0, width, height, parent), m_currentLayer(VisualItem::GROUND)
{
    m_title= title;

    m_bgColor= bgColor;
    setBackgroundBrush(m_bgColor);
    initMap();
}
void VMap::initMap()
{
    m_editionMode= VToolsBar::Painting;
    m_propertiesHash= new QHash<VisualItem::Properties, QVariant>();
    m_zIndex= 0;

    PlayersList* list= PlayersList::instance();
    connect(list, SIGNAL(characterDeleted(Character*)), this, SLOT(characterHasBeenDeleted(Character*)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionHasChanged()));

    m_penSize= 1;
    m_id= QUuid::createUuid().toString();
    m_currentItem= nullptr;
    m_currentPath= nullptr;
    m_fogItem= nullptr;
    m_itemMap= new QMap<QString, VisualItem*>;
    m_characterItemMap= new QMap<QString, CharacterItem*>();
    setItemIndexMethod(QGraphicsScene::NoIndex);

    m_sightItem= new SightItem(m_characterItemMap);
    m_gridItem= new GridItem();

    // Set default value
    setOption(VisualItem::ShowNpcName, false);
    setOption(VisualItem::ShowPcName, false);
    setOption(VisualItem::ShowNpcNumber, false);
    setOption(VisualItem::ShowHealthStatus, false);
    setOption(VisualItem::ShowInitScore, true);
    setOption(VisualItem::ShowHealthBar, false);
    setOption(VisualItem::ShowGrid, false);
    setOption(VisualItem::LocalIsGM, false);
    setOption(VisualItem::EnableCharacterVision, false);
    setOption(VisualItem::CollisionStatus, false);
    setOption(VisualItem::PermissionMode, Map::GM_ONLY);
    setOption(VisualItem::FogOfWarStatus, false);
    setOption(VisualItem::VisibilityMode, VMap::HIDDEN);
    setOption(VisualItem::MapLayer, m_currentLayer);
    setOption(VisualItem::GridPattern, VMap::NONE);
    setOption(VisualItem::GridColor, QColor(Qt::black));
    setOption(VisualItem::GridSize, 50);
    setOption(VisualItem::Scale, 1.0);
    setOption(VisualItem::Unit, VMap::M);
    setOption(VisualItem::GridAbove, false);
    setOption(VisualItem::HideOtherLayers, false);
}

VToolsBar::SelectableTool VMap::getSelectedtool() const
{
    return m_selectedtool;
}
void VMap::setCurrentTool(VToolsBar::SelectableTool selectedtool)
{
    cleanFogEdition();
    if((m_selectedtool == VToolsBar::PATH) && (m_selectedtool != selectedtool))
    {
        m_currentPath= nullptr;
    }
    m_selectedtool= selectedtool;
    m_currentItem= nullptr;
}
VisualItem::Layer VMap::currentLayer() const
{
    return m_currentLayer;
}

void VMap::setCurrentLayer(const VisualItem::Layer& currentLayer)
{
    if(editLayer(currentLayer))
    {
        emit mapChanged();
    }
}

void VMap::initScene()
{
    addNewItem(new AddVmapItemCommand(m_sightItem, true, this), false);
    m_sightItem->initChildPointItem();
    m_sightItem->setPos(0, 0);
    m_sightItem->setZValue(1);
    m_sightItem->setVisible(false);

    addNewItem(new AddVmapItemCommand(m_gridItem, true, this), false);
    m_gridItem->initChildPointItem();
    m_gridItem->setPos(0, 0);
    m_gridItem->setZValue(2);
    m_gridItem->setVisible(false);
}
// TODO remove those two value.
void VMap::setWidth(int width)
{
    m_width= width;
    setSceneRect();
}

void VMap::setHeight(int height)
{
    m_height= height;
    setSceneRect();
}

void VMap::setTitle(QString title)
{
    if(m_title == title)
        return;

    m_title= title;
    emit mapChanged();
}

void VMap::setBackGroundColor(QColor bgcolor)
{
    if(m_bgColor == bgcolor)
        return;
    m_bgColor= bgcolor;
    computePattern();
    update();
    emit mapChanged();
}

void VMap::setSceneRect()
{
    QGraphicsScene::setSceneRect(0, 0, m_width, m_height);
}
int VMap::mapWidth() const
{
    return m_width;
}
int VMap::mapHeight() const
{
    return m_height;
}
const QString& VMap::getMapTitle() const
{
    return m_title;
}

void VMap::setPenSize(quint16 p)
{
    m_penSize= p;
}
void VMap::characterHasBeenDeleted(Character* character)
{
    QList<CharacterItem*> list= getCharacterOnMap(character->getUuid());
    for(CharacterItem* item : list)
    {
        removeItemFromScene(item->getId());
    }
}

void VMap::fill(NetworkMessageWriter& msg)
{
    if(m_sightItem == nullptr || m_itemMap == nullptr)
        return;

    msg.string8(getId());
    msg.string16(getMapTitle());
    msg.rgb(getBackGroundColor().rgb());
    msg.uint16(static_cast<quint16>(mapWidth()));
    msg.uint16(static_cast<quint16>(mapHeight()));
    msg.uint8(static_cast<quint8>(m_currentLayer));
    msg.string8(m_sightItem->getId());
    msg.uint8(static_cast<quint8>(getPermissionMode()));
    msg.uint8(static_cast<quint8>(getOption(VisualItem::VisibilityMode).toInt()));
    msg.uint64(m_zIndex);
    msg.uint8(getOption(VisualItem::EnableCharacterVision).toBool());
    msg.uint8(static_cast<quint8>(getOption(VisualItem::GridPattern).toInt()));
    msg.uint8(getOption(VisualItem::ShowGrid).toBool());
    msg.uint32(static_cast<quint8>(getOption(VisualItem::GridSize).toInt()));
    msg.uint8(getOption(VisualItem::GridAbove).toBool());
    msg.uint8(getOption(VisualItem::CollisionStatus).toBool());
    msg.rgb(getOption(VisualItem::GridColor).value<QColor>().rgb());
    msg.uint64(static_cast<quint64>(m_itemMap->size()));
    m_sightItem->fillMessage(&msg);
}
void VMap::readMessage(NetworkMessageReader& msg, bool readCharacter)
{
    if(nullptr == m_sightItem)
        return;

    m_id= msg.string8();
    m_title= msg.string16();
    m_bgColor= msg.rgb();
    setWidth(msg.uint16());
    setHeight(msg.uint16());
    VisualItem::Layer layer= static_cast<VisualItem::Layer>(msg.uint8());
    QString idSight= msg.string8();
    m_sightItem->setId(idSight);
    Map::PermissionMode permissionMode= static_cast<Map::PermissionMode>(msg.uint8());
    VMap::VisibilityMode mode= static_cast<VMap::VisibilityMode>(msg.uint8());
    m_zIndex= msg.uint64();
    quint8 enableCharacter= msg.uint8();

    // Grid
    GRID_PATTERN gridP= static_cast<GRID_PATTERN>(msg.uint8());
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
            processAddItemMessage(&msg);
        }
    }

    blockSignals(true);
    editLayer(layer);
    setPermissionMode(permissionMode);
    setOption(VisualItem::EnableCharacterVision, enableCharacter);
    setOption(VisualItem::GridPattern, gridP);
    setOption(VisualItem::GridAbove, gridAbove);
    setOption(VisualItem::GridColor, colorGrid);
    setOption(VisualItem::GridSize, gridSize);
    setOption(VisualItem::ShowGrid, showGrid);
    setOption(VisualItem::CollisionStatus, collision);
    setVisibilityMode(mode);
    blockSignals(false);
    emit mapStatutChanged();
}

void VMap::processCharacterStateHasChanged(NetworkMessageReader& msg)
{
    QString idItem= msg.string8();
    VisualItem* item= m_itemMap->value(idItem);
    if(item->getType() == VisualItem::CHARACTER)
    {
        QString idCharacter= msg.string8();
        QList<CharacterItem*> list= getCharacterOnMap(idCharacter);
        for(auto characterItem : list)
        {
            if(characterItem->getId() == idItem)
            {
                characterItem->readCharacterStateChanged(msg);
            }
        }
    }
}

void VMap::processCharacterHasChanged(NetworkMessageReader& msg)
{
    QString idItem= msg.string8();
    VisualItem* item= m_itemMap->value(idItem);
    if(item->getType() == VisualItem::CHARACTER)
    {
        auto character= dynamic_cast<CharacterItem*>(item);
        if(character == nullptr)
            return;

        QString idCharacter= msg.string8();
        if(idCharacter == character->getCharacterId())
            character->readCharacterChanged(msg);
    }
}

VisualItem::Layer VMap::getCurrentLayer() const
{
    return m_currentLayer;
}
QString VMap::getCurrentLayerText() const
{
    return VisualItem::getLayerToText(m_currentLayer);
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
void VMap::setEditionMode(VToolsBar::EditionMode mode)
{
    m_editionMode= mode;
}
void VMap::cleanFogEdition()
{
    if(nullptr != m_fogItem)
    {
        if(nullptr != m_fogItem->scene())
        {
            removeItem(m_fogItem);
        }
        m_fogItem= nullptr;
    }
}

void VMap::updateItem()
{
    switch(m_selectedtool)
    {
    case VToolsBar::PATH:
    {
        m_currentPath->setNewEnd(m_first);
        update();
    }
    break;
    default:
        break;
    }
}
void VMap::addImageItem(QImage img)
{
    ImageItem* led= new ImageItem();
    led->setImage(img);
    QPointF size(img.width(), img.height());
    led->setNewEnd(size);
    // led->initChildPointItem();
    addNewItem(new AddVmapItemCommand(led, true, this), true);
    sendOffItem(led);
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

void VMap::addItem()
{
    if(VToolsBar::PIPETTE == m_selectedtool)
    {
        QList<QGraphicsItem*> itemList= items(m_first);
        itemList.removeAll(m_gridItem);
        if(!itemList.isEmpty())
        {
            VisualItem* item= dynamic_cast<VisualItem*>(itemList.at(0));
            if(nullptr != item)
            {
                if(item->getType() != VisualItem::IMAGE)
                {
                    QColor color= item->getColor();
                    if(color.isValid())
                    {
                        emit colorPipette(color);
                    }
                }
            }
        }
    }
    else if(VToolsBar::BUCKET == m_selectedtool)
    {
        QList<QGraphicsItem*> itemList= items(m_first);
        itemList.removeAll(m_gridItem);
        if(!itemList.isEmpty())
        {
            VisualItem* item= dynamic_cast<VisualItem*>(itemList.at(0));
            if(nullptr != item)
            {
                if(item->getType() != VisualItem::IMAGE)
                {
                    auto cmd= new ChangeColorItemCmd(item, m_itemColor, m_id);
                    if(!m_undoStack.isNull())
                        m_undoStack->push(cmd);
                }
            }
        }
    }
    else
    {
        AddVmapItemCommand* itemCmd= new AddVmapItemCommand(m_selectedtool, this, m_first, m_itemColor, m_penSize);
        if(itemCmd->hasError())
        {
            delete itemCmd;
            return;
        }
        m_currentItem= itemCmd->getItem();
        m_currentPath= itemCmd->getPath();
        if(m_currentItem.isNull())
            return;
        m_currentItem->setPropertiesHash(m_propertiesHash);
        bool undoable= true;
        if(VToolsBar::Painting != m_editionMode || m_selectedtool == VToolsBar::HIGHLIGHTER
           || m_selectedtool == VToolsBar::RULE)
        {
            undoable= false;
        }
        addNewItem(itemCmd, undoable); //(VToolsBar::Painting == m_editionMode));

        if(VToolsBar::Painting != m_editionMode)
        {
            m_fogItem= m_currentItem;
        }
    }
}
void VMap::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if(m_selectedtool == VToolsBar::HANDLER)
    {
        if(mouseEvent->button() == Qt::LeftButton)
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
    }
    else if(mouseEvent->button() == Qt::LeftButton)
    {
        m_first= mouseEvent->scenePos();
        m_end= m_first;
        if(m_currentPath == nullptr)
        {
            addItem();
        }
        else
        {
            updateItem();
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
        m_end= mouseEvent->scenePos();
        m_currentItem->setModifiers(mouseEvent->modifiers());
        m_currentItem->setNewEnd(m_end);
        update();
    }
    if((m_selectedtool == VToolsBar::HANDLER) || (m_selectedtool == VToolsBar::TEXT)
       || (m_selectedtool == VToolsBar::TEXTBORDER))
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
void VMap::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if((!m_currentPath.isNull()) && ((VToolsBar::Painting == m_editionMode)))
    {
        if(VisualItem::PATH == m_currentPath->getType())
        {
            PathItem* itm= dynamic_cast<PathItem*>(m_currentPath.data());
            if(nullptr != itm)
            {
                itm->release();
            }
        }
    }
    if(!m_currentItem.isNull())
    {
        if(VisualItem::ANCHOR == m_currentItem->getType())
        {
            manageAnchor();
        }
        if((m_currentItem->getType() == VisualItem::RULE) || (m_currentItem->getType() == VisualItem::ANCHOR))
        {
            removeItem(m_currentItem);
            m_currentItem= nullptr;
            return;
        }
        if(VToolsBar::Painting == m_editionMode)
        {
            sendOffItem(m_currentItem);
        }
        else
        {
            QPolygonF* poly= new QPolygonF();
            *poly= m_currentItem->shape().toFillPolygon();
            *poly= poly->translated(m_currentItem->pos());
            m_currentFog= m_sightItem->addFogPolygon(poly, (VToolsBar::Mask == m_editionMode));
            sendItemToAll(m_sightItem);
            if(nullptr == m_currentPath)
            {
                removeItem(m_currentItem);
            }
        }
    }
    else if((nullptr != m_currentPath) && (VToolsBar::Painting != m_editionMode))
    {
        QPolygonF* poly= new QPolygonF();
        *poly= m_currentPath->shape().toFillPolygon();
        m_currentFog->setPolygon(poly);
        update();
    }
    m_currentItem= nullptr;
    if(m_selectedtool == VToolsBar::HANDLER)
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
    if(nullptr != child)
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::SetParentItem);
        msg.string8(m_id);
        VisualItem* childItem= dynamic_cast<VisualItem*>(child);

        if(nullptr != childItem)
        {
            msg.string8(childItem->getId());
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
            msg.string8(paItem->getId());
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

    if(vItem->getLayer() != m_currentLayer)
        return false;

    return true;
}

void VMap::manageAnchor()
{
    if(m_currentItem.isNull())
        return;
    AnchorItem* tmp= dynamic_cast<AnchorItem*>(m_currentItem.data());

    if(nullptr != tmp)
    {
        QGraphicsItem* child= nullptr;
        QGraphicsItem* parent= nullptr;
        QList<QGraphicsItem*> item1= items(tmp->getStart());
        for(QGraphicsItem* item : item1)
        {
            if((nullptr == child) && (isNormalItem(item)))
            {
                child= item;
            }
        }
        QList<QGraphicsItem*> item2= items(tmp->getEnd());
        for(QGraphicsItem* item : item2)
        {
            if((nullptr == parent) && (isNormalItem(item)))
            {
                parent= item;
            }
        }
        setAnchor(child, parent);
    }
}

bool VMap::editLayer(VisualItem::Layer layer)
{
    if(m_currentLayer != layer)
    {
        m_currentLayer= layer;
        emit mapChanged();
        setOption(VisualItem::MapLayer, layer);
        auto const& values= m_itemMap->values();
        for(auto& item : values)
        {
            item->updateItemFlags();
        }
        return true;
    }
    return false;
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

    if((nullptr != m_currentAddCmd) && (m_currentAddCmd->getItem() == item))
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
    }
}

void VMap::setCurrentChosenColor(QColor& p)
{
    m_itemColor= p;
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

    out << m_width;
    out << m_height;
    out << m_title;
    out << m_bgColor;
    out << m_zIndex;

    out << m_propertiesHash->count();
    auto keys= m_propertiesHash->keys();
    for(VisualItem::Properties& property : keys)
    {
        out << static_cast<int>(property);
        out << m_propertiesHash->value(property);
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
    }
}
// read
void VMap::openFile(QDataStream& in)
{
    if(m_itemMap == nullptr)
        return;

    in >> m_width;
    in >> m_height;

    setSceneRect();
    in >> m_title;
    in >> m_bgColor;
    in >> m_zIndex;

    int propertyCount;
    in >> propertyCount;

    for(int i= 0; i < propertyCount; ++i)
    {
        int pro;
        in >> pro;
        QVariant var;
        in >> var;
        setOption(static_cast<VisualItem::Properties>(pro), var);
    }
    editLayer(getOption(VisualItem::MapLayer).value<VisualItem::Layer>());

    // FOG
    in >> *m_sightItem;

    int numberOfItem;
    in >> numberOfItem;

    for(int i= 0; i < numberOfItem; ++i)
    {
        VisualItem* item;
        CharacterItem* charItem= nullptr;
        item= nullptr;
        int tmptype;
        in >> tmptype;
        VisualItem::ItemType type= static_cast<VisualItem::ItemType>(tmptype);

        switch(type)
        {
        case VisualItem::TEXT:
            item= new TextItem();
            break;
        case VisualItem::CHARACTER:
            charItem= new CharacterItem();
            item= charItem;
            break;
        case VisualItem::LINE:
            item= new LineItem();

            break;
        case VisualItem::RECT:
            item= new RectItem();
            break;
        case VisualItem::ELLISPE:
            item= new EllipsItem();

            break;
        case VisualItem::PATH:
            item= new PathItem();
            break;
        case VisualItem::SIGHT:
            // remove the old one
            m_orderedItemList.removeAll(m_sightItem);
            m_sortedItemList.removeAll(m_sightItem->getId());
            m_itemMap->remove(m_sightItem->getId());
            // add new
            item= m_sightItem;
            break;
        case VisualItem::GRID:
            // remove the old one
            m_orderedItemList.removeAll(m_gridItem);
            m_sortedItemList.removeAll(m_gridItem->getId());
            m_itemMap->remove(m_gridItem->getId());
            // add new
            item= m_gridItem;
            break;
        case VisualItem::IMAGE:
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
    update();
}

void VMap::addCharacter(Character* p, QPointF pos)
{
    CharacterItem* item= new CharacterItem(p, pos);
    if(nullptr != item)
    {
        item->setZValue(m_zIndex);
        item->initChildPointItem();
        addNewItem(new AddVmapItemCommand(item, true, this), true);
        insertCharacterInMap(item);
        sendOffItem(item);
    }
}
QColor VMap::getBackGroundColor() const
{
    return m_bgColor;
}
void VMap::computePattern()
{
    if((getOption(VisualItem::GridPattern).toInt() == VMap::NONE) || (!getOption(VisualItem::ShowGrid).toBool())
       || (getOption(VisualItem::GridAbove).toBool()))
    {
        setBackgroundBrush(m_bgColor);
    }
    else if(getOption(VisualItem::ShowGrid).toBool())
    {
        QPolygonF polygon;

        if(getOption(VisualItem::GridPattern).toInt() == VMap::HEXAGON)
        {
            qreal radius= getOption(VisualItem::GridSize).toInt() / 2;
            qreal hlimit= radius * qSin(M_PI / 3);
            qreal offset= radius - hlimit;
            QPointF A(2 * radius, radius - offset);
            QPointF B(radius * 1.5, radius - hlimit - offset);
            QPointF C(radius * 0.5, radius - hlimit - offset);
            QPointF D(0, radius - offset);
            QPointF E(radius * 0.5, radius + hlimit - offset - 1);
            QPointF F(radius * 1.5, radius + hlimit - offset - 1);

            QPointF G(2 * radius + radius, radius - offset);
            polygon << C << D << E << F << A << B << A << G;

            m_computedPattern= QImage(getOption(VisualItem::GridSize).toInt() * 1.5, 2 * hlimit,
                                      QImage::Format_RGBA8888_Premultiplied);
            m_computedPattern.fill(m_bgColor);
        }
        else if(getOption(VisualItem::GridPattern).toInt() == VMap::SQUARE)
        {
            m_computedPattern= QImage(getOption(VisualItem::GridSize).toInt(), getOption(VisualItem::GridSize).toInt(),
                                      QImage::Format_RGB32);
            m_computedPattern.fill(m_bgColor);
            int sizeP= getOption(VisualItem::GridSize).toInt();
            QPointF A(0, 0);
            QPointF B(0, sizeP - 1);
            QPointF C(sizeP - 1, sizeP - 1);
            // QPointF D(sizeP-1,0);
            polygon << A << B << C; //<< D << A;
        }
        QPainter painter(&m_computedPattern);
        QPen pen;
        pen.setColor(getOption(VisualItem::GridColor).value<QColor>());
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawPolyline(polygon);
        painter.end();
        setBackgroundBrush(QPixmap::fromImage(m_computedPattern));
    }
    m_gridItem->computePattern();
    m_gridItem->setVisible(getOption(VisualItem::ShowGrid).toBool());
}
void VMap::processAddItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        VisualItem* item= nullptr;
        VisualItem::ItemType type= static_cast<VisualItem::ItemType>(msg->uint8());
        CharacterItem* charItem= nullptr;
        switch(type)
        {
        case VisualItem::TEXT:
            item= new TextItem();
            break;
        case VisualItem::CHARACTER:
            charItem= new CharacterItem();
            item= charItem;
            break;
        case VisualItem::LINE:
            item= new LineItem();
            break;
        case VisualItem::RECT:
            item= new RectItem();
            break;
        case VisualItem::ELLISPE:
            item= new EllipsItem();
            break;
        case VisualItem::IMAGE:
            item= new ImageItem();
            break;
        case VisualItem::PATH:
            item= new PathItem();
            break;
        case VisualItem::SIGHT:
            item= m_sightItem;
            break;
        case VisualItem::GRID:
            item= m_gridItem;
            break;
        case VisualItem::HIGHLIGHTER:
            item= new HighlighterItem();
            break;
        default:
            break;
        }
        if(nullptr != item)
        {
            item->readItem(msg);
            QPointF pos= item->pos();
            qreal z= item->zValue();
            addNewItem(new AddVmapItemCommand(item, false, this), false, true);
            item->initChildPointItem();
            if(nullptr != charItem)
            {
                insertCharacterInMap(charItem);
            }
            item->setPos(pos);
            item->setZValue(z);
        }
    }
}
void VMap::processGeometryChangeItem(NetworkMessageReader* msg)
{
    QString idItem= msg->string16();
    if(m_itemMap->contains(idItem))
    {
        VisualItem* item= m_itemMap->value(idItem);
        item->readItem(msg);
    }
    else if(idItem == m_sightItem->getId())
    {
        m_sightItem->readItem(msg);
    }
}
void VMap::processMoveItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readPositionMsg(msg);
        }
    }
}
void VMap::processOpacityMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readOpacityMsg(msg);
        }
    }
}
void VMap::processLayerMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readLayerMsg(msg);
        }
    }
}
void VMap::processDelItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        removeItemFromScene(id, false);
    }
}
void VMap::processSetParentItem(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString childId= msg->string8();
        QString parentId= msg->string8();
        VisualItem* childItem= m_itemMap->value(childId);

        VisualItem* parentItem= nullptr;
        if(parentId != QStringLiteral("nullptr"))
        {
            parentItem= m_itemMap->value(parentId);
        }
        if(nullptr != childItem)
        {
            setAnchor(childItem, parentItem, false);
        }
    }
}
void VMap::processZValueMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readZValueMsg(msg);
            ensureFogAboveAll();
        }
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

QHash<VisualItem::Properties, QVariant>* VMap::getPropertiesHash() const
{
    return m_propertiesHash;
}

void VMap::setPropertiesHash(QHash<VisualItem::Properties, QVariant>* propertiesHash)
{
    m_propertiesHash= propertiesHash;
}

QUndoStack* VMap::getUndoStack() const
{
    return m_undoStack;
}

void VMap::setUndoStack(QUndoStack* undoStack)
{
    m_undoStack= undoStack;
}

QString VMap::getCurrentNpcName() const
{
    return m_currentNpcName;
}

void VMap::removeItemFromData(VisualItem* item)
{
    if(nullptr == item)
        return;

    m_sortedItemList.removeAll(item->getId());
    m_itemMap->remove(item->getId());
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

    auto id= item->getId();
    if(isItemStorable(item))
    {
        m_itemMap->insert(id, item);
        if(!m_sortedItemList.contains(id))
            m_sortedItemList.append(id);
    }
}

void VMap::insertItemFromData(VisualItem*, int) {}

int VMap::getCurrentNpcNumber() const
{
    return m_currentNpcNumber;
}

void VMap::processRotationMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readRotationMsg(msg);
        }
    }
}
void VMap::processRectGeometryMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readRectGeometryMsg(msg);
        }
    }
}

void VMap::processVisionMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString CharacterId= msg->string16();
        QString itemId= msg->string16();
        QList<CharacterItem*> itemList= m_characterItemMap->values(CharacterId);
        for(auto item : itemList)
        {
            if((nullptr != item) && (item->getId() == itemId))
            {
                item->readVisionMsg(msg);
            }
        }
    }
}
void VMap::processColorMsg(NetworkMessageReader* msg)
{
    if(msg == nullptr)
        return;

    QString id= msg->string16();
    VisualItem* item= m_itemMap->value(id);
    QColor color= QColor::fromRgb(msg->rgb());
    if(nullptr != item)
    {
        item->setPenColor(color);
    }
}

void VMap::processMovePointMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readMovePointMsg(msg);
        }
    }
}

bool VMap::isItemStorable(VisualItem* item)
{
    if((item->getType() == VisualItem::ANCHOR) || (item->getType() == VisualItem::GRID)
       || (item->getType() == VisualItem::SIGHT) || (item->getType() == VisualItem::RULE)
       || (item->getType() == VisualItem::HIGHLIGHTER))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void VMap::addNewItem(AddVmapItemCommand* itemCmd, bool undoable, bool fromNetwork)
{
    if((nullptr != itemCmd) && (!itemCmd->hasError()))
    {
        itemCmd->setUndoable(undoable);
        m_currentAddCmd= itemCmd;
        VisualItem* item= m_currentAddCmd->getItem();

        if(isItemStorable(item))
        {
            m_orderedItemList.append(item);
        }

        item->setZValue(m_zIndex);

        if((VToolsBar::Painting == m_editionMode) || (fromNetwork))
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
    }
}
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

void VMap::promoteItemInType(VisualItem* item, VisualItem::ItemType type)
{
    if(nullptr != item)
    {
        VisualItem* bis= item->promoteTo(type);
        if(bis == nullptr)
            return;
        // bis->setLayer(item->getLayer());
        removeItemFromScene(item->getId());
        addNewItem(new AddVmapItemCommand(bis, false, this), true);
        bis->initChildPointItem();
    }
}

void VMap::removeItemFromScene(QString id, bool sendToAll, bool undoable)
{
    if(m_sightItem->getId() == id || m_gridItem->getId() == id)
    {
        return;
    }
    VisualItem* item= m_itemMap->value(id);

    if(nullptr != item)
    {
        if(undoable)
        {
            m_undoStack->push(new DeleteVmapItemCommand(this, item, sendToAll));
        }
        else
        {
            DeleteVmapItemCommand cmd(this, item, sendToAll);
            cmd.redo();
        }
    }
}

void VMap::sendItemToAll(VisualItem* item)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::GeometryItemChanged);
    msg.string8(m_id);
    msg.string16(item->getId());
    item->fillMessage(&msg);
    msg.sendToServer();
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
                    idListToRemove << itemV->getId();
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
void VMap::setPermissionMode(Map::PermissionMode mode)
{
    if(getOption(VisualItem::PermissionMode).toInt() == mode)
        return;

    setOption(VisualItem::PermissionMode, mode);
    if(!getOption(VisualItem::LocalIsGM).toBool())
    {
        auto const& values= m_itemMap->values();
        for(auto& item : values)
        {
            item->updateItemFlags();
        }
    }
}
QString VMap::getPermissionModeText()
{
    QStringList permissionData;
    permissionData << tr("No Right") << tr("His character") << tr("All Permissions");
    return permissionData.at(getOption(VisualItem::PermissionMode).toInt());
}
Map::PermissionMode VMap::getPermissionMode()
{
    return static_cast<Map::PermissionMode>(getOption(VisualItem::PermissionMode).toInt());
}
void VMap::setCurrentNpcName(QString text)
{
    m_currentNpcName= text;
}
void VMap::setCurrentNpcNumber(int number)
{
    m_currentNpcNumber= number;
}
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
                if(resource->getResourcesType() == ResourcesNode::Cleveruri)
                {
                    qInfo() << "VMAP dropEvent: image from resources list";
                    auto media= dynamic_cast<CleverURI*>(resource);
                    if(media)
                    {
                        if(media->getType() == CleverURI::PICTURE)
                        {
                            ImageItem* led= new ImageItem();
                            led->setImageUri(media->getUri());
                            addNewItem(new AddVmapItemCommand(led, true, this), true);
                            led->setPos(event->scenePos());
                            sendOffItem(led);
                        }
                    }
                }
                else if(resource->getResourcesType() == ResourcesNode::Person)
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
        if(data->hasUrls())
        {
            for(QUrl& url : data->urls())
            {
                VisualItem* item= nullptr;
                if(url.isLocalFile() && url.fileName().endsWith("rtok"))
                {
                    qInfo() << "VMAP dropEvent: rtok from file";
                    CharacterItem* persona= new CharacterItem();
                    persona->setTokenFile(url.toLocalFile());
                    insertCharacterInMap(persona);
                    auto character= persona->getCharacter();
                    if(character)
                    {
                        PlayersList* list= PlayersList::instance();
                        list->addLocalCharacter(character);
                    }
                    item= persona;
                }
                else if(url.isLocalFile())
                {
                    qInfo() << "VMAP dropEvent: Image from file";
                    ImageItem* led= new ImageItem();
                    led->setImageUri(url.toLocalFile());
                    item= led;
                }
                if(nullptr != item)
                {
                    addNewItem(new AddVmapItemCommand(item, true, this), true);
                    item->setPos(event->scenePos());
                    sendOffItem(item);
                }
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
        copy->setLayer(item->getLayer());
        addNewItem(new AddVmapItemCommand(copy, false, this), true);
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
        QList<CharacterItem*> list= m_characterItemMap->values(item->getUuid());

        // Remove them from the map
        m_characterItemMap->remove(item->getUuid());
        // remove the changed characterItem
        list.removeOne(cItem);
        // add the others
        for(CharacterItem* itemC : list)
        {
            m_characterItemMap->insertMulti(itemC->getCharacterId(), itemC);
        }
        // add item with its new key
        m_characterItemMap->insertMulti(cItem->getCharacterId(), cItem);
        if((cItem->isPlayableCharacter()) && (!getOption(VisualItem::LocalIsGM).toBool()) && (cItem->isLocal()))
        {
            changeStackOrder(cItem, VisualItem::FRONT);
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
            m_sightItem->insertVision(item);
        }
        else
        {
            auto list= PlayersList::instance();
            list->addNpc(item->getCharacter());
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

bool VMap::setVisibilityMode(VMap::VisibilityMode mode)
{
    bool result= false;
    setOption(VisualItem::VisibilityMode, mode);
    bool visibilitySight= false;
    bool visibilityItems= false;
    if(!getOption(VisualItem::LocalIsGM).toBool())
    {
        if(mode == VMap::HIDDEN)
        {
            visibilityItems= false;
        }
        else if(mode == VMap::ALL)
        {
            visibilityItems= true;
        }
        else if(VMap::FOGOFWAR == mode)
        {
            visibilitySight= true;
            visibilityItems= true;
        }
    }
    else
    {
        visibilityItems= true;
        if(VMap::FOGOFWAR == mode)
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
        m_sightItem->setVisible(visibilitySight);
        m_propertiesHash->insert(VisualItem::FogOfWarStatus, visibilitySight);
        if((visibilitySight) && (m_propertiesHash->value(VisualItem::LocalIsGM).toBool() == false))
        {
            for(auto& item : values)
            {
                if(item->getType() == VisualItem::CHARACTER)
                {
                    item->setToolTip("");
                }
            }
        }
    }
    return result;
}
void VMap::hideOtherLayers(bool b)
{
    auto const& values= m_itemMap->values();
    for(auto& item : values)
    {
        if(item == m_sightItem || item == m_gridItem)
            continue;

        if((b) && (item->getLayer() != m_currentLayer))
        {
            item->setVisible(false);
        }
        else
        {
            item->setVisible(true);
        }
    }
}

bool VMap::setOption(VisualItem::Properties pop, QVariant value)
{
    if(nullptr != m_propertiesHash)
    {
        if(getOption(pop) != value)
        {
            m_propertiesHash->insert(pop, value);
            if(pop == VisualItem::HideOtherLayers)
            {
                hideOtherLayers(value.toBool());
            }
            emit mapChanged();
            computePattern();
            update();
            return true;
        }
    }
    return false;
}
QVariant VMap::getOption(VisualItem::Properties pop)
{
    if(nullptr != m_propertiesHash)
    {
        return m_propertiesHash->value(pop);
    }
    return QVariant();
}
QString VMap::getVisibilityModeText()
{
    QStringList visibilityData;
    visibilityData << tr("Hidden") << tr("Fog Of War") << tr("All visible");

    if(getOption(VisualItem::VisibilityMode).toInt() >= 0
       && getOption(VisualItem::VisibilityMode).toInt() < visibilityData.size())
    {
        return visibilityData.at(getOption(VisualItem::VisibilityMode).toInt());
    }
    return {};
}
VToolsBar::EditionMode VMap::getEditionMode()
{
    return m_editionMode;
}

SightItem* VMap::getFogItem() const
{
    return m_sightItem;
}
void VMap::changeStackOrder(VisualItem* item, VisualItem::StackOrder op)
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
}
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

void VMap::rollInit(APPLY_ON_CHARACTER zone)
{
    QList<CharacterItem*> list;
    if(zone == AllCharacter)
    {
        list= m_characterItemMap->values();
    }
    else if(zone == AllNPC)
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

void VMap::cleanUpInit(APPLY_ON_CHARACTER zone)
{
    QList<CharacterItem*> list;
    if(zone == AllCharacter)
    {
        list= m_characterItemMap->values();
    }
    else if(zone == AllNPC)
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
