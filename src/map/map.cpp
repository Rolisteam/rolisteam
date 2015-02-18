#include "map.h"
#include "rectitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
Map::Map(QObject * parent)
    : QGraphicsScene(parent)
{

}


Map::Map(int width,int height,QString& title,QColor& bgColor,QObject * parent)
    : QGraphicsScene(0,0,width,height,parent)
{
    m_title = title;
    m_bgColor = bgColor;
    setBackgroundBrush(m_bgColor);
    currentItem = NULL;

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
    currentItem = NULL;
 }
 void Map::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
 {
     if(currentItem!=NULL)
     {
         m_end = mouseEvent->scenePos ();
         currentItem->setNewEnd( m_end);
         update();
     }
 }
void Map::addItem()
{
   //QGraphicsItem* item = NULL;
    qDebug() << "add Item" << m_selectedtool;
    switch(m_selectedtool)
    {
        case ToolsBar::PEN:
            break;
        case ToolsBar::LINE:
            break;
        case ToolsBar::EMPTYRECT:
            currentItem = new RectItem(m_first,m_end,false);
            break;
        case ToolsBar::FILLRECT:
            currentItem = new RectItem(m_first,m_end,true);
            break;
        case ToolsBar::EMPTYELLIPSE:
        case ToolsBar::FILLEDELLIPSE:

            break;
        case ToolsBar::TEXT:

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
    if(currentItem!=NULL)
        QGraphicsScene::addItem(currentItem);
}
 void Map::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
 {

     if(mouseEvent->button() == Qt::LeftButton)
     {
         m_first = mouseEvent->scenePos();
         m_end = m_first;
         addItem();
     }
 }

 void Map::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
 {
        Q_UNUSED(mouseEvent);
 }
 void Map::setCurrentChosenColor(QColor& p)
 {
    Q_UNUSED(p);
 }
