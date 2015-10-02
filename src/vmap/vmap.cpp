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

#include "userlist/rolisteammimedata.h"

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

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
    m_penSize = 1;
    m_id = QUuid::createUuid().toString();
    m_currentItem = NULL;
    m_currentPath = NULL;
    m_itemMap=new  QMap<QString,VisualItem*>;
    m_characterItemMap = new QMap<QString,VisualItem*>();
    setItemIndexMethod(QGraphicsScene::NoIndex);
    m_sightItem = new SightItem(m_characterItemMap);
}
void  VMap::initScene()
{
    addNewItem(m_sightItem);
    //m_sightItem->setParentItem(this);
    m_sightItem->initChildPointItem();
    m_sightItem->setPos(0,0);
    m_sightItem->setZValue(1);
    m_sightItem->setVisible(false);
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
    m_title = title;
}

void VMap::setBackGroundColor(QColor bgcolor)
{
    m_bgColor = bgcolor;
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
const QString& VMap::mapTitle() const
{
    return m_title;
}
const QColor& VMap::mapColor() const
{
    return m_bgColor;
}

void VMap::setCurrentTool(VToolsBar::SelectableTool selectedtool)
{
    if((m_selectedtool == VToolsBar::PATH)&&(m_selectedtool != selectedtool))
    {
        m_currentPath = NULL;
    }
    m_selectedtool = selectedtool;
    m_currentItem = NULL;

}
void VMap::updateItem()
{
    switch(m_selectedtool)
    {
    case VToolsBar::PATH:
    {
        m_currentPath->setNewEnd(m_first);
    }
        break;
    }
}

void VMap::addItem()
{
    //QGraphicsItem* item = NULL;
    
    switch(m_selectedtool)
    {
    case VToolsBar::PEN:
        m_currentItem=new PathItem(m_first,m_itemColor,m_penSize,true);
        break;
    case VToolsBar::LINE:
        m_currentItem= new LineItem(m_first,m_itemColor,m_penSize);
        break;
    case VToolsBar::EMPTYRECT:
        m_currentItem = new RectItem(m_first,m_end,false,m_penSize,m_itemColor);
        break;
    case VToolsBar::FILLRECT:
        m_currentItem = new RectItem(m_first,m_end,true,m_penSize,m_itemColor);
        break;
    case VToolsBar::EMPTYELLIPSE:
        m_currentItem = new EllipsItem(m_first,false,m_penSize,m_itemColor);
        break;
    case VToolsBar::FILLEDELLIPSE:
        m_currentItem = new EllipsItem(m_first,true,m_penSize,m_itemColor);
        break;
    case VToolsBar::TEXT:
    {
        QLineEdit* tempedit = new QLineEdit();
        TextItem* temptext = new TextItem(m_first,tempedit,m_itemColor);
        m_currentItem = temptext;
        QGraphicsProxyWidget * tmp = QGraphicsScene::addWidget(tempedit);
        //tmp->setPos(m_first.x(),m_first.y()-tempedit->height());

        tmp->setParentItem(temptext);
        tmp->setPos(0,0);
        tmp->setFlag(QGraphicsItem::ItemIgnoresParentOpacity,true);
        tempedit->setEnabled(true);
        tempedit->setFocus();
        connect(tempedit,SIGNAL(editingFinished()),temptext,SLOT(editingFinished()));
        connect(tempedit,SIGNAL(editingFinished()),this,SLOT(update()));
    }
        break;
    case VToolsBar::HANDLER:

        break;
    case VToolsBar::ADDNPC:
    {
        CharacterItem* itemCharar= new CharacterItem(new Character(m_currentNpcName,m_itemColor,true,m_currentNpcNumber),m_first);
        m_currentItem = itemCharar;
        itemCharar->showNpcName(m_showNpcName);
        itemCharar->showNpcNumber(m_showNpcNumber);
        itemCharar->showPcName(m_showPcName);
        insertCharacterInMap(itemCharar);
        emit npcAdded();
        connect(this,SIGNAL(showNpcName(bool)),itemCharar,SLOT(showNpcName(bool)));
        connect(this,SIGNAL(showNpcNumber(bool)),itemCharar,SLOT(showNpcNumber(bool)));
        connect(this,SIGNAL(showPcName(bool)),itemCharar,SLOT(showPcName(bool)));
    }
        break;
    case VToolsBar::RULE:
    {
        RuleItem* itemRule = new RuleItem(m_first);
        itemRule->setUnit(m_patternUnit);
        itemRule->setPixelToUnit(m_sizePattern/m_patternScale);
        m_currentItem = itemRule;
    }
        break;
    case VToolsBar::PATH:
    {
        PathItem* pathItem=new PathItem(m_first,m_itemColor,m_penSize);
        m_currentItem = pathItem;
        m_currentPath = pathItem;
    }
        break;
    }
    addNewItem(m_currentItem);
}
void VMap::setPenSize(int p)
{
    m_penSize =p;
}
void VMap::setPatternColor(QColor c)
{
    m_gridColor = c;
}
void VMap::fill(NetworkMessageWriter& msg)
{
    msg.string16(mapTitle());
    msg.string8(getId());
    msg.rgb(mapColor());
    msg.uint16(mapWidth());
    msg.uint16(mapHeight());
    msg.uint8((quint8)getPermissionMode());
    msg.uint8((quint8)getVisibilityMode());
    msg.uint64(m_itemMap->values().size());
}
void VMap::readMessage(NetworkMessageReader& msg,bool readCharacter)
{
    m_title = msg.string16();
    m_id = msg.string8();
    m_bgColor = msg.rgb();
    setWidth(msg.uint16());
    setHeight(msg.uint16());
    m_currentMode = (Map::PermissionMode)msg.uint8();
    VMap::VisibilityMode mode = (VMap::VisibilityMode)msg.uint8();
    setVisibilityMode(mode);
    int itemCount = msg.uint64();

    if(readCharacter)
    {
        for(int i = 0; i < itemCount; ++i)
        {
            processAddItemMessage(&msg);
        }
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
    foreach(VisualItem* item, m_itemMap->values())
    {
        msg.uint8(item->getType());
        item->fillMessage(&msg);
    }
}

void VMap::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        if(mouseEvent->button() == Qt::LeftButton)
        {
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
    }
    else if(mouseEvent->button() == Qt::LeftButton)
    {
        m_first = mouseEvent->scenePos();
        m_end = m_first;
        if(m_currentPath==NULL)
        {
            addItem();
        }
        else
        {
            updateItem();
        }
    }
    /*  else if((m_selectedtool == VToolsBar::RULE)&&(mouseEvent->button() == Qt::RightButton))
    {
        mouseEvent->accept();
    }*/
}
void VMap::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentItem!=NULL)
    {
        //Comment it out for testing PEN ITEM.
        if(m_currentItem->getType()!=VisualItem::PATH)
        {
            m_end = mouseEvent->scenePos();
            m_currentItem->setModifiers(mouseEvent->modifiers());
            m_currentItem->setNewEnd( m_end);
            update();
        }
    }
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
bool VMap::editLayer(VisualItem::Layer layer)
{
    if(m_currentLayer!=layer)
    {
        m_currentLayer = layer;
	emit mapChanged();
        foreach(VisualItem* item, m_itemMap->values())
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

void VMap::sendOffItem(VisualItem* item)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addItem);
    msg.string8(m_id);
    msg.uint8(item->getType());
    item->fillMessage(&msg);
    msg.sendAll();
    item->initChildPointItem();
}
void VMap::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    if(m_currentItem!=NULL)
    {
        if(m_currentItem->getType() == VisualItem::RULE )
        {
            removeItem(m_currentItem);
            m_currentItem = NULL;
            return;
        }
        sendOffItem(m_currentItem);
    }
    m_currentItem = NULL;
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
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

