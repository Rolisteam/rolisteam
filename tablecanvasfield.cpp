#include "tablecanvasfield.h"
#include <QPainter>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "field.h"
#define SQUARE_SIZE 12

HandleItem::HandleItem(QGraphicsObject* parent)
    : QGraphicsObject(parent)
{
    m_currentMotion = X_AXIS;
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
}

HandleItem::~HandleItem()
{

}

QVariant HandleItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && isSelected() )
    {
        QPointF newPos = value.toPointF();
        if(m_currentMotion == X_AXIS)
        {
            newPos.setY(pos().y());
        }
        else if( Y_AXIS == m_currentMotion)
        {
            newPos.setX(pos().x());
        }
        if(newPos != value.toPointF())
        {
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);

}

QRectF HandleItem::boundingRect() const
{
    return QRectF(0,0, SQUARE_SIZE, SQUARE_SIZE);
}

void HandleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::black);
    painter->fillRect(m_startPoint.x(), m_startPoint.y(), SQUARE_SIZE, SQUARE_SIZE,Qt::gray);
    painter->drawRect(m_startPoint.x(), m_startPoint.y(), SQUARE_SIZE, SQUARE_SIZE);
}

void HandleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseMoveEvent(event);
}

void HandleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
}





//////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////
bool compareHandles(HandleItem* first, HandleItem* two)
{
    return (first->pos().x() < two->pos().x());
}


TableCanvasField::TableCanvasField(Field* field)
 :CanvasField(field),m_colunmCount(1),m_lineCount(1),m_dataReset(true)
{
    m_addColumn= new ButtonCanvas();

    m_dialog = new ColumnDefinitionDialog();
    m_addColumn->setParentItem(this);
    m_addLine = new ButtonCanvas();

    m_defineColumns = new QAction(tr("Define Columns"),this);

    connect(m_defineColumns,SIGNAL(triggered(bool)),this,SLOT(defineColumns()));

    m_addLine->setMsg("+");
    m_addColumn->setMsg("+");

    m_addLine->setParentItem(this);

    m_addLine->setPos(24,200);
    m_addColumn->setPos(200,24);
    connect(m_addColumn,SIGNAL(clicked()),this,SLOT(addColumn()));
    connect(m_addLine,SIGNAL(clicked()),this,SLOT(addLine()));
}
void TableCanvasField::addColumn()
{
    m_colunmCount++;

    HandleItem* item = new HandleItem(this);
    m_handles.append(item);

    qreal colW = boundingRect().width()/(m_handles.size()+1);
    for(auto item : m_handles)
    {
        item->setPos(colW*(m_handles.indexOf(item)+1),boundingRect().height()/2);
    }
    update();
}


void TableCanvasField::addLine()
{
    m_lineCount++;
    update();
}

void TableCanvasField::defineColumns()
{
    std::sort(m_handles.begin(),m_handles.end(),compareHandles);

    if(m_dataReset)
    {
        m_dialog->setData(m_handles,boundingRect().width());
        m_dataReset = false;
    }

    m_dialog->exec();

    update();
}

void TableCanvasField::setMenu(QMenu &menu)
{
    menu.addAction(m_defineColumns);


    //menu.addAction(m_properties);
}

QList<HandleItem *> TableCanvasField::handles() const
{
    return m_handles;
}

void TableCanvasField::setHandles(const QList<HandleItem *> &handles)
{
    m_handles = handles;
}

QList<CharacterSheetItem::TypeField> TableCanvasField::fieldTypes() const
{
    return m_fieldTypes;
}

void TableCanvasField::setFieldTypes(const QList<CharacterSheetItem::TypeField> &fieldTypes)
{
    m_fieldTypes = fieldTypes;
}

int TableCanvasField::lineCount() const
{
    return m_lineCount;
}

void TableCanvasField::setLineCount(int lineCount)
{
    m_lineCount = lineCount;
}

