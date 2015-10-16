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
#include <QGraphicsSceneWheelEvent>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

TextItem::TextItem()
{
    init();
    createActions();
}

TextItem::TextItem(QPointF& start/*,QLineEdit* editor*/,QColor& penColor,QGraphicsItem * parent)
    : VisualItem(penColor,parent)
{
    init();
    m_start = start;
    createActions();
}

void TextItem::init()
{
    //setFlag(QGraphicsItem::ItemHasNoContents,true);
    setPos(m_start);
    m_textItem = new QGraphicsTextItem(this);
    m_textItem->setFocus();
    m_textItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    m_textItem->setPos(QPointF(0,0));
    m_textItem->setTextWidth(200);
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
}

QRectF TextItem::boundingRect() const
{
    return m_textItem->boundingRect();
}
void TextItem::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
 /*   if(!m_text.isEmpty())
    {
        setChildrenVisible(hasFocusOrChild());

        painter->save();
        painter->setFont(m_font);
        painter->setPen(m_color);
        painter->drawText(QPoint(0,0),m_text);
       // painter->drawRect(boundingRect());
        painter->restore();
    }    */

    painter->drawRect(boundingRect());
  //  QGraphicsObject::paint(painter,option,widget);
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
    m_rect = metric.boundingRect(m_text);
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

void TextItem::editingFinished()
{
    setOpacity(1.0);
    /*if(m_textEdit!=NULL)
    {
        m_text = m_textEdit->text();
        if(!m_text.isEmpty())
        {
            updateFont();
            m_textEdit->setVisible(false);
            itemGeometryChanged(this);
            updateChildPosition();
        }
    }*/
    
}
void TextItem::setGeometryPoint(qreal /*pointId*/, QPointF &pos)
{
    m_start = pos;
    setPos(m_start);
    //m_textItem->setPos(pos);
}
void TextItem::initChildPointItem()
{
    m_rect = m_rect.normalized();
    setTransformOriginPoint(m_rect.center());
    m_child = new QVector<ChildPointItem*>();

    for(int i = 0; i< 1 ; ++i)
    {
        ChildPointItem* tmp = new ChildPointItem(i,this);
        tmp->setMotion(ChildPointItem::NONE);
        tmp->setRotationEnable(true);
        m_child->append(tmp);
    }
   updateChildPosition();
}
void TextItem::updateChildPosition()
{
    m_child->value(0)->setPos(m_rect.center());
    m_child->value(0)->setPlacement(ChildPointItem::Center);
    setTransformOriginPoint(m_rect.center());
    update();
}
void TextItem::writeData(QDataStream& out) const
{
    out << m_start;
    out << m_text;
    out << m_color;
    out << m_id;
}
void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    /*setOpacity(0.1);
   m_textEdit->setVisible(true);
    m_textEdit->setFocus();*/
    VisualItem::mouseDoubleClickEvent(event);
}

void TextItem::readData(QDataStream& in)
{
    in >> m_start;
    in >> m_text;
    in >> m_color;
    in >> m_id;
}
void TextItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->real(scale());
    msg->real(rotation());
    //center
    msg->real(m_start.x());
    msg->real(m_start.y());
    msg->string32(m_text);
    msg->rgb(m_color);
}
void TextItem::readItem(NetworkMessageReader* msg)
{
    m_id = msg->string16();
    setScale(msg->real());
    setRotation(msg->real());
    //center
    m_start.setX(msg->real());
    m_start.setY(msg->real());
    m_text = msg->string32();
    m_color = msg->rgb();
}
VisualItem* TextItem::getItemCopy()
{
    TextItem* rectItem = new TextItem(m_start/*,new QLineEdit(),*/,m_color);
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
}
void TextItem::decreaseTextSize()
{
    int i = m_font.pointSize();
    m_font.setPointSize(--i);
}