QDataStream& operator<<(QDataStream& out, const VMap& con)
{
    out << con.m_width;
    out << con.m_height;
    out << con.m_title;
    out << con.m_bgColor;
    
    out << con.m_itemMap->size();
    for(int i = 0; i< con.m_itemMap->size();i++)
    {
        VisualItem* item = con.m_itemMap->values().at(i);
        out << *item ;
    }
    return out;
}

QDataStream& operator>>(QDataStream& is,VMap& con)
{
    is >>(con.m_width);
    is >>(con.m_height);
    is >>(con.m_title);
    is >>(con.m_bgColor);
    
    int size;
    is >> size;
    
    return is;
}
void VMap::saveFile(QDataStream& out)
{
    if(m_itemMap->isEmpty())
        return;
    
    out << m_width;
    out<< m_height;
    out<< m_title;
    out<< m_bgColor;
    out << m_itemMap->size();

    
    foreach(VisualItem* tmp, m_itemMap->values())
    {
        out << tmp->getType() << *tmp;
    }
}

void VMap::openFile(QDataStream& in)
{
    if(m_itemMap!=NULL)
    {
        in >> m_width;
        in >> m_height;
        in>> m_title;
        in>> m_bgColor;

        int numberOfItem;
        in >> numberOfItem;

        for(int i =0 ; i<numberOfItem;i++)
        {
            VisualItem* item;
            CharacterItem* charItem = NULL;
            item=NULL;
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
                /// @TODO: Reimplement that feature
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

            }
            in >> *item;
            if(NULL!=charItem)
            {
                insertCharacterInMap(charItem);
            }
            addNewItem(item);
            item->initChildPointItem();
        }
    }
}

