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
    m_editionMode = VToolsBar::Painting;
    m_propertiesHash = new QHash<VisualItem::Properties,QVariant>();

    PlayersList* list = PlayersList::instance();
    connect(list,SIGNAL(characterDeleted(Character*)),this,SLOT(characterHasBeenDeleted(Character*)));

    connect(this, SIGNAL(selectionChanged()),this,SLOT(selectionHasChanged()));

    m_penSize = 1;
    m_id = QUuid::createUuid().toString();
    m_currentItem = NULL;
    m_currentPath = NULL;
    m_fogItem = NULL;
    m_itemMap=new  QMap<QString,VisualItem*>;
    m_characterItemMap = new QMap<QString,CharacterItem*>();
    setItemIndexMethod(QGraphicsScene::NoIndex);

    m_sightItem = new SightItem(m_characterItemMap);

    m_propertiesHash->insert(VisualItem::ShowNpcName,false);
    m_propertiesHash->insert(VisualItem::ShowPcName,false);
    m_propertiesHash->insert(VisualItem::ShowNpcNumber,false);
    m_propertiesHash->insert(VisualItem::ShowHealtStatus,false);
    m_propertiesHash->insert(VisualItem::ShowGrid,false);
    m_propertiesHash->insert(VisualItem::LocalIsGM,false);
    m_propertiesHash->insert(VisualItem::EnableCharacterVision,false);
    m_propertiesHash->insert(VisualItem::PermissionMode,Map::GM_ONLY);
}

VToolsBar::SelectableTool VMap::getSelectedtool() const
{
    return m_selectedtool;
}
VisualItem::Layer VMap::currentLayer() const
{
    return m_currentLayer;
}

void VMap::setCurrentLayer(const VisualItem::Layer &currentLayer)
{
    editLayer(currentLayer);
    //m_currentLayer = currentLayer;
    emit mapChanged();
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
    emit mapChanged();
}

