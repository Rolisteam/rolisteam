/***************************************************************************
    *      Copyright (C) 2010 by Renaud Guezennec                             *
    *                                                                         *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    *   This program is distributed in the hope that it will be useful,       *
    *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
    *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
    *   GNU General Public License for more details.                          *
    *                                                                         *
    *   You should have received a copy of the GNU General Public License     *
    *   along with this program; if not, write to the                         *
    *   Free Software Foundation, Inc.,                                       *
    *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
    ***************************************************************************/
#include "textitem.h"
#include <QLineEdit>
#include <QPainter>
#include <QObject>
#include <QDebug>
#include <QFont>
#include <QMenu>
#include <QStyle>

#include <QGraphicsSceneWheelEvent>
#include <QStyleOptionGraphicsItem>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

#include "vmap/vmap.h"

TextLabel::TextLabel(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{

}
void TextLabel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    VMap* map = dynamic_cast<VMap*>(scene());
    if( NULL != map)
    {
        if(map->getSelectedtool() == VToolsBar::HANDLER)
        {
            event->ignore();
        }
        else if((map->getSelectedtool() == VToolsBar::TEXT)||
                (map->getSelectedtool() == VToolsBar::TEXTBORDER))
        {
            QGraphicsTextItem::mousePressEvent(event);
        }
    }
}

void TextLabel::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    VMap* map = dynamic_cast<VMap*>(scene());
    qDebug() << "double click";
    if( NULL != map)
    {
        if(map->getSelectedtool() == VToolsBar::HANDLER)
        {
            qDebug() << "double click HANDLER";
        }
        else if((map->getSelectedtool() == VToolsBar::TEXT)||
                (map->getSelectedtool() == VToolsBar::TEXTBORDER))
        {
             qDebug() << "double click TEXT";
        }
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);

}

///////////////////////
//
//
//
///////////////////////
TextItem::TextItem()
    : m_offset(QPointF(100,30))
{
    init();
    createActions();
}

TextItem::TextItem(QPointF& start,quint16 penSize,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent),m_penWidth(penSize),m_offset(QPointF(100,30))
{
    m_start = start;
    m_rect.setTopLeft(QPointF(0,0));
    m_rect.setBottomRight(m_offset);
    setPos(m_start);
    //m_rect.setCoords(-m_rect.width()/2,-m_rect.height()/2,m_rect.width()/2,m_rect.height()/2);
    init();
    createActions();
}

void TextItem::init()
{
    setAcceptHoverEvents(true);
    m_showRect = false;
    m_textItem = new TextLabel(this);
    m_textItem->setFocus();
    m_textItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    m_textItem->setPos(QPointF(0,0));
    m_textItem->setTextWidth(100);
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_doc = new QTextDocument(m_textItem);
    m_doc->setHtml(tr("<b style=\"color: %1\">Text</b>").arg(m_color.name()));
    //m_doc->setDefaultStyleSheet(QString("color: %1;").arg(m_color.name()));
    m_textItem->setDocument(m_doc);

    connect(m_doc,SIGNAL(contentsChanged()),this,SLOT(updateTextPosition()));
    //connect(m_doc,SIGNAL(contentsChanged()),this,SLOT(textHasChanged()));

}

QRectF TextItem::boundingRect() const
{
    return m_rect;
}
void TextItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    setChildrenVisible(hasFocusOrChild());
    if((m_showRect)||(option->state & QStyle::State_MouseOver))
    {
        QPen pen = painter->pen();
        pen.setColor(m_color);
        pen.setWidth(m_showRect?m_penWidth:2);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }
}
void TextItem::setNewEnd(QPointF& p)
{

}
VisualItem::ItemType TextItem::getType()
{
    return VisualItem::TEXT;
}
void TextItem::updateFont()
{
    QFontMetrics metric(m_font);
    m_rect = metric.boundingRect(m_doc->toHtml());
    updateChildPosition();
    update();
}

void TextItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if(event->modifiers() && Qt::ControlModifier)
    {
        if(event->delta()>0)
        {
            int i = m_font.pointSize();
            m_font.setPointSize(++i);
        }
        else
        {
            int i = m_font.pointSize();
            m_font.setPointSize(--i);
        }
        updateFont();
        event->accept();
    }
    else
    {
        VisualItem::wheelEvent(event);
    }
}