void VMap::addCharacter(Character* p, QPointF pos)
{
    CharacterItem* item= new CharacterItem(p,pos);
    item->showNpcName(m_showNpcName);
    item->showNpcNumber(m_showNpcNumber);
    item->showPcName(m_showPcName);

    connect(this,SIGNAL(showNpcName(bool)),item,SLOT(showNpcName(bool)));
    connect(this,SIGNAL(showNpcNumber(bool)),item,SLOT(showNpcNumber(bool)));
    connect(this,SIGNAL(showPcName(bool)),item,SLOT(showPcName(bool)));

    insertCharacterInMap(item);

    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addItem);
    msg.string8(m_id);
    msg.uint8(item->getType());
    item->fillMessage(&msg);
    msg.sendAll();
    item->initChildPointItem();
    addNewItem(item);
}

void VMap::setPatternSize(int p)
{
    m_sizePattern = p;
}
int VMap::getPatternSize() const
{
    return m_sizePattern;
}
QColor VMap::getGridColor()const
{
    return m_gridColor;
}
VMap::GRID_PATTERN VMap::getGrid() const
{
    return m_gridPattern;
}
void VMap::setPattern(VMap::GRID_PATTERN p)
{
    m_gridPattern = p;
}
void VMap::setNpcNameVisible(bool b)
{
    m_showNpcName = b;
}

void VMap::setPcNameVisible(bool b)
{
    m_showNpcName = b;
}

void VMap::setNpcNumberVisible(bool b)
{
    m_showNpcNumber = b;
}
QColor VMap::getBackGroundColor() const
{
    return m_bgColor;
}

void VMap::setScale(int p)
{
    m_patternScale = p;
}
int VMap::getPatternUnit()const
{
    return (int)m_patternUnit;
}
int VMap::getScaleValue()const
{
    return m_patternScale;
}
QString VMap::getTitle() const
{
    return m_title;
}
void VMap::computePattern()
{

    if(m_gridPattern == VMap::NONE)
    {
        setBackgroundBrush(m_bgColor);
    }
    else
    {
        QPolygonF polygon;

        if(m_gridPattern==VMap::HEXAGON)
        {
            qreal radius = m_sizePattern/2;
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

            m_computedPattern = QImage(m_sizePattern*1.5,2*hlimit,QImage::Format_RGBA8888_Premultiplied);
            m_computedPattern.fill(m_bgColor);
        }
        else if(m_gridPattern == VMap::SQUARE)
        {
            m_computedPattern = QImage(m_sizePattern,m_sizePattern,QImage::Format_RGB32);
            m_computedPattern.fill(m_bgColor);
            QPointF A(0,0);
            QPointF B(0,m_sizePattern-1);
            QPointF C(m_sizePattern-1,m_sizePattern-1);
            QPointF D(m_sizePattern-1,0);
            polygon << A << B << C << D << A;
        }
        QPainter painter(&m_computedPattern);
        //painter.setRenderHint(QPainter::Antialiasing,true);
        //painter.setRenderHint(QPainter::SmoothPixmapTransform,true);
        QPen pen;
        pen.setColor(m_gridColor);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawPolyline(polygon);
        painter.end();
        m_computedPattern.save("/tmp/pattern.png","PNG");
        setBackgroundBrush(QPixmap::fromImage(m_computedPattern));
    }

}

