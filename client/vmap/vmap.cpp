#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QKeyEvent>

#include "vmap.h"
#include "data/character.h"
#include "items/rectitem.h"
#include "items/ellipsitem.h"
#include "items/pathitem.h"
#include "items/lineitem.h"
#include "items/textitem.h"
#include "items/characteritem.h"
#include "items/ruleitem.h"
#include "items/imageitem.h"
#include "items/anchoritem.h"
#include "items/highlighteritem.h"

#include "userlist/rolisteammimedata.h"
#include "data/cleverurimimedata.h"

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

//Undo management
#include "undoCmd/addvmapitem.h"
#include "undoCmd/movevmapitem.h"
#include "undoCmd/changecoloritem.h"

VMap::VMap(QObject * parent)
    : QGraphicsScene(parent),m_currentLayer(VisualItem::GROUND)
{
    initMap();
}


VMap::VMap(int width,int height,QString& title,QColor& bgColor,QObject * parent)
    : QGraphicsScene(0,0,width,height,parent),m_currentLayer(VisualItem::GROUND)
{
    m_title = title;

    m_bgColor = bgColor;
    setBackgroundBrush(m_bgColor);
    initMap();

}
void VMap::initMap()
{
    m_editionMode = VToolsBar::Painting;
    m_propertiesHash = new QHash<VisualItem::Properties,QVariant>();
    m_zIndex =0;

    PlayersList* list = PlayersList::instance();
    connect(list,SIGNAL(characterDeleted(Character*)),this,SLOT(characterHasBeenDeleted(Character*)));

    connect(this, SIGNAL(selectionChanged()),this,SLOT(selectionHasChanged()));

    m_penSize = 1;
    m_id = QUuid::createUuid().toString();
    m_currentItem = nullptr;
    m_currentPath = nullptr;
    m_fogItem = nullptr;
    m_itemMap=new  QMap<QString,VisualItem*>;
    m_characterItemMap = new QMap<QString,CharacterItem*>();
    setItemIndexMethod(QGraphicsScene::NoIndex);

    m_sightItem = new SightItem(m_characterItemMap);
    m_gridItem = new GridItem();


    m_propertiesHash->insert(VisualItem::ShowNpcName,false);
    m_propertiesHash->insert(VisualItem::ShowPcName,false);
    m_propertiesHash->insert(VisualItem::ShowNpcNumber,false);
    m_propertiesHash->insert(VisualItem::ShowHealthStatus,false);
    m_propertiesHash->insert(VisualItem::ShowInitScore,true);
    m_propertiesHash->insert(VisualItem::ShowHealthBar,true);
    m_propertiesHash->insert(VisualItem::ShowGrid,false);
    m_propertiesHash->insert(VisualItem::LocalIsGM,false);
    m_propertiesHash->insert(VisualItem::EnableCharacterVision,false);
    m_propertiesHash->insert(VisualItem::CollisionStatus,false);
    m_propertiesHash->insert(VisualItem::PermissionMode,Map::GM_ONLY);
    m_propertiesHash->insert(VisualItem::FogOfWarStatus,false);
    m_propertiesHash->insert(VisualItem::VisibilityMode,VMap::HIDDEN);
}

VToolsBar::SelectableTool VMap::getSelectedtool() const
{
    return m_selectedtool;
}
void VMap::setCurrentTool(VToolsBar::SelectableTool selectedtool)
{
    cleanFogEdition();
    if((m_selectedtool == VToolsBar::PATH)&&(m_selectedtool != selectedtool))
    {
        m_currentPath = nullptr;
    }
    m_selectedtool = selectedtool;
    m_currentItem = nullptr;

}
VisualItem::Layer VMap::currentLayer() const
{
    return m_currentLayer;
}

void VMap::setCurrentLayer(const VisualItem::Layer &currentLayer)
{
    if(editLayer(currentLayer))
    {
        emit mapChanged();
    }

}

void  VMap::initScene()
{
    addNewItem(new AddVmapItemCommand(m_sightItem,this),false);
    m_sightItem->initChildPointItem();
    m_sightItem->setPos(0,0);
    m_sightItem->setZValue(1);
    m_sightItem->setVisible(false);


    addNewItem(new AddVmapItemCommand(m_gridItem,this),false);
    m_gridItem->initChildPointItem();
    m_gridItem->setPos(0,0);
    m_gridItem->setZValue(2);
    m_gridItem->setVisible(true);
}

void VMap::setWidth(int width)
{
    m_width = width;
    setSceneRect();
}

void VMap::setHeight(int height)
{
    m_height = height;
    setSceneRect();
}

void VMap::setTitle(QString title)
{
    if(m_title == title)
        return;

    m_title = title;
    emit mapChanged();
}

void VMap::setBackGroundColor(QColor bgcolor)
{
    if(m_bgColor == bgcolor)
        return;
    m_bgColor = bgcolor;
    computePattern();
    update();
    emit mapChanged();
}