void TextItem::setGeometryPoint(qreal pointId, QPointF &pos)
{
    switch ((int)pointId)
    {
    case 0:
        m_rect.setTopLeft(pos);
        updateTextPosition();
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 1:
        m_rect.setTopRight(pos);
        updateTextPosition();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 2:
        m_rect.setBottomRight(pos);
        updateTextPosition();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 3:
        m_rect.setBottomLeft(pos);
        updateTextPosition();
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        break;
    default:
        break;
    }

    setTransformOriginPoint(m_rect.center());

    updateTextPosition();
}
void TextItem::updateTextPosition()
{
    m_textItem->setTextWidth(m_rect.width()-10);
    QRectF rectItem = m_textItem->boundingRect();
    setTransformOriginPoint(m_rect.center());
    if(rectItem.height() > m_rect.height())
    {
        m_rect.setTop(m_rect.top()-5);
        m_rect.setBottom(m_rect.bottom()+5);
    }
    setTransformOriginPoint(m_rect.center());
    m_textItem->setPos(m_rect.center().x()-rectItem.width()/2,m_rect.center().y()-rectItem.height()/2);

    if((NULL!=m_child)&&(!m_child->isEmpty()))
    {
        if(!m_child->at(0)->isSelected())
        {
            m_child->value(0)->setPos(m_rect.topLeft());
        }
        if(!m_child->at(1)->isSelected())
        {
            m_child->value(1)->setPos(m_rect.topRight());
        }
        if(!m_child->at(2)->isSelected())
        {
            m_child->value(2)->setPos(m_rect.bottomRight());
        }
        if(!m_child->at(3)->isSelected())
        {
            m_child->value(3)->setPos(m_rect.bottomLeft());
        }
    }
    itemGeometryChanged(this);
}

void TextItem::initChildPointItem()
{
    m_rect = m_rect.normalized();
    setTransformOriginPoint(m_rect.center());

    updateTextPosition();

    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 4 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this);
        tmp->setMotion(ChildPointItem::ALL);
        m_child->append(tmp);
    }
    updateChildPosition();
}
void TextItem::updateChildPosition()
{
    m_child->value(0)->setPos(m_rect.topLeft());
    m_child->value(0)->setPlacement(ChildPointItem::TopLeft);

    m_child->value(1)->setPos(m_rect.topRight());
    m_child->value(1)->setPlacement(ChildPointItem::TopRight);

    m_child->value(2)->setPos(m_rect.bottomRight());
    m_child->value(2)->setPlacement(ChildPointItem::ButtomRight);

    m_child->value(3)->setPos(m_rect.bottomLeft());
    m_child->value(3)->setPlacement(ChildPointItem::ButtomLeft);

    setTransformOriginPoint(m_rect.center());
    update();
}
void TextItem::writeData(QDataStream& out) const
{
    out << m_start;
    out << m_doc->toHtml();
    out << m_color;
    out << m_id;
    out << m_rect;
    out << scale();
    out << rotation();
    out << pos();
}
void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setChildrenVisible(false);
    VisualItem::mouseDoubleClickEvent(event);
}
void TextItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    VisualItem::mousePressEvent(event);
}

void TextItem::readData(QDataStream& in)
{
    in >> m_start;
    QString text;
    in >> text;
    m_doc->setHtml(text);
    in >> m_color;
    in >> m_id;
    in >> m_rect;

    qreal scale;
    in >> scale;
    setScale(scale);

    qreal rotation;
    in >> rotation;
    setRotation(rotation);

    QPointF pos;
    in >> pos;
    setPos(pos);


}
void TextItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    //pos
    msg->real(pos().x());
    msg->real(pos().y());
    //center
    msg->real(m_start.x());
    msg->real(m_start.y());
    msg->string32(m_doc->toHtml());
    msg->rgb(m_color);
}//Votre appel est en attente.
void TextItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
    //pos
    qreal x = msg->real();
    qreal y = msg->real();
    setPos(x,y);
    //center
    m_start.setX(msg->real());
    m_start.setY(msg->real());
    m_doc->setHtml(msg->string32());
    m_color = msg->rgb();
}
VisualItem* TextItem::getItemCopy()
{
    TextItem* rectItem = new TextItem(m_start,m_penWidth,m_color);
    return rectItem;
}
void TextItem::addActionContextMenu(QMenu* menu)
{
    QMenu* state =  menu->addMenu(tr("Font Size"));
    state->addAction(m_increaseFontSize);
    state->addAction(m_decreaseFontSize);
}
void TextItem::createActions()
{
    m_increaseFontSize = new QAction(tr("Increase Text Size"),this);
    m_decreaseFontSize= new QAction(tr("Decrease Text Size"),this);

    connect(m_increaseFontSize,SIGNAL(triggered()),this,SLOT(increaseTextSize()));
    connect(m_decreaseFontSize,SIGNAL(triggered()),this,SLOT(decreaseTextSize()));
}
void TextItem::increaseTextSize()
{
    int i = m_font.pointSize();
    m_font.setPointSize(++i);
    m_textItem->setFont(m_font);
}
void TextItem::decreaseTextSize()
{
    int i = m_font.pointSize();
    m_font.setPointSize(--i);
    m_textItem->setFont(m_font);
}
void TextItem::setBorderVisible(bool b)
{
    m_showRect = b ;
}

void TextItem::setEditableItem(bool b)
{
    VisualItem::setEditableItem(b);
    if(!b)
    {
        m_textItem->setTextInteractionFlags(Qt::NoTextInteraction);
    }
    else
    {
        m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    }
}
void TextItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if(isEditable())
    {
        VMap* vmap = dynamic_cast<VMap*>(scene());
        if(NULL!=vmap)
        {
            VToolsBar::SelectableTool tool = vmap->getSelectedtool();
            if((VToolsBar::TEXT ==tool)||(VToolsBar::TEXTBORDER== tool))
            {
                QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));
            }
        }

    }
}
void TextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if(isEditable())
    QApplication::restoreOverrideCursor();
}