void VMap::setScaleUnit(int p)
{
    switch(p)
    {
    case PX:
    case FEET:
    case CM:
    case INCH:
    case M:
    case KM:
    case YARD:
    case MILE:
        m_patternUnit = (VMap::SCALE_UNIT)p;
        break;
    default:
        m_patternUnit = PX;
        break;
    }
    
    
}
void VMap::processAddItemMessage(NetworkMessageReader* msg)
{
    if(NULL!=msg)
    {
        VisualItem* item=NULL;
        VisualItem::ItemType type = (VisualItem::ItemType)msg->uint8();
        CharacterItem* charItem = NULL;
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

        }
        if(NULL!=item)
        {
            item->readItem(msg);
            if(NULL!=charItem)
            {
                insertCharacterInMap(charItem);
            }
            addNewItem(item);
            item->initChildPointItem();
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
}

void VMap::addNewItem(VisualItem* item)
{
    if(NULL!=item)
    {
        item->setMapId(m_id);
        setFocusItem(item);
        connect(item,SIGNAL(itemGeometryChanged(VisualItem*)),this,SLOT(sendItemToAll(VisualItem*)));
        connect(item,SIGNAL(itemRemoved(QString)),this,SLOT(removeItemFromScene(QString)));
        connect(item,SIGNAL(duplicateItem(VisualItem*)),this,SLOT(duplicateItem(VisualItem*)));
        connect(item,SIGNAL(itemLayerChanged(VisualItem*)),this,SLOT(checkItemLayer(VisualItem*)));
        connect(item,SIGNAL(promoteItemTo(VisualItem*,VisualItem::ItemType)),this,SLOT(promoteItemInType(VisualItem*,VisualItem::ItemType)));
        QGraphicsScene::addItem(item);

        //Editing permission
        if(m_localIsGM)
        {
            item->setEditableItem(m_localIsGM);
        }
        else if((m_currentMode == Map::GM_ONLY))
        {
            item->setEditableItem(m_localIsGM);
        }
        else if(m_currentMode == Map::PC_ALL)
        {
            item->setEditableItem(true);
        }
        else if(m_currentMode == Map::PC_MOVE)
        {
            if(item->getType()!=VisualItem::CHARACTER)
            {
                item->setEditableItem(false);
            }
            else
            {
                CharacterItem* charItem = dynamic_cast<CharacterItem*>(item);
                if(NULL != charItem )
                {
                    if(charItem->getParentId() == m_localUserId)//LocalUser is owner of item.
                    {
                        item->setEditableItem(true);
                    }
                }
            }
        }


        //View permission
        if((m_localIsGM)||(m_currentVisibityMode == VMap::ALL))
        {
            item->setVisible(true);
        }
        else if(m_currentVisibityMode == VMap::HIDDEN)
        {
            item->setVisible(false);
        }
        else if(m_currentVisibityMode == VMap::CHARACTER)
        {
            QList<CharacterItem*> items = getCharacterOnMap(m_localUserId);
            if(!items.isEmpty())
            {
                /// @todo activate dynamic shadows
            }
            else
            {
                item->setVisible(false);
            }
        }
        m_itemMap->insert(item->getId(),item);
    }
}
QList<CharacterItem*> VMap::getCharacterOnMap(QString id)
{
    /// @todo activate dynamic shadows
    return QList<CharacterItem*>();
}

void VMap::promoteItemInType(VisualItem* item, VisualItem::ItemType type)
{
    if(NULL!=item)
    {
        VisualItem* bis = item->promoteTo(type);
        removeItemFromScene(item->getId());
        addNewItem(bis);
        bis->initChildPointItem();
    }
}

void VMap::removeItemFromScene(QString id)
{
    VisualItem* item = m_itemMap->take(id);
    QGraphicsScene::removeItem(item);
    delete item;

    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::DelItem);
    msg.string8(m_id);//id map
    msg.string16(id);//id item
    msg.sendAll();
}