void VMap::setSceneRect()
{
    QGraphicsScene::setSceneRect(0,0,m_width,m_height);
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
const QColor& VMap::mapColor() const
{
    return m_bgColor;
}
void VMap::setPenSize(int p)
{
    m_penSize =p;
}
void VMap::characterHasBeenDeleted(Character* character)
{
    QList<CharacterItem*> list = getCharacterOnMap(character->getUuid());
    for(CharacterItem* item : list)
    {
        removeItemFromScene(item->getId());
    }
}

void VMap::fill(NetworkMessageWriter& msg)
{
    msg.string8(getId());
    msg.string16(getMapTitle());
    msg.rgb(mapColor().rgb());
    msg.uint16(mapWidth());
    msg.uint16(mapHeight());
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
    msg.rgb(getOption(VisualItem::GridColor).value<QColor>().rgb());
    msg.uint64(static_cast<quint64>(m_itemMap->values().size()));
}
void VMap::readMessage(NetworkMessageReader& msg,bool readCharacter)
{
    m_id = msg.string8();
    m_title = msg.string16();
    m_bgColor = msg.rgb();
    setWidth(msg.uint16());
    setHeight(msg.uint16());
    VisualItem::Layer layer = static_cast<VisualItem::Layer>(msg.uint8());
    QString idSight = msg.string8();
    m_sightItem->setId(idSight);
    Map::PermissionMode permissionMode = static_cast<Map::PermissionMode>(msg.uint8());
    VMap::VisibilityMode mode = static_cast<VMap::VisibilityMode>(msg.uint8());
    m_zIndex = msg.uint64();
    quint8 enableCharacter = msg.uint8();

    //Grid
    GRID_PATTERN gridP = static_cast<GRID_PATTERN>(msg.uint8());
    bool showGrid = static_cast<bool>(msg.uint8());
    int gridSize = static_cast<int>(msg.uint32());
    bool gridAbove = static_cast<bool>(msg.uint8());
    QColor colorGrid = msg.rgb();

    quint64 itemCount = msg.uint64();

    if(readCharacter)
    {
        for(quint64 i = 0; i < itemCount; ++i)
        {
            processAddItemMessage(&msg);
        }
    }

    blockSignals(true);
    editLayer(layer);
    setPermissionMode(permissionMode);
    setOption(VisualItem::EnableCharacterVision,enableCharacter);
    setOption(VisualItem::GridPattern,gridP);
    setOption(VisualItem::GridAbove,gridAbove);
    setOption(VisualItem::GridColor,colorGrid);
    setOption(VisualItem::GridSize,gridSize);
    setOption(VisualItem::ShowGrid,showGrid);
    setVisibilityMode(mode);
    blockSignals(false);
    emit mapStatutChanged();
}

void VMap::processCharacterStateHasChanged(NetworkMessageReader& msg)
{
    QString idItem = msg.string8();
    VisualItem* item = m_itemMap->value(idItem);
    if(item->getType() == VisualItem::CHARACTER)
    {
        QString idCharacter = msg.string8();
        QList<CharacterItem*> list = getCharacterOnMap(idCharacter);
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
    QString idItem = msg.string8();
    VisualItem* item = m_itemMap->value(idItem);
    if(item->getType() == VisualItem::CHARACTER)
    {
        auto character = dynamic_cast<CharacterItem*>(item);
        if(character == nullptr)
            return;

        QString idCharacter = msg.string8();
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
    for(QString key : m_sortedItemList)
    {
        VisualItem* item = m_itemMap->value(key);
        if(nullptr!=item)
        {
            msg.uint8(item->getType());
            item->fillMessage(&msg);
        }
    }
}
void  VMap::setEditionMode(VToolsBar::EditionMode mode)
{
    m_editionMode = mode;
}
void VMap::cleanFogEdition()
{
    if(nullptr!=m_fogItem)
    {
        if(nullptr != m_fogItem->scene())
        {
            removeItem(m_fogItem);
        }
        m_fogItem = nullptr;
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
void VMap::addImageItem(QString file)
{
    ImageItem* led = new ImageItem();
    led->setImageUri(file);
    //led->initChildPointItem();
    addNewItem(new AddVmapItemCommand(led,this),true);
    sendOffItem(led);
}
void VMap::addImageItem(QImage img)
{
    ImageItem* led = new ImageItem();
    led->setImage(img);
    QPointF size(img.width(),img.height());
    led->setNewEnd(size);
    //led->initChildPointItem();
    addNewItem(new AddVmapItemCommand(led,this),true);
    sendOffItem(led);
}
void VMap::setCurrentItemOpacity(qreal a)
{
    QList<QGraphicsItem*> selection= selectedItems();
    for(auto item : selection)
    {
        VisualItem* vItem = dynamic_cast<VisualItem*>(item);
        if(nullptr!=vItem)
        {
            vItem->setOpacity(a);
        }
    }
}

void VMap::selectionHasChanged()
{
    QList<QGraphicsItem*> items = selectedItems();
    for(auto item : items)
    {
        VisualItem* vItem = dynamic_cast<VisualItem*>(item);
        if(nullptr!=vItem)
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
        QList<QGraphicsItem *> itemList = items(m_first);
        itemList.removeAll(m_gridItem);
        if(!itemList.isEmpty())
        {
            VisualItem* item = dynamic_cast<VisualItem*>(itemList.at(0));
            if(nullptr!=item)
            {
                if(item->getType() != VisualItem::IMAGE)
                {
                    QColor color = item->getColor();
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
        QList<QGraphicsItem *> itemList = items(m_first);
        itemList.removeAll(m_gridItem);
        if(!itemList.isEmpty())
        {
            VisualItem* item = dynamic_cast<VisualItem*>(itemList.at(0));
            if(nullptr!=item)
            {
                if(item->getType() != VisualItem::IMAGE)
                {
                    auto cmd = new ChangeColorItemCmd(item,m_itemColor, m_id);
                    m_undoStack->push(cmd);
                }
            }
        }
    }
    else
    {
        AddVmapItemCommand* itemCmd = new AddVmapItemCommand(m_selectedtool,this, m_first,m_itemColor,m_penSize);
        m_currentItem = itemCmd->getItem();
        m_currentPath = itemCmd->getPath();
        m_currentItem->setPropertiesHash(m_propertiesHash);

        addNewItem(itemCmd,(VToolsBar::Painting == m_editionMode));

        if(VToolsBar::Painting != m_editionMode)
        {
            m_fogItem = m_currentItem;
        }
    }
}
void VMap::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        if(mouseEvent->button() == Qt::LeftButton)
        {
            const QList<QGraphicsItem *> itemList = selectedItems();
            for(auto item : itemList)
            {
                VisualItem* vitem = dynamic_cast<VisualItem*>(item);
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
        m_first = mouseEvent->scenePos();
        m_end = m_first;
        if(m_currentPath==nullptr)
        {
            addItem();
        }
        else
        {
            updateItem();
        }
    }
    else if(mouseEvent->button()==Qt::RightButton)
    {
        m_currentPath = nullptr;
        cleanFogEdition();
    }
}
void VMap::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentItem!=nullptr)
    {
        m_end = mouseEvent->scenePos();
        m_currentItem->setModifiers(mouseEvent->modifiers());
        m_currentItem->setNewEnd( m_end);
        update();
    }
    if((m_selectedtool==VToolsBar::HANDLER)||
            (m_selectedtool==VToolsBar::TEXT)||
            (m_selectedtool==VToolsBar::TEXTBORDER))
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
void VMap::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    if((nullptr!=m_currentPath)&&((VToolsBar::Painting==m_editionMode)))
    {
        if(VisualItem::PATH == m_currentPath->getType())
        {
            PathItem* itm = dynamic_cast<PathItem*>(m_currentPath);
            if(nullptr!=itm)
            {
                itm->release();
            }
        }
    }
    if(m_currentItem!=nullptr)
    {
        if(VisualItem::ANCHOR == m_currentItem->getType())
        {
            manageAnchor();
        }
        if((m_currentItem->getType() == VisualItem::RULE )||(m_currentItem->getType() == VisualItem::ANCHOR))
        {
            removeItem(m_currentItem);
            m_currentItem = nullptr;
            return;
        }
        if(VToolsBar::Painting==m_editionMode)
        {
            sendOffItem(m_currentItem);
        }
        else
        {
            QPolygonF* poly = new QPolygonF();
            *poly = m_currentItem->shape().toFillPolygon();
            *poly = poly->translated(m_currentItem->pos());
            m_currentFog = m_sightItem->addFogPolygon(poly,(VToolsBar::Mask == m_editionMode));
            sendItemToAll(m_sightItem);
            if(nullptr==m_currentPath)
            {
                removeItem(m_currentItem);
            }
        }
    }
    else if((nullptr!=m_currentPath)&&(VToolsBar::Painting!=m_editionMode))
    {
        QPolygonF* poly = new QPolygonF();
        *poly = m_currentPath->shape().toFillPolygon();
        m_currentFog->setPolygon(poly);
        update();
    }
    m_currentItem = nullptr;
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        if(!m_movingItems.isEmpty())
        {
            if (m_movingItems.first() != nullptr && mouseEvent->button() == Qt::LeftButton)
            {
                if (m_oldPos.first() != m_movingItems.first()->pos())
                {
                    MoveItemCommand* moveCmd = new MoveItemCommand(m_movingItems,m_oldPos);
                    m_undoStack->push(moveCmd);
                }
                m_movingItems.clear();
                m_oldPos.clear();
            }
        }
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}
void VMap::setAnchor(QGraphicsItem* child,QGraphicsItem* parent,bool send)
{
    if(nullptr!=child)
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::SetParentItem);
        msg.string8(m_id);
        VisualItem* childItem = dynamic_cast<VisualItem*>(child);

        if(nullptr!=childItem)
        {
            msg.string8(childItem->getId());
        }

        QPointF pos = child->pos();
        QPointF pos2;
        if(nullptr!=parent)
        {
            pos2 =parent->mapFromScene(pos);
        }
        else
        {
            if(nullptr!=child->parentItem())
            {
                pos2 = child->parentItem()->mapToScene(pos);
            }
        }
        VisualItem* paItem = dynamic_cast<VisualItem*>(parent);
        if(nullptr!=paItem)
        {
            msg.string8(paItem->getId());
        }
        else
        {
            msg.string8("nullptr");
        }
        bool hasMoved=false;
        if(nullptr==parent)
        {
            if(send)
            {
                msg.sendToServer();
            }
            child->setParentItem(parent);
            hasMoved = true;
        }
        else if(parent->parentItem()!=child)
        {
            if(send)
            {
                msg.sendToServer();
            }
            child->setParentItem(parent);
            hasMoved = true;
        }
        if(!(pos2.isNull() && parent == nullptr )&& hasMoved)
        {
            child->setPos(pos2);
        }
    }
}
bool VMap::isNormalItem(QGraphicsItem* item)
{
    if((item == m_gridItem)||(item == m_fogItem))
    {
        return false;
    }
    return true;
}
void VMap::manageAnchor()
{
    AnchorItem* tmp = dynamic_cast< AnchorItem*>(m_currentItem);

    if(nullptr!=tmp)
    {
        QGraphicsItem* child = nullptr;
        QGraphicsItem* parent = nullptr;
        QList<QGraphicsItem*> item1 = items(tmp->getStart());
        for (QGraphicsItem* item: item1)
        {
            if((nullptr==child)&&(isNormalItem(item))&&(item!=m_sightItem))
            {
                child = item;
            }
        }
        QList<QGraphicsItem*> item2 = items(tmp->getEnd());
        for (QGraphicsItem* item: item2)
        {
            if((nullptr==parent)&&(isNormalItem(item))&&(item!=m_sightItem))
            {
                parent = item;
            }
        }
        setAnchor(child,parent);
    }
}


bool VMap::editLayer(VisualItem::Layer layer)
{
    if(m_currentLayer!=layer)
    {
        m_currentLayer = layer;
        emit mapChanged();
        for(VisualItem* item: m_itemMap->values())
        {
            if(m_currentLayer == item->getLayer())
            {
                item->setEditableItem(true);
            }
            else
            {
                item->setEditableItem(false);
            }
        }
        return true;
    }
    return false;
}
void VMap::checkItemLayer(VisualItem* item)
{
    if(item->getLayer() == m_currentLayer)
    {
        item->setEditableItem(true);
    }
    else
    {
        item->setEditableItem(false);
    }
}

void VMap::sendOffItem(VisualItem* item, bool doInitPoint)
{
    if(nullptr == m_undoStack)
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
            delete m_currentAddCmd;
        }
    }
    m_currentAddCmd = nullptr;
}

void VMap::setCurrentChosenColor(QColor& p)
{
    m_itemColor = p;
}
QString VMap::getId() const
{
    return m_id;
}
void VMap::setId(QString id)
{
    m_id = id;
}
//write
QDataStream& operator<<(QDataStream& out, const VMap& con)
{
    out << con.m_width;
    out << con.m_height;
    out << con.m_title;
    out << con.m_bgColor;
    out << con.m_zIndex;
    
    out << con.m_itemMap->size();
    for(int i = 0; i< con.m_itemMap->size();i++)
    {
        VisualItem* item = con.m_itemMap->values().at(i);
        out << *item ;
    }
    return out;
}
//read
/// @todo not symetric with operator<<
QDataStream& operator>>(QDataStream& is,VMap& con)
{
    is >>(con.m_width);
    is >>(con.m_height);
    is >>(con.m_title);
    is >>(con.m_bgColor);
    is >> (con.m_zIndex);

    int size;
    is >> size;
    
    return is;
}
//write
void VMap::saveFile(QDataStream& out)
{
    if(m_itemMap->isEmpty())
        return;
    
    out << m_width;
    out<< m_height;
    out<< m_title;
    out<< m_bgColor;
    out << m_zIndex;




    out << m_propertiesHash->count();
    for(VisualItem::Properties property: m_propertiesHash->keys())
    {
        out << (int)property;
        out << m_propertiesHash->value(property);
    }


    out << m_itemMap->size();
    for(QString key: m_sortedItemList)//m_itemMap->values()
    {
        VisualItem* tmp = m_itemMap->value(key);
        if(nullptr!=tmp)
        {
            out << (int)tmp->getType() << *tmp << tmp->pos().x() << tmp->pos().y();
        }
    }
}
//read
void VMap::openFile(QDataStream& in)
{
    if(m_itemMap!=nullptr)
    {
        in >> m_width;
        in >> m_height;

        setSceneRect();
        in>> m_title;
        in>> m_bgColor;
        in>> m_zIndex;

        int propertyCount;
        in >> propertyCount;

        for(int i = 0; i < propertyCount;++i)
        {
            int pro;
            in >> pro;
            QVariant var;
            in >> var;
            m_propertiesHash->insert((VisualItem::Properties)pro,var);
        }

        int numberOfItem;
        in >> numberOfItem;

        for(int i =0 ; i<numberOfItem;++i)
        {
            VisualItem* item;
            CharacterItem* charItem = nullptr;
            item=nullptr;
            VisualItem::ItemType type;
            int tmptype;
            in >> tmptype;
            type=(VisualItem::ItemType)tmptype;

            switch(type)
            {
            case VisualItem::TEXT:
                item=new TextItem();
                break;
            case VisualItem::CHARACTER:
                charItem=new CharacterItem();
                item = charItem;
                break;
            case VisualItem::LINE:
                item=new LineItem();

                break;
            case VisualItem::RECT:
                item=new RectItem();
                break;
            case VisualItem::ELLISPE:
                item=new EllipsItem();

                break;
            case VisualItem::PATH:
                item=new PathItem();
                break;
            case VisualItem::SIGHT:
                //remove the old one
                m_orderedItemList.removeAll(m_sightItem);
                m_sortedItemList.removeAll(m_sightItem->getId());
                m_itemMap->remove(m_sightItem->getId());
                //add new
                item= m_sightItem;
                break;
            case VisualItem::GRID:
                //remove the old one
                m_orderedItemList.removeAll(m_gridItem);
                m_sortedItemList.removeAll(m_gridItem->getId());
                m_itemMap->remove(m_gridItem->getId());
                //add new
                item= m_gridItem;
                break;
            case VisualItem::IMAGE:
                item= new ImageItem();
                break;
            default:
                break;

            }
            if(nullptr!=item)
            {
                in >> *item;

                qreal x,y;
                in >> x;
                in >> y;
                addNewItem(new AddVmapItemCommand(item,this),false);
                item->setPos(x,y);
                item->initChildPointItem();
                if(nullptr!=charItem)
                {
                    insertCharacterInMap(charItem);
                }
            }
        }
        ensureFogAboveAll();
    }
}

void VMap::addCharacter(Character* p, QPointF pos)
{
    CharacterItem* item= new CharacterItem(p,pos);
    if(nullptr!=item)
    {
        item->setZValue(m_zIndex);
        item->initChildPointItem();
        addNewItem(new AddVmapItemCommand(item,this),true);
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
    if((getOption(VisualItem::GridPattern).toInt() == VMap::NONE)||(!getOption(VisualItem::ShowGrid).toBool())||(getOption(VisualItem::GridAbove).toBool()))
    {
        setBackgroundBrush(m_bgColor);
    }
    else if(getOption(VisualItem::ShowGrid).toBool())
    {

        QPolygonF polygon;

        if(getOption(VisualItem::GridPattern).toInt()==VMap::HEXAGON)
        {
            qreal radius = getOption(VisualItem::GridSize).toInt()/2;
            qreal hlimit = radius * qSin(M_PI/3);
            qreal offset = radius-hlimit;
            QPointF A(2*radius,radius-offset);
            QPointF B(radius*1.5,radius-hlimit-offset);
            QPointF C(radius*0.5,radius-hlimit-offset);
            QPointF D(0,radius-offset);
            QPointF E(radius*0.5,radius+hlimit-offset-1);
            QPointF F(radius*1.5,radius+hlimit-offset-1);

            QPointF G(2*radius+radius,radius-offset);
            polygon << C << D << E << F << A << B << A << G;

            m_computedPattern = QImage(getOption(VisualItem::GridSize).toInt()*1.5,2*hlimit,QImage::Format_RGBA8888_Premultiplied);
            m_computedPattern.fill(m_bgColor);
        }
        else if(getOption(VisualItem::GridPattern).toInt() == VMap::SQUARE)
        {
            m_computedPattern = QImage(getOption(VisualItem::GridSize).toInt(),getOption(VisualItem::GridSize).toInt(),QImage::Format_RGB32);
            m_computedPattern.fill(m_bgColor);
            int sizeP = getOption(VisualItem::GridSize).toInt();
            QPointF A(0,0);
            QPointF B(0,sizeP-1);
            QPointF C(sizeP-1,sizeP-1);
            //QPointF D(sizeP-1,0);
            polygon << A << B << C ;//<< D << A;
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
}
void VMap::processAddItemMessage(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        VisualItem* item=nullptr;
        VisualItem::ItemType type = static_cast<VisualItem::ItemType>(msg->uint8());
        CharacterItem* charItem = nullptr;
        switch(type)
        {
        case VisualItem::TEXT:
            item=new TextItem();
            break;
        case VisualItem::CHARACTER:
            charItem=new CharacterItem();
            item = charItem;
            break;
        case VisualItem::LINE:
            item=new LineItem();
            break;
        case VisualItem::RECT:
            item=new RectItem();
            break;
        case VisualItem::ELLISPE:
            item=new EllipsItem();
            break;
        case VisualItem::IMAGE:
            item=new ImageItem();
            break;
        case VisualItem::PATH:
            item=new PathItem();
            break;
        case VisualItem::SIGHT:
            item=m_sightItem;
            break;
        case VisualItem::GRID:
            item=m_gridItem;
            break;
        case VisualItem::HIGHLIGHTER:
            item=new HighlighterItem();
            break;
        default:
            break;
        }
        if(nullptr!=item)
        {
            item->readItem(msg);
            QPointF pos = item->pos();
            qreal z = item->zValue();
            addNewItem(new AddVmapItemCommand(item,this),false,true);
            item->initChildPointItem();
            if(nullptr!=charItem)
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
    QString idItem = msg->string16();
    if(m_itemMap->contains(idItem))
    {
        VisualItem* item = m_itemMap->value(idItem);
        item->readItem(msg);

    }
    else if(idItem == m_sightItem->getId())
    {
        m_sightItem->readItem(msg);
    }
}
void VMap::processMoveItemMessage(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(nullptr!=item)
        {
            item->readPositionMsg(msg);
        }

    }
}
void VMap::processOpacityMessage(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(nullptr!=item)
        {
            item->readOpacityMsg(msg);
        }
    }
}
void VMap::processLayerMessage(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(nullptr!=item)
        {
            item->readLayerMsg(msg);
        }
    }
}
void VMap::processDelItemMessage(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        removeItemFromScene(id,false);
    }
}
void VMap::processSetParentItem(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QString childId = msg->string8();
        QString parentId = msg->string8();
        VisualItem* childItem = m_itemMap->value(childId);

        VisualItem* parentItem = nullptr;
        if(parentId!=QStringLiteral("nullptr"))
        {
            parentItem = m_itemMap->value(parentId);
        }
        if(nullptr!=childItem)
        {
            setAnchor(childItem,parentItem,false);
        }
    }
}
void VMap::processZValueMsg(NetworkMessageReader* msg)
{                                                                                                    
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(nullptr!=item)
        {
            item->readZValueMsg(msg);
            ensureFogAboveAll();
        }
    }
}
void VMap::ensureFogAboveAll()
{
    QList<VisualItem*> list = m_itemMap->values();
    std::sort(list.begin(),list.end(),[](const VisualItem* item, const VisualItem* meti){
        return meti->zValue()>item->zValue();
    });
    m_orderedItemList = list;
    VisualItem* highest = nullptr;
    for(auto item : m_orderedItemList)
    {
        if(item!=m_sightItem)
        {
            highest = item;
        }
    }
    if(nullptr!=highest)
    {
        int z = highest->zValue();
        m_sightItem->setZValue(z+1);
        m_gridItem->setZValue(z+2);
    }
}

