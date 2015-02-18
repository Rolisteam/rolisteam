#include "map.h"
#include "rectitem.h"
#include "ellipsitem.h"
#include "pathitem.h"
#include "lineitem.h"
#include "textitem.h"
#include "characteritem.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
Map::Map(QObject * parent)
    : QGraphicsScene(parent)
{
    m_currentItem = NULL;
    m_itemList=new  QList<VisualItem*>;
	setItemIndexMethod(QGraphicsScene::NoIndex);
}


Map::Map(int width,int height,QString& title,QColor& bgColor,QObject * parent)
    : QGraphicsScene(0,0,width,height,parent)
{
    m_title = title;
    m_bgColor = bgColor;
    setBackgroundBrush(m_bgColor);
    m_currentItem = NULL;
    m_itemList=new  QList<VisualItem*>;
	setItemIndexMethod(QGraphicsScene::NoIndex);

}

void Map::setWidth(int width)
{
    m_width = width;
    setSceneRect();
}

void Map::setHeight(int height)
{
    m_height = height;
    setSceneRect();
}

void Map::setTitle(QString title)
{
    m_title = title;
}

void Map::setBackGroundColor(QColor& bgcolor)
{
    m_bgColor = bgcolor;
    setBackgroundBrush(bgcolor);
}
void Map::setSceneRect()
{
    QGraphicsScene::setSceneRect(0,0,m_width,m_height);
}
int Map::mapWidth() const
{
    return m_width;
}
int Map::mapHeight() const
{
    return m_height;
}
const QString& Map::mapTitle() const
{
    return m_title;
}
const QColor& Map::mapColor() const
{
    return m_bgColor;
}
 void Map::setCurrentTool(ToolsBar::SelectableTool selectedtool)
 {
    m_selectedtool = selectedtool;
    m_currentItem = NULL;
 }
 void Map::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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
    if(m_selectedtool==ToolsBar::HANDLER)
          QGraphicsScene::mouseMoveEvent(mouseEvent);
 }
void Map::addItem()
{
   //QGraphicsItem* item = NULL;

    switch(m_selectedtool)
    {
        case ToolsBar::PEN:
             m_currentItem=new PathItem(m_first,m_itemColor,m_penSize);
            break;
        case ToolsBar::LINE:
            m_currentItem= new LineItem(m_first,m_itemColor,m_penSize);
            break;
        case ToolsBar::EMPTYRECT:
            m_currentItem = new RectItem(m_first,m_end,false,m_itemColor);
            break;
        case ToolsBar::FILLRECT:
            m_currentItem = new RectItem(m_first,m_end,true,m_itemColor);
            break;
        case ToolsBar::EMPTYELLIPSE:
            m_currentItem = new EllipsItem(m_first,false,m_itemColor);
            break;
        case ToolsBar::FILLEDELLIPSE:
            m_currentItem = new EllipsItem(m_first,true,m_itemColor);
            break;
        case ToolsBar::TEXT:
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
        case ToolsBar::HANDLER:

            break;
        case ToolsBar::ADDNPC:

            break;
        case ToolsBar::DELNPC:

            break;
        case ToolsBar::MOVECHARACTER:

            break;
        case ToolsBar::STATECHARACTER:

            break;
    }
    if(m_currentItem!=NULL)
    {
        QGraphicsScene::addItem(m_currentItem);
        m_itemList->append(m_currentItem);
    }
}
void Map::setPenSize(int p)
{
    m_penSize =p;
}

void Map::setNPCSize(int p)
{
    m_npcSize =p;
}
 void Map::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
 {

     if(mouseEvent->button() == Qt::LeftButton)
     {
         m_first = mouseEvent->scenePos();
         m_end = m_first;
         addItem();
     }

     if(m_selectedtool==ToolsBar::HANDLER)
        QGraphicsScene::mousePressEvent(mouseEvent);
 }

 void Map::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
 {
     Q_UNUSED(mouseEvent);
     m_currentItem = NULL;
     if(m_selectedtool==ToolsBar::HANDLER)
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
 }
 void Map::setCurrentChosenColor(QColor& p)
 {
    m_itemColor = p;


 }

QDataStream& operator<<(QDataStream& out, const Map& con)
{
   out << con.m_width;
   out << con.m_height;
   out << con.m_title;
   out << con.m_bgColor;

    out << con.m_itemList->size();
   for(int i = 0; i< con.m_itemList->size();i++)
   {
       VisualItem* item = con.m_itemList->at(i);
       out << *item ;
   }
   return out;
}

QDataStream& operator>>(QDataStream& is,Map& con)
{
   is >>(con.m_width);
   is >>(con.m_height);
   is >>(con.m_title);
   is >>(con.m_bgColor);

   int size;
   is >> size;



   return is;
 }
void Map::saveFile(QDataStream& out)
{
    if(m_itemList->isEmpty())
        return;

    out << m_width;
    out<< m_height;
    out<< m_title;
    out<< m_bgColor;
    out << m_itemList->size();
    qDebug()<< "m_itemList size" << m_itemList->size() <<  m_bgColor << m_width << m_height << m_title ;

    foreach(VisualItem* tmp, *m_itemList)
    {
        out << tmp->getType() << *tmp;
    }
}

void Map::openFile(QDataStream& in)
{
    if(m_itemList!=NULL)
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
            in >> *item;
            QGraphicsScene::addItem(item);
            m_itemList->append(item);
         }
        qDebug()<< m_itemList->size();
    }
}