void VMap::sendItemToAll(VisualItem* item)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::GeometryItemChanged);
    msg.string8(m_id);
    msg.string16(item->getId());
    item->fillMessage(&msg);
    msg.sendAll();
}
void VMap::keyPressEvent(QKeyEvent* event)
{
    if(event->key ()==Qt::Key_Delete)
    {
        foreach(QGraphicsItem* item, selectedItems())
        {
            VisualItem* itemV = dynamic_cast<VisualItem*>(item);
            if(NULL!=itemV)
            {
                removeItemFromScene(itemV->getId());
            }
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void VMap::setLocalIsGM(bool b)
{
    m_localIsGM = b;
}
void VMap::processMoveItemMessage(NetworkMessageReader* msg)
{
    if(NULL!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(NULL!=item)
        {
            item->readPositionMsg(msg);
        }

    }
}
void VMap::processDelItemMessage(NetworkMessageReader* msg)
{
    if(NULL!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(NULL!=item)
        {
            m_itemMap->remove(id);
            QGraphicsScene::removeItem(item);
            delete item;
        }

    }
}
void VMap::setPermissionMode(Map::PermissionMode mode)
{
    m_currentMode = mode;
	emit mapChanged();
}
QString VMap::getPermissionModeText()
{
	QStringList permissionData;
	permissionData<< tr("No Right") << tr("His character") << tr("All Permissions");
	return permissionData.at(m_currentMode);
}
Map::PermissionMode VMap::getPermissionMode()
{
    return m_currentMode;
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

void VMap::dropEvent ( QGraphicsSceneDragDropEvent * event )
{
    const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(data)
    {
        if (data->hasFormat("rolisteam/userlist-item"))
        {
            Person* item = data->getData();
            Character* character = dynamic_cast<Character*>(item);
            if(character)
            {
                addCharacter(character,event->scenePos());
            }
        }
    }
    else
    {
        const QMimeData* mimeData =  event->mimeData();

        if(mimeData->hasUrls())
        {
            foreach(QUrl url, mimeData->urls())
            {
                ImageItem* led = new ImageItem();
                led->setImageUri(url.toLocalFile());
                sendOffItem(led);
                addNewItem(led);
                led->setPos(event->scenePos());
            }

        }

    }
}
void VMap::dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
{
    const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(NULL!=data)
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
    if(NULL!=copy)
    {
        copy->initChildPointItem();
        addNewItem(copy);
        copy->setPos(item->pos());
        clearSelection();
        item->clearFocus();
        copy->clearFocus();
        clearFocus();
        setFocusItem(copy);
        //m_currentItem = copy;
        update();
    }
}
bool VMap::isIdle() const
{
    return (m_currentItem==NULL);
}
void VMap::insertCharacterInMap(CharacterItem* item)
{
    if((NULL!=m_characterItemMap)&&(NULL!=item))
    {
        m_characterItemMap->insert(item->getCharacterId(),item);
        m_sightItem->insertVision(item);
    }
}

bool VMap::setVisibilityMode(VMap::VisibilityMode mode)
{
    if(mode != m_currentVisibityMode)
    {
        m_currentVisibityMode = mode;
		emit mapChanged();
        if(!m_localIsGM)
        {
            if(m_currentVisibityMode == VMap::HIDDEN)
            {
                foreach(VisualItem* item, m_itemMap->values())
                {
                    item->setVisible(false);
                }
                if(NULL!=m_sightItem)
                {
                    m_sightItem->setVisible(false);
                }
            }
            else if(m_currentVisibityMode == VMap::ALL)
            {
                foreach(VisualItem* item, m_itemMap->values())
                {
                    item->setVisible(true);
                }
                if(NULL!=m_sightItem)
                {
                    m_sightItem->setVisible(false);
                }
            }
            else if(VMap::CHARACTER == m_currentVisibityMode)
            {
                if(NULL!=m_sightItem)
                {
                    m_sightItem->setVisible(true);
                }
            }
        }
        else
        {
            if(VMap::CHARACTER == m_currentVisibityMode)
            {
                if(NULL!=m_sightItem)
                {
                    m_sightItem->setVisible(true);
                }
            }
            else
            {
                if(NULL!=m_sightItem)
                {
                    m_sightItem->setVisible(false);
                }
            }
        }
        return true;
    }
    return false;
}
VMap::VisibilityMode VMap::getVisibilityMode()
{
    return m_currentVisibityMode;
}
QString VMap::getVisibilityModeText()
{
	QStringList visibilityData;
	visibilityData << tr("Hidden") << tr("His character") << tr("All visible");
	return visibilityData.at(m_currentVisibityMode);
}