void VMap::setBackGroundColor(QColor bgcolor)
{
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

void VMap::setCurrentTool(VToolsBar::SelectableTool selectedtool)
{
    cleanFogEdition();
    if((m_selectedtool == VToolsBar::PATH)&&(m_selectedtool != selectedtool))
    {
        m_currentPath = NULL;
    }
    m_selectedtool = selectedtool;
    m_currentItem = NULL;

}

void VMap::setPenSize(int p)
{
    m_penSize =p;
}
void VMap::characterHasBeenDeleted(Character* character)
{
    QList<CharacterItem*> list = getCharacterOnMap(character->getUuid());
    foreach(CharacterItem* item,list)
    {
        m_sightItem->removeVision(item);
        m_characterItemMap->remove(character->getUuid());
        removeItemFromScene(item->getId());
    }
}

void VMap::fill(NetworkMessageWriter& msg)
{
    msg.string16(getMapTitle());
    msg.string8(getId());
    msg.rgb(mapColor());
    msg.uint16(mapWidth());
    msg.uint16(mapHeight());
    msg.string8(m_sightItem->getId());
    msg.uint8((quint8)getPermissionMode());
    msg.uint8((quint8)getVisibilityMode());
    msg.uint8(getOption(VisualItem::EnableCharacterVision).toBool());
    msg.uint64(m_itemMap->values().size());
}
void VMap::readMessage(NetworkMessageReader& msg,bool readCharacter)
{
    m_title = msg.string16();
    m_id = msg.string8();
    m_bgColor = msg.rgb();
    setWidth(msg.uint16());
    setHeight(msg.uint16());
    QString idSight = msg.string8();
    m_sightItem->setId(idSight);
    quint8 permissionMode = msg.uint8();
    VMap::VisibilityMode mode = (VMap::VisibilityMode)msg.uint8();
    quint8 enableCharacter = msg.uint8();
    int itemCount = msg.uint64();
    if(readCharacter)
    {
        for(int i = 0; i < itemCount; ++i)
        {
            processAddItemMessage(&msg);
        }
    }

    blockSignals(true);
    setPermissionMode((Map::PermissionMode)permissionMode);
    setOption(VisualItem::EnableCharacterVision,enableCharacter);
    setVisibilityMode(mode);
    blockSignals(false);
    emit mapStatutChanged();
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
        if(NULL!=item)
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
    if(NULL!=m_fogItem)
    {
        removeItem(m_fogItem);
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
    else if(mouseEvent->button()==Qt::RightButton)
    {
        m_currentPath = NULL;
        cleanFogEdition();
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
    }
}
void VMap::addImageItem(QString file)
{
    ImageItem* led = new ImageItem();
    led->setImageUri(file);
    led->initChildPointItem();
    addNewItem(led);
}

void VMap::setCurrentItemOpacity(qreal a)
{
    QList<QGraphicsItem*> selection= selectedItems();
    for(auto item : selection)
    {
        VisualItem* vItem = dynamic_cast<VisualItem*>(item);
        if(NULL!=vItem)
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
        if(NULL!=vItem)
        {
            emit currentItemOpacity(vItem->opacity());
            break;
        }
    }
}

void VMap::addItem()
{
    switch(m_selectedtool)
    {
    case VToolsBar::PEN:
        m_currentItem=new PathItem(m_first,m_itemColor,m_penSize,true);
        break;
    case VToolsBar::PATH:
    {
        PathItem* pathItem=new PathItem(m_first,m_itemColor,m_penSize);
        m_currentItem = pathItem;
        m_currentPath = pathItem;
    }
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
        QGraphicsItem* item = itemAt(m_first,QTransform());
        TextItem* tmp = dynamic_cast<TextItem*>(item);
        QGraphicsTextItem* tmpGraph = dynamic_cast<QGraphicsTextItem*>(item);
        if((NULL==tmp)&&(NULL==tmpGraph))
        {
            TextItem* temptext = new TextItem(m_first,m_penSize,m_itemColor);
            m_currentItem = temptext;
        }
        else
        {
           return;
        }
    }
        break;
    case VToolsBar::TEXTBORDER:
    {
        QGraphicsItem* item = itemAt(m_first,QTransform());
        TextItem* tmp = dynamic_cast<TextItem*>(item);
        QGraphicsTextItem* tmpGraph = dynamic_cast<QGraphicsTextItem*>(item);
        if((NULL==tmp)&&(NULL==tmpGraph))
        {
            TextItem* temptext = new TextItem(m_first,m_penSize,m_itemColor);
            temptext->setBorderVisible(true);
            m_currentItem = temptext;
        }
        else
        {
           return;
        }
    }
        break;
    case VToolsBar::HANDLER:

        break;
    case VToolsBar::ADDNPC:
    {
        CharacterItem* itemCharar= new CharacterItem(new Character(m_currentNpcName,m_itemColor,true,m_currentNpcNumber),m_first);
        m_currentItem = itemCharar;
        emit npcAdded();
    }
        break;
    case VToolsBar::RULE:
    {
        RuleItem* itemRule = new RuleItem(m_first);
        itemRule->setUnit((VMap::SCALE_UNIT)getOption(VisualItem::Unit).toInt());
        itemRule->setPixelToUnit(getOption(VisualItem::GridSize).toInt()/getOption(VisualItem::Scale).toReal());
        m_currentItem = itemRule;
    }
        break;
    case VToolsBar::ANCHOR:
        AnchorItem* anchorItem = new AnchorItem(m_first);
        m_currentItem = anchorItem;
        break;
    }
    addNewItem(m_currentItem);

    if(VToolsBar::Painting != m_editionMode)
    {
        m_fogItem = m_currentItem;
    }

}
void VMap::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentItem!=NULL)
    {
        //Comment it out for testing PEN ITEM.
        //if(m_selectedtool==VToolsBar::PEN)
        //{
            m_end = mouseEvent->scenePos();
            m_currentItem->setModifiers(mouseEvent->modifiers());
            m_currentItem->setNewEnd( m_end);
            update();
        //}
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
    if((NULL!=m_currentPath)&&((VToolsBar::Painting==m_editionMode)))
    {
        if(VisualItem::PATH == m_currentPath->getType())
        {

            PathItem* itm = dynamic_cast<PathItem*>(m_currentPath);
            if(NULL!=itm)
            {
                itm->release();
            }
            //ManageAnchor();

        }
    }
    if(m_currentItem!=NULL)
    {
        if(VisualItem::ANCHOR == m_currentItem->getType())
        {
            manageAnchor();
        }
        if((m_currentItem->getType() == VisualItem::RULE )||(m_currentItem->getType() == VisualItem::ANCHOR))
        {
            removeItem(m_currentItem);
            m_currentItem = NULL;
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
            if(NULL==m_currentPath)
            {
                removeItem(m_currentItem);
            }

        }
    }
    else if((NULL!=m_currentPath)&&(VToolsBar::Painting!=m_editionMode))
    {
        QPolygonF* poly = new QPolygonF();
        *poly = m_currentPath->shape().toFillPolygon();
        m_currentFog->setPolygon(poly);
        update();
        //removeItem(m_currentPath);
    }

    m_currentItem = NULL;
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}
void VMap::setAnchor(QGraphicsItem* child,QGraphicsItem* parent,bool send)
{
    if(NULL!=child)
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::SetParentItem);
        msg.string8(m_id);
        VisualItem* childItem = dynamic_cast<VisualItem*>(child);

        if(NULL!=childItem)
        {
            msg.string8(childItem->getId());
        }

        QPointF pos = child->pos();
        QPointF pos2;
        if(NULL!=parent)
        {
            pos2 =parent->mapFromScene(pos);
        }
        else
        {
            if(NULL!=child->parentItem())
            {
                pos2 = child->parentItem()->mapToScene(pos);
            }
        }
        VisualItem* paItem = dynamic_cast<VisualItem*>(parent);
        if(NULL!=paItem)
        {
            msg.string8(paItem->getId());
        }
        else
        {
            msg.string8("NULL");
        }
        if(send)
        {
            msg.sendAll();
        }
        child->setParentItem(parent);
        child->setPos(pos2);
    }
}