QHash<VisualItem::Properties, QVariant>* VMap::getPropertiesHash() const
{
    return m_propertiesHash;
}

void VMap::setPropertiesHash(QHash<VisualItem::Properties, QVariant> *propertiesHash)
{
    m_propertiesHash = propertiesHash;
}

QUndoStack* VMap::getUndoStack() const
{
    return m_undoStack;
}

void VMap::setUndoStack(QUndoStack* undoStack)
{
    m_undoStack = undoStack;
}

QString VMap::getCurrentNpcName() const
{
    return m_currentNpcName;
}

void VMap::removeItemFromData(VisualItem *item)
{
    m_sortedItemList.removeAll(item->getId());
    m_itemMap->remove(item->getId());
    m_characterItemMap->remove(item->getId());
}

void VMap::addItemFromData(VisualItem *item)
{
    m_itemMap->insert(item->getId(),item);
    m_sortedItemList.append(item->getId());
}

void VMap::insertItemFromData(VisualItem *, int )
{

}

int VMap::getCurrentNpcNumber() const
{
    return m_currentNpcNumber;
}

void VMap::processRotationMsg(NetworkMessageReader* msg)
{                                                                                                             
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(nullptr!=item)
        {
            item->readRotationMsg(msg);
        }
    }
}                                                                                                             
void VMap::processRectGeometryMsg(NetworkMessageReader* msg)
{                                                                                                             
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(nullptr!=item)
        {
            item->readRectGeometryMsg(msg);
        }
    }
}

