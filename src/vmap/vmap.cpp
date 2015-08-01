#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QKeyEvent>

#include "vmap.h"
#include "items/rectitem.h"
#include "items/ellipsitem.h"
#include "items/pathitem.h"
#include "items/lineitem.h"
#include "items/textitem.h"
#include "items/characteritem.h"


#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

VMap::VMap(QObject * parent)
    : QGraphicsScene(parent)
{
    m_penSize = 1;
    m_currentItem = NULL;
    m_id = QUuid::createUuid().toString();
    m_itemMap=new  QMap<QString,VisualItem*>;
    setItemIndexMethod(QGraphicsScene::NoIndex);
}


VMap::VMap(int width,int height,QString& title,QColor& bgColor,QObject * parent)
    : QGraphicsScene(0,0,width,height,parent)
{
    m_title = title;
    m_penSize = 1;
    m_bgColor = bgColor;
    setBackgroundBrush(m_bgColor);
    m_id = QUuid::createUuid().toString();
    m_currentItem = NULL;
    m_itemMap=new  QMap<QString,VisualItem*>;
    setItemIndexMethod(QGraphicsScene::NoIndex);
    
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

void VMap::setBackGroundColor(QColor& bgcolor)
{
    m_bgColor = bgcolor;
    generateBackground();
}
void VMap::generateBackground()
{
    if(m_gridPattern.isNull())
    {
        setBackgroundBrush(m_bgColor);
    }
    else
    {
        QPixmap p(m_gridPattern);
        p.fill(m_bgColor);
        QPainter painter(&p);
        painter.drawPixmap(0,0,m_gridPattern);
        painter.end();
        setBackgroundBrush(p);
    }
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
int VMap::getNpcSize() const
{
    return m_npcSize;
}

void VMap::setCurrentTool(VToolsBar::SelectableTool selectedtool)
{
    m_selectedtool = selectedtool;
    m_currentItem = NULL;
}
void VMap::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentItem!=NULL)
    {
        /*  if(m_selectedtool!=ToolsBar::LINE)
    {*/

        m_end = mouseEvent->scenePos ();
        m_currentItem->setNewEnd( m_end);
        update();
        // }
    }
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
void VMap::addItem()
{
    //QGraphicsItem* item = NULL;
    
    switch(m_selectedtool)
    {
    case VToolsBar::PEN:
        m_currentItem=new PathItem(m_first,m_itemColor,m_penSize);
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
        tmp->setPos(m_first.x(),m_first.y()-tempedit->height());
        tempedit->setEnabled(true);
        tempedit->setFocus();
        connect(tempedit,SIGNAL(editingFinished()),temptext,SLOT(editingFinished()));
        connect(tempedit,SIGNAL(editingFinished()),this,SLOT(update()));
    }
        break;
    case VToolsBar::HANDLER:

        break;
    case VToolsBar::ADDNPC:

        break;
    case VToolsBar::DELNPC:

        break;
    case VToolsBar::MOVECHARACTER:

        break;
    case VToolsBar::STATECHARACTER:

        break;
    }
    addNewItem(m_currentItem);
}
void VMap::setPenSize(int p)
{
    m_penSize =p;
}

void VMap::setNPCSize(int p)
{
    m_npcSize =p;
}
void VMap::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    
    if(m_selectedtool==VToolsBar::HANDLER)
    {
        //m_currentItem = dynamic_cast<VisualItem*>(itemAt(mouseEvent->scenePos()));
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
    else if(mouseEvent->button() == Qt::LeftButton)
    {
        m_first = mouseEvent->scenePos();
        m_end = m_first;
        addItem();
    }

}

void VMap::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);
    if(m_currentItem!=NULL)
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addItem);
        msg.string8(m_id);
        msg.uint8(m_currentItem->getType());
        m_currentItem->fillMessage(&msg);
        msg.sendAll();

        m_currentItem->initChildPointItem();
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
        qDebug()<< "m_itemList size" << numberOfItem <<  m_bgColor << m_width << m_height << m_title ;
        for(int i =0 ; i<numberOfItem;i++)
        {
            VisualItem* item;
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
                // item=new CharacterItem();

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
            addNewItem(item);
        }
    }
}

void VMap::addCharacter(const Character* p, QPointF pos)
{
    CharacterItem* item= new CharacterItem(p,pos);
    QGraphicsScene::addItem(item);
}

void VMap::setPatternSize(int p)
{
    m_sizePattern = p;
}

void VMap::setPattern(QPixmap p)
{
    m_gridPattern = p;
}

void VMap::setScale(int p)
{
    m_patternScale = p;
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
        switch(type)
        {
        case VisualItem::TEXT:
            item=new TextItem();
            break;
        case VisualItem::CHARACTER:
            /// @TODO: Reimplement that feature
            item=new CharacterItem();
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
        if(NULL!=item)
        {
            item->readItem(msg);
            addNewItem(item);
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
        connect(item,SIGNAL(itemGeometryChanged(VisualItem*)),this,SLOT(sendItemToAll(VisualItem*)));
        connect(item,SIGNAL(itemRemoved(QString)),this,SLOT(removeItemFromScene(QString)));
        QGraphicsScene::addItem(item);
		item->setEditableItem(m_localIsGM);
        m_itemMap->insert(item->getId(),item);
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
}
Map::PermissionMode VMap::getPermissionMode()
{
	return m_currentMode;
}