void VMap::manageAnchor()
{
    AnchorItem* tmp = dynamic_cast< AnchorItem*>(m_currentItem);
    if(NULL!=tmp)
    {
        QGraphicsItem* child = NULL;
        QGraphicsItem* parent = NULL;
        QList<QGraphicsItem*> item1 = items(tmp->getStart());

        foreach (QGraphicsItem* item, item1)
        {
            if(item!=m_currentItem)
            {
                child = item;
            }
        }

        QList<QGraphicsItem*> item2 = items(tmp->getEnd());
        foreach (QGraphicsItem* item, item2)
        {
            if(item!=m_currentItem)
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




    out << m_propertiesHash->count();
    foreach(VisualItem::Properties property, m_propertiesHash->keys())
    {
        out << (int)property;
        out << m_propertiesHash->value(property);
    }


    out << m_itemMap->size();
    foreach(QString key, m_sortedItemList)//m_itemMap->values()
    {
        VisualItem* tmp = m_itemMap->value(key);
        if(NULL!=tmp)
        {
            out << tmp->getType() << *tmp << tmp->pos().x() << tmp->pos().y();
        }
    }
}
//read
void VMap::openFile(QDataStream& in)
{
    if(m_itemMap!=NULL)
    {
        in >> m_width;
        in >> m_height;

        setSceneRect();
        in>> m_title;
        in>> m_bgColor;

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
            case VisualItem::SIGHT:
                item= m_sightItem;
                break;
            case VisualItem::IMAGE:
                item= new ImageItem();
                break;
            default:
                qDebug() << "not managed item"<< type;
                break;

            }
            if(NULL!=item)
            {
                in >> *item;
                qreal x,y;
                in >> x;
                in >> y;
                addNewItem(item);
                item->setPos(x,y);
                item->initChildPointItem();
                if(NULL!=charItem)
                {
                    insertCharacterInMap(charItem);
                }
            }
        }
    }
}