void VMap::processVisionMsg(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QString CharacterId = msg->string16();
        QString itemId = msg->string16();
        QList<CharacterItem*> itemList = m_characterItemMap->values(CharacterId);
        for(auto item: itemList)
        {
            if((nullptr!=item)&&(item->getId() == itemId))
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

    QString id = msg->string16();
    VisualItem* item = m_itemMap->value(id);
    QColor color = QColor::fromRgb(msg->rgb());
    if(nullptr!=item)
    {
        item->setPenColor(color);
    }

}

void VMap::processMovePointMsg(NetworkMessageReader* msg)
{
    if(nullptr!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(nullptr!=item)
        {
            item->readMovePointMsg(msg);
        }
    }
}
void VMap::addNewItem(AddVmapItemCommand* itemCmd,bool undoable, bool fromNetwork)
{
    if((nullptr!=itemCmd)&&(!itemCmd->hasError()))
    {
        m_currentAddCmd = itemCmd;
        VisualItem* item = m_currentAddCmd->getItem();

        if((item!=m_sightItem)&&(item!=m_gridItem)&&(VisualItem::ANCHOR!=item->type())&&(VisualItem::RULE!=item->type()))
        {
            m_orderedItemList.append(item);
        }

        item->setZValue(m_zIndex);

        if((VToolsBar::Painting == m_editionMode)||(fromNetwork))
        {
            if((item->type() != VisualItem::ANCHOR)&&(item->type() != VisualItem::GRID))
            {
                m_itemMap->insert(item->getId(),item);
                m_sortedItemList << item->getId();
            }
        }

        if(itemCmd->isNpc())
        {
            emit npcAdded();
        }
        itemCmd->setUndoable(undoable);

    }
}
QList<CharacterItem*> VMap::getCharacterOnMap(QString id)
{
    QList<CharacterItem*> result;
    for(CharacterItem* item: m_characterItemMap->values())
    {
        if(nullptr!=item)
        {
            if(item->getCharacterId()== id)
            {
                result.append(item);
            }
        }
    }
    return result;
}

void VMap::promoteItemInType(VisualItem* item, VisualItem::ItemType type)
{
    if(nullptr!=item)
    {
        VisualItem* bis = item->promoteTo(type);
        removeItemFromScene(item->getId());
        addNewItem(new AddVmapItemCommand(bis,this),true);
        bis->initChildPointItem();
        //sendOffItem(bis);
    }
}
#include "undoCmd/deletevmapitem.h"
void VMap::removeItemFromScene(QString id,bool sendToAll)
{
    if(m_sightItem->getId()==id || m_gridItem->getId() == id)
    {
        return;
    }
    VisualItem* item = m_itemMap->value(id);

    if(nullptr!=item)
    {
        DeleteVmapItemCommand* cmd = new DeleteVmapItemCommand(this,item,sendToAll);
        m_undoStack->push(cmd);
    }
}

void VMap::sendItemToAll(VisualItem* item)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::GeometryItemChanged);
    msg.string8(m_id);
    msg.string16(item->getId());
    item->fillMessage(&msg);
    msg.sendToServer();
}
void VMap::keyPressEvent(QKeyEvent* event)
{
    if(event->key ()==Qt::Key_Delete)
    {
        QStringList idListToRemove;
        for(QGraphicsItem* item : selectedItems())
        {
            VisualItem* itemV = dynamic_cast<VisualItem*>(item);
            if(nullptr!=itemV)
            {
                if(itemV->getType() == VisualItem::CHARACTER)
                {
                    CharacterItem* itemC = dynamic_cast<CharacterItem*>(itemV);

                    if(((getOption(VisualItem::LocalIsGM).toBool())||getOption(VisualItem::PermissionMode).toInt()!=Map::PC_ALL)||
                            (itemC->isLocal()&&(getOption(VisualItem::PermissionMode).toInt()!=Map::PC_MOVE)))
                    {
                        idListToRemove << itemV->getId();
                    }
                }
                else if(itemV->isEditable())
                {
                    if((getOption(VisualItem::LocalIsGM).toBool()) || ((!getOption(VisualItem::LocalIsGM).toBool())
                        &&(getOption(VisualItem::PermissionMode).toInt()!=Map::PC_ALL)))
                    {
                        idListToRemove << itemV->getId();
                    }
                }
            }
        }
        for(auto id : idListToRemove)
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
    if(getOption(VisualItem::PermissionMode).toInt()!=mode)
    {
        setOption(VisualItem::PermissionMode,mode);
        if(!getOption(VisualItem::LocalIsGM).toBool())
        {
            if(Map::PC_MOVE == mode)
            {
                for(VisualItem* item:m_itemMap->values())
                {
                    item->setEditableItem(false);
                }
                for(CharacterItem* item:m_characterItemMap->values())
                {
                    item->setEditableItem(false);
                    item->setCharacterIsMovable(true);
                    //item->setEditableItem(false);
                }
            }
            else
            {
                bool value = false;//GM_ONLY
                if(Map::PC_ALL == mode)
                {
                    value = true;
                }
                for(VisualItem* item: m_itemMap->values())
                {
                    item->setEditableItem(value);
                }
            }
        }

    }
}
QString VMap::getPermissionModeText()
{
    QStringList permissionData;
    permissionData<< tr("No Right") << tr("His character") << tr("All Permissions");
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
void VMap::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->acceptProposedAction();
}
void VMap::setLocalId(QString id)
{
    m_localUserId = id;
}
const QString& VMap::getLocalUserId() const
{
    return m_localUserId;
}

#include "data/cleveruri.h"
void VMap::dropEvent ( QGraphicsSceneDragDropEvent * event )
{ 
    auto data = event->mimeData();
    if (data->hasFormat("rolisteam/userlist-item"))
    {
        const RolisteamMimeData* rolisteamData = qobject_cast<const RolisteamMimeData*>(data);
        Person* item = rolisteamData->getData();
        Character* character = dynamic_cast<Character*>(item);
        if(character)
        {
            addCharacter(character,event->scenePos());
        }
    }
    else if(data->hasFormat("rolisteam/cleverurilist"))
    {
        const  CleverUriMimeData* resourcesData = qobject_cast<const CleverUriMimeData*>(data);
        if(nullptr!=resourcesData)
        {
            QList<ResourcesNode*> resourcesList = resourcesData->getList().values();
            for(ResourcesNode* resource : resourcesList)
            {
                if(resource->getResourcesType() == ResourcesNode::Cleveruri)
                {
                    auto media = dynamic_cast<CleverURI*>(resource);
                    if(media->getType() == CleverURI::PICTURE)
                    {
                        ImageItem* led = new ImageItem();
                        led->setImageUri(media->getUri());
                        addNewItem(new AddVmapItemCommand(led,this),true);
                        led->setPos(event->scenePos());
                        sendOffItem(led);
                    }
                }
                else if(resource->getResourcesType() == ResourcesNode::Person)
                {
                    Person* item = dynamic_cast<Character*>(resource);
                    Character* character = dynamic_cast<Character*>(item);
                    if(character)
                    {
                        addCharacter(character,event->scenePos());
                    }
                }
            }
        }
    }
    else
    {
        if(data->hasUrls())
        {
            for(QUrl url: data->urls())
            {
                VisualItem* item = nullptr;
                if(url.isLocalFile() && url.fileName().endsWith("rtok"))
                {
                    CharacterItem* persona = new CharacterItem();
                    persona->setTokenFile(url.toLocalFile());
                    insertCharacterInMap(persona);
                    item=persona;
                }
                else if(url.isLocalFile())
                {
                    ImageItem* led = new ImageItem();
                    led->setImageUri(url.toLocalFile());
                    item=led;
                }
                if(nullptr != item)
                {
                    addNewItem(new AddVmapItemCommand(item,this),true);
                    item->setPos(event->scenePos());
                    sendOffItem(item);
                }
            }
        }
    }
}
void VMap::dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
{
    const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(nullptr!=data)
    {
        if (data->hasFormat("rolisteam/userlist-item"))
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        const QMimeData* mimeData =  event->mimeData();
        if(mimeData->hasUrls())
        {
            event->acceptProposedAction();
        }
    }
}
void VMap::duplicateItem(VisualItem* item)
{
    VisualItem* copy = item->getItemCopy();
    if(nullptr!=copy)
    {
        copy->initChildPointItem();
        addNewItem(new AddVmapItemCommand(copy,this),true);
        copy->setPos(item->pos());
        clearSelection();
        item->clearFocus();
        copy->clearFocus();
        clearFocus();
        setFocusItem(copy);
        update();
        sendOffItem(copy,false);
    }
}
bool VMap::isIdle() const
{
    return (m_currentItem==nullptr);
}
void VMap::ownerHasChangedForCharacterItem(Character* item,CharacterItem* cItem)
{
    if(nullptr!=item)
    {
        //Get all item with the key
        QList<CharacterItem*> list = m_characterItemMap->values(item->getUuid());

        //Remove them from the map
        m_characterItemMap->remove(item->getUuid());
        //remove the changed characterItem
        list.removeOne(cItem);
        //add the others
        for(CharacterItem* itemC : list)
        {
            m_characterItemMap->insertMulti(itemC->getCharacterId(),itemC);
        }
        //add item with its new key
        m_characterItemMap->insertMulti(cItem->getCharacterId(),cItem);
        if((cItem->isPlayableCharacter())&&(!getOption(VisualItem::LocalIsGM).toBool())&&(cItem->isLocal()))
        {
            changeStackOrder(cItem,VisualItem::FRONT);
        }
    }
}

void VMap::insertCharacterInMap(CharacterItem* item)
{
    if((nullptr!=m_characterItemMap)&&(nullptr!=item))
    {
        m_characterItemMap->insertMulti(item->getCharacterId(),item);
        connect(item,&CharacterItem::ownerChanged,this,&VMap::ownerHasChangedForCharacterItem);
        connect(item,&CharacterItem::runDiceCommand,this, &VMap::runDiceCommandForCharacter);
        if(!item->isNpc())
        {
            m_sightItem->insertVision(item);
        }
        else
        {
            auto list = PlayersList::instance();
            list->addNpc(item->getCharacter());
            auto search = item->getName();
            auto items = m_characterItemMap->values();
            items.removeAll(item);
            QList<CharacterItem*> sameNameItems;
            std::copy_if(items.begin(), items.end(), std::back_inserter(sameNameItems),[search](CharacterItem* item){
                return item->getName() == search;
            });
            auto it = std::max_element(sameNameItems.begin(), sameNameItems.end(),[](const CharacterItem* a, const CharacterItem* b){
                                 return a->getNumber() < b->getNumber();
                             });
            if(it != sameNameItems.end())
            {
                int max = (*it)->getNumber();
                item->setNumber(++max);
            }
        }
    }
}

bool VMap::setVisibilityMode(VMap::VisibilityMode mode)
{
    bool result = false;
    setOption(VisualItem::VisibilityMode,mode);
    bool visibilitySight = false;
    bool visibilityItems = false;
    if(!getOption(VisualItem::LocalIsGM).toBool())
    {
        if(mode == VMap::HIDDEN)
        {
            visibilityItems = false;
        }
        else if(mode == VMap::ALL)
        {
            visibilityItems = true;
        }
        else if(VMap::FOGOFWAR == mode)
        {
            visibilitySight = true;
            visibilityItems = true;
        }
    }
    else
    {
        visibilityItems = true;
        if(VMap::FOGOFWAR == mode)
        {
            visibilitySight = true;
        }
        else
        {
            visibilitySight = false;
        }
    }
    for(VisualItem* item: m_itemMap->values())
    {
        item->setVisible(visibilityItems);
    }
    if(nullptr!=m_sightItem)
    {
        m_sightItem->setVisible(visibilitySight);
        m_propertiesHash->insert(VisualItem::FogOfWarStatus,visibilitySight);
        if((visibilitySight)&&(m_propertiesHash->value(VisualItem::LocalIsGM).toBool() == false))
        {
            for(auto item : m_itemMap->values())
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
    for( auto item : m_itemMap->values())
    {
        if(item == m_sightItem || item == m_gridItem)
            continue;

        if((b)&&(item->getLayer() != m_currentLayer))
        {
            item->setVisible(false);
        }
        else
        {
            item->setVisible(true);
        }
    }
}

bool VMap::setOption(VisualItem::Properties pop,QVariant value)
{
    if(nullptr!=m_propertiesHash)
    {
        if(getOption(pop)!=value)
        {
            m_propertiesHash->insert(pop,value);
            if(pop==VisualItem::HideOtherLayers)
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
    if(nullptr!=m_propertiesHash)
    {
        return m_propertiesHash->value(pop);
    }
    return QVariant();
}
QString VMap::getVisibilityModeText()
{
    QStringList visibilityData;
    visibilityData << tr("Hidden") << tr("Fog Of War") << tr("All visible");

    if(getOption(VisualItem::VisibilityMode).toInt()>=0 && getOption(VisualItem::VisibilityMode).toInt() < visibilityData.size())
    {
        return visibilityData.at(getOption(VisualItem::VisibilityMode).toInt());
    }
    return {};
}
VToolsBar::EditionMode  VMap::getEditionMode()
{
    return m_editionMode;
}

SightItem* VMap::getFogItem() const
{
    return m_sightItem;
}
void VMap::changeStackOrder(VisualItem* item,VisualItem::StackOrder op)
{
    if (nullptr == item || m_sortedItemList.size() < 2)
        return;
    int size = m_sortedItemList.size();
    int index = m_sortedItemList.indexOf(item->getId());

    // find out insertion indexes over the stacked items
    QList<QGraphicsItem *> stackedItems = items(item->sceneBoundingRect(), Qt::IntersectsItemShape);

    int prevIndex = 0;
    int nextIndex = size - 1;
    for (QGraphicsItem * qitem: stackedItems)
    {
        // operate only on different Content and not on sightItem.
        VisualItem* c = dynamic_cast<VisualItem*>(qitem);
        if (!c || c == item || c == m_sightItem || c == m_gridItem)
            continue;

        // refine previous/next indexes (close to 'index')
        int cIdx = m_sortedItemList.indexOf(c->getId());
        if (cIdx < nextIndex && cIdx > index)
            nextIndex = cIdx;
        else if (cIdx > prevIndex && cIdx < index)
            prevIndex = cIdx;
    }


    if(index < 0)
    {
        return;
    }

    // move items
    switch (op)
    {
    case VisualItem::FRONT: // front
        m_sortedItemList.append(m_sortedItemList.takeAt(index));
        break;
    case VisualItem::RAISE: // raise
        if (index >= size - 1)
            return;

        m_sortedItemList.insert(nextIndex, m_sortedItemList.takeAt(index));
        break;
    case VisualItem::LOWER: // lower
        if (index <= 0)
            return;
        m_sortedItemList.insert(prevIndex, m_sortedItemList.takeAt(index));
        break;
    case VisualItem::BACK: // back
        m_sortedItemList.prepend(m_sortedItemList.takeAt(index));
        break;
    }

    // reassign z-levels
    m_sortedItemList.removeAll(m_sightItem->getId());
    m_sortedItemList.removeAll(m_gridItem->getId());
    m_sortedItemList.append(m_sightItem->getId());
    m_sortedItemList.append(m_gridItem->getId());

    quint64 z =0;
    for(QString key : m_sortedItemList)
    {
        VisualItem* item = m_itemMap->value(key);
        if(nullptr!=item)
        {
            if(item==m_sightItem)
            {//z index is just one level before the level of fog.
                m_zIndex = z;
            }
            item->setZValue(++z);
        }
    }
}
void VMap::showTransparentItems()
{
    for(auto item : m_itemMap->values())
    {
        if(item->opacity() == 0)
        {
            item->setOpacity(1);
        }
    }
}

QRectF VMap::itemsBoundingRectWithoutSight()
{
    QRectF result;
    for(auto item : m_itemMap->values())
    {
        if(item != m_sightItem && item != m_gridItem)
        {
            if(result.isNull())
            {
                result = item->boundingRect();
            }
            result = result.united(item->boundingRect());
        }
    }
    return result;
}

void VMap::rollInit(APPLY_ON_CHARACTER zone)
{
    QList<CharacterItem*> list;
    if(zone == AllCharacter)
    {
        list = m_characterItemMap->values();
    }
    else if(zone == AllNPC)
    {
        auto all = m_characterItemMap->values();
        for(auto i : all)
        {
            if(i->isNpc())
            {
                list.append(i);
            }
        }
    }
    else
    {
        auto selection = selectedItems();
        for(auto sel : selection)
        {
            auto item = dynamic_cast<CharacterItem*>(sel);
            if(nullptr != item)
            {
                list.append(item);
            }
        }
    }

    for(auto charac : list)
    {
        charac->runInit();
    }
}

void VMap::cleanUpInit(APPLY_ON_CHARACTER zone)
{
    QList<CharacterItem*> list;
    if(zone == AllCharacter)
    {
        list = m_characterItemMap->values();
    }
    else if(zone == AllNPC)
    {
        auto all = m_characterItemMap->values();
        for(auto i : all)
        {
            if(i->isNpc())
            {
                list.append(i);
            }
        }
    }
    else
    {
        auto selection = selectedItems();
        for(auto sel : selection)
        {
            auto item = dynamic_cast<CharacterItem*>(sel);
            if(nullptr != item)
            {
                list.append(item);
            }
        }
    }

    for(auto charac : list)
    {
        charac->cleanInit();
    }
}