int TableCanvasField::colunmCount() const
{
    return m_colunmCount;
}

void TableCanvasField::setColunmCount(int colunmCount)
{
    m_colunmCount = colunmCount;
}

void TableCanvasField::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    
    if(NULL==m_field)
        return;
    painter->save();
    painter->fillRect(m_rect,m_field->bgColor());
    painter->setPen(Qt::black);
    painter->drawRect(m_rect);

    m_addLine->setPos(0,boundingRect().height()/2);
    m_addColumn->setPos(boundingRect().width()/2,0);

    if(hasFocusOrChild() )
    {
        painter->save();
        QPen pen = painter->pen();
        pen.setColor(Qt::red);
        pen.setWidth(5);
        painter->setPen(pen);
        painter->drawRect(m_rect);
        painter->restore();
    }

    for(auto handle : m_handles)
    {
        painter->drawLine(handle->pos().x(),0,handle->pos().x(),boundingRect().height());
        handle->setVisible(hasFocusOrChild());
    }

    auto yStep = boundingRect().height()/(m_lineCount);

    if(!m_dataReset)
    {
        auto model = m_dialog->model();
        if(nullptr != model)
        {
            auto list = model->children();
            for(int x = 0; x < m_colunmCount; ++x)
            {
                auto  field = list.at(x);
                qreal xPos = 0;
                if(x!=0)
                {
                    xPos = m_handles.at(x-1)->pos().x();
                }
                auto itemType = field->getItemType();
                auto fieldW = field->getValueFrom(CharacterSheetItem::WIDTH,Qt::DisplayRole).toDouble();
                auto fieldH = field->getValueFrom(CharacterSheetItem::HEIGHT,Qt::DisplayRole).toDouble();
                for(int y = 0; y < m_lineCount; ++y)
                {
                    QRectF rect(xPos,y*yStep,fieldW,fieldH);
                    QPixmap map = QPixmap(m_pictureMap[itemType]);
                    m_pix=map.scaled(32,32);
                    if((!m_pix.isNull())&& m_showImageField )
                    {
                        painter->drawPixmap(rect.center()-m_pix.rect().center(),m_pix,m_pix.rect());
                    }
                    painter->save();
                    painter->setPen(Qt::green);
                    painter->drawRect(rect);
                    painter->restore();
                }
            }

        }

    }


    for(int i = 0; i < m_lineCount ; ++i)
    {
        painter->drawLine(0,i*yStep,boundingRect().width(),i*yStep);
    }
    painter->drawText(QPoint(0,0),m_field->getId());
    painter->restore();
}
bool TableCanvasField::hasFocusOrChild()
{
        if(isSelected())
        {
            return true;
        }
        else
        {
            if(m_handles.isEmpty())
            {
                return false;
            }
            for(int i = 0; i< m_handles.size();++i)
            {
                if((m_handles.at(i)!=NULL)&&(m_handles.at(i)->isSelected()))
                {
                    return true;
                }
            }
        }

    return false;
}

//////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////
ButtonCanvas::ButtonCanvas()
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlags( QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );
}

QRectF ButtonCanvas::boundingRect() const
{
      return QRectF(0,0, SQUARE_SIZE*2, SQUARE_SIZE*2);
}

void ButtonCanvas::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->fillRect(boundingRect(),Qt::blue);
    painter->setPen(Qt::black);
    painter->drawRect(boundingRect());
    painter->drawText(boundingRect(),m_msg);
    painter->restore();

}

QString ButtonCanvas::msg() const
{
    return m_msg;
}

void ButtonCanvas::setMsg(const QString &msg)
{
    m_msg = msg;
}
#include <QGraphicsSceneMouseEvent>
void ButtonCanvas::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
   if(event->buttons() & Qt::LeftButton)
   {
       emit clicked();
   }
   else
   {
       QGraphicsObject::mousePressEvent(event);
   }
}