void VMap::addCharacter(Character* p, QPointF pos)
{

    CharacterItem* item= new CharacterItem(p,pos);

    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addItem);
    msg.string8(m_id);
    msg.uint8(item->getType());
    item->fillMessage(&msg);
    msg.sendAll();
    item->initChildPointItem();
    addNewItem(item);

    insertCharacterInMap(item);
}
QColor VMap::getBackGroundColor() const
{
    return m_bgColor;
}
void VMap::computePattern()
{
    if((getOption(VisualItem::GridPattern).toInt() == VMap::NONE)||(!getOption(VisualItem::ShowGrid).toBool()))
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
        //painter.setRenderHint(QPainter::Antialiasing,true);
        //painter.setRenderHint(QPainter::SmoothPixmapTransform,true);
        QPen pen;
        pen.setColor(getOption(VisualItem::GridColor).value<QColor>());
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawPolyline(polygon);
        painter.end();
        /*QTemporaryFile file;
        m_computedPattern.save(file,"PNG");*/
        setBackgroundBrush(QPixmap::fromImage(m_computedPattern));

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
        case VisualItem::SIGHT:
            item=m_sightItem;
            break;
        }
        if(NULL!=item)
        {
            item->readItem(msg);
            QPointF pos = item->pos();
            addNewItem(item);
            item->initChildPointItem();
            if(NULL!=charItem)
            {
                insertCharacterInMap(charItem);
            }
            item->setPos(pos);
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
void VMap::processOpacityMessage(NetworkMessageReader* msg)
{
    if(NULL!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(NULL!=item)
        {
            item->readOpacityMsg(msg);
        }
    }
}
void VMap::processLayerMessage(NetworkMessageReader* msg)
{
    qDebug() << "processe Layer Message;";
    if(NULL!=msg)
    {
        QString id = msg->string16();
        VisualItem* item = m_itemMap->value(id);
        if(NULL!=item)
        {
            item->readLayerMsg(msg);
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
            m_sortedItemList.removeAll(id);
            m_characterItemMap->remove(id);
            QGraphicsScene::removeItem(item);
            delete item;
        }

    }
}
void VMap::processSetParentItem(NetworkMessageReader* msg)
{
    if(NULL!=msg)
    {
        QString childId = msg->string8();
        QString parentId = msg->string8();
        VisualItem* childItem = m_itemMap->value(childId);

        VisualItem* parentItem = NULL;
        if(parentId!=QStringLiteral("NULL"))
        {
            parentItem = m_itemMap->value(parentId);
        }
        if(NULL!=childItem)
        {
            setAnchor(childItem,parentItem,false);
        }
    }
}

void VMap::addNewItem(VisualItem* item)
{
    if(NULL!=item)
    {
        item->setMapId(m_id);
        item->setPropertiesHash(m_propertiesHash);
        setFocusItem(item);
        connect(item,SIGNAL(itemGeometryChanged(VisualItem*)),this,SLOT(sendItemToAll(VisualItem*)));
        connect(item,SIGNAL(itemRemoved(QString)),this,SLOT(removeItemFromScene(QString)));
        connect(item,SIGNAL(duplicateItem(VisualItem*)),this,SLOT(duplicateItem(VisualItem*)));
        connect(item,SIGNAL(itemLayerChanged(VisualItem*)),this,SLOT(checkItemLayer(VisualItem*)));
        connect(item,SIGNAL(promoteItemTo(VisualItem*,VisualItem::ItemType)),this,SLOT(promoteItemInType(VisualItem*,VisualItem::ItemType)));

        if((item!=m_sightItem)&&(VisualItem::ANCHOR!=item->type())&&(VisualItem::RULE!=item->type()))
        {
            m_orderedItemList.append(item);
        }

        connect(item,SIGNAL(changeStackPosition(VisualItem*,VisualItem::StackOrder)),this,SLOT(changeStackOrder(VisualItem*,VisualItem::StackOrder)));

        item->setLayer(m_currentLayer);
        QGraphicsScene::addItem(item);



        //Editing permission
        if(getOption(VisualItem::LocalIsGM).toBool())
        {
            if(item!=m_sightItem)
            {
                item->setEditableItem(getOption(VisualItem::LocalIsGM).toBool());
            }
        }
        else if((getPermissionMode() == Map::GM_ONLY))
        {
            item->setEditableItem(getOption(VisualItem::LocalIsGM).toBool());
        }
        else if(getPermissionMode() == Map::PC_ALL)
        {
            item->setEditableItem(true);
        }
        else if(getPermissionMode() == Map::PC_MOVE)
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
        if((getOption(VisualItem::LocalIsGM).toBool())||(m_currentVisibityMode == VMap::ALL))
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
                item->setVisible(true);
            }
        }
        if(VToolsBar::Painting == m_editionMode)
        {
            if(item->type() != VisualItem::ANCHOR)
            {
                m_itemMap->insert(item->getId(),item);
                m_sortedItemList << item->getId();
            }
        }
    }
}
QList<CharacterItem*> VMap::getCharacterOnMap(QString id)
{
    /// @todo activate dynamic shadows
    QList<CharacterItem*> result;
    foreach(CharacterItem* item, m_characterItemMap->values())
    {
        if(NULL!=item)
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
    if(m_sightItem->getId()==id)
    {
        return;
    }
    VisualItem* item = m_itemMap->take(id);

    // if(m_item )
    m_characterItemMap->remove(id);
    m_sortedItemList.removeAll(id);
    if(item->getType() == VisualItem::CHARACTER)
    {
        CharacterItem* cItem = dynamic_cast<CharacterItem*>(item);
        if(NULL != cItem)
        {
            m_characterItemMap->remove(cItem->getCharacterId());
        }
    }
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
        QStringList idListToRemove;
        foreach(QGraphicsItem* item, selectedItems())
        {
            VisualItem* itemV = dynamic_cast<VisualItem*>(item);
            if(NULL!=itemV)
            {
                if(itemV->isEditable())
                {
                    idListToRemove << itemV->getId();
                }
            }
        }
        for(auto id : idListToRemove)
        {
            removeItemFromScene(id);
        }
        event->accept();
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
                foreach(VisualItem* item,m_itemMap->values())
                {
                    item->setEditableItem(false);
                }
                foreach(CharacterItem* item,m_characterItemMap->values())
                {
                    item->setEditableItem(false);
                }
            }
            else
            {
                bool value = false;//GM_ONLY
                if(Map::PC_ALL == mode)
                {
                    value = true;
                }
                foreach(VisualItem* item,m_itemMap->values())
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
    return (Map::PermissionMode)getOption(VisualItem::PermissionMode).toInt();
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
    const RolisteamMimeData* data = qobject_cast<const RolisteamMimeData*>(event->mimeData());
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
                if(url.isLocalFile())
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
        sendOffItem(copy);
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
        m_characterItemMap->insertMulti(item->getCharacterId(),item);
        m_sightItem->insertVision(item);
        if((item->isPlayableCharacter())&&(!getOption(VisualItem::LocalIsGM).toBool())&&(m_localUserId==item->getParentId()))
        {
            changeStackOrder(item,VisualItem::FRONT);
        }
    }
}

bool VMap::setVisibilityMode(VMap::VisibilityMode mode)
{
    bool result = false;
    if(mode != m_currentVisibityMode)
    {
        result = true;
    }
    m_currentVisibityMode = mode;
    if(result)
    {
       emit mapChanged();
    }
    bool visibilitySight = false;
    bool visibilityItems = false;
    if(!getOption(VisualItem::LocalIsGM).toBool())
    {
        if(m_currentVisibityMode == VMap::HIDDEN)
        {
            visibilityItems = false;
        }
        else if(m_currentVisibityMode == VMap::ALL)
        {
            visibilityItems = true;
        }
        else if(VMap::CHARACTER == m_currentVisibityMode)
        {
            visibilitySight = true;
            visibilityItems = true;
        }
    }
    else
    {
        visibilityItems = true;
        if(VMap::CHARACTER == m_currentVisibityMode)
        {
            visibilitySight = true;
        }
        else
        {
            visibilitySight = false;
        }
    }
    foreach(VisualItem* item, m_itemMap->values())
    {
        item->setVisible(visibilityItems);
    }
    if(NULL!=m_sightItem)
    {
        m_sightItem->setVisible(visibilitySight);
    }
    return result;
}
VMap::VisibilityMode VMap::getVisibilityMode()
{
    return m_currentVisibityMode;
}
void VMap::setOption(VisualItem::Properties pop,QVariant value)
{
    if(NULL!=m_propertiesHash)
    {
        if(getOption(pop)!=value)
        {
            m_propertiesHash->insert(pop,value);
            emit mapChanged();
            computePattern();
            update();
        }
    }
}
QVariant VMap::getOption(VisualItem::Properties pop)
{
    if(NULL!=m_propertiesHash)
    {
        return m_propertiesHash->value(pop);
    }
    return QVariant();
}
QString VMap::getVisibilityModeText()
{
    QStringList visibilityData;
    visibilityData << tr("Hidden") << tr("Fog Of War") << tr("All visible");
    if(m_currentVisibityMode>0 && m_currentVisibityMode < visibilityData.size())
    {
        return visibilityData.at(m_currentVisibityMode);
    }
    else
    {
        return QString();
    }
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
    if (NULL== item || m_sortedItemList.size() < 2)
        return;
    int size = m_sortedItemList.size();
    int index = m_sortedItemList.indexOf(item->getId());

    // find out insertion indexes over the stacked items
    QList<QGraphicsItem *> stackedItems = items(item->sceneBoundingRect(), Qt::IntersectsItemShape);
    int prevIndex = 0;
    int nextIndex = size - 1;
    foreach (QGraphicsItem * qitem, stackedItems)
    {
        // operate only on different Content and not on sightItem.
        VisualItem* c = dynamic_cast<VisualItem*>(qitem);
        if (!c || c == item || c == m_sightItem)
            continue;

        // refine previous/next indexes (close to 'index')
        int cIdx = m_sortedItemList.indexOf(c->getId());
        if (cIdx < nextIndex && cIdx > index)
            nextIndex = cIdx;
        else if (cIdx > prevIndex && cIdx < index)
            prevIndex = cIdx;
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
    m_sortedItemList.append(m_sightItem->getId());
    int z =0;
    //foreach (VisualItem* item, )
    for(QString key : m_sortedItemList)
    {
        VisualItem* item = m_itemMap->value(key);
        if(NULL!=item)
        {
            item->setZValue(++z);
        }
    }
    qDebug() << m_sortedItemList.contains(m_sightItem->getId()) << "######################";

    m_sightItem->setZValue(++z);
    //ensure that character player are above everythings.
    if(!getOption(VisualItem::LocalIsGM).toBool())
    {
        foreach(CharacterItem* item,m_characterItemMap->values())
        {
            if(item->getParentId() == m_localUserId)
            {
                item->setZValue(++z);
            }
        }
    }
}
