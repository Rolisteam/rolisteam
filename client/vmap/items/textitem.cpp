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
#include <QDebug>
#include <QFont>
#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QStyle>

#include <QGraphicsSceneWheelEvent>
#include <QStyleOptionGraphicsItem>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "vmap/vmap.h"

///////////////////////////////////////
/// Code of TextLabel
///////////////////////////////////////
TextLabel::TextLabel(QGraphicsItem* parent) : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    // setAcceptHoverEvents(true);
}
void TextLabel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    VMap* map= dynamic_cast<VMap*>(scene());
    if(nullptr != map)
    {
        if(map->getSelectedtool() == VToolsBar::HANDLER)
        {
            // event->accept();
            setFocus(Qt::OtherFocusReason);
            QGraphicsTextItem::mousePressEvent(event);
        }
        else if((map->getSelectedtool() == VToolsBar::TEXT) || (map->getSelectedtool() == VToolsBar::TEXTBORDER))
        {
            QGraphicsTextItem::mousePressEvent(event);
        }
    }
}

void TextLabel::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

///////////////////////////////////////
/// Code of RichTextEditDialog
///////////////////////////////////////

RichTextEditDialog::RichTextEditDialog()
{
    m_richText= new MRichTextEdit(this);
    QVBoxLayout* lay= new QVBoxLayout(this);
    lay->addWidget(m_richText);

    QDialogButtonBox* dialogButton= new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    lay->addWidget(dialogButton);

    connect(dialogButton, SIGNAL(accepted()), this, SLOT(accept()));
    connect(dialogButton, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(lay);
}

void RichTextEditDialog::setText(QString str)
{
    m_richText->setText(str);
}

QString RichTextEditDialog::getText()
{
    return m_richText->toHtml();
}

///////////////////////
/// Code of TextItem
//
//
//
///////////////////////

RichTextEditDialog* TextItem::m_dialog= nullptr;
TextItem::TextItem() : m_offset(QPointF(100, 30))
{
    init();
    createActions();
}

TextItem::TextItem(const QPointF& start, quint16 penSize, const QColor& penColor, QGraphicsItem* parent)
    : VisualItem(penColor, penSize, parent), m_offset(QPointF(100, 30))
{
    m_start= start;
    m_rect.setTopLeft(QPointF(0, 0));
    m_rect.setBottomRight(m_offset);
    setPos(m_start);
    init();
    createActions();
}

void TextItem::init()
{
    if(nullptr == m_dialog)
    {
        m_dialog= new RichTextEditDialog();
    }
    setAcceptHoverEvents(true);
    m_showRect= false;
    m_textItem= new TextLabel(this);
    m_textItem->setFocus();
    m_textItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    m_textItem->setPos(QPointF(0, 0));
    m_textItem->setTextWidth(100);
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_textItem->setDefaultTextColor(m_color);
    m_doc= new QTextDocument(m_textItem);
    m_doc->setPlainText(tr("Text"));
    // m_doc->setDefaultStyleSheet(QString("color: %1;").arg(m_color.name()));
    m_textItem->setDocument(m_doc);

    connect(m_doc, SIGNAL(contentsChanged()), this, SLOT(updateTextPosition()));
    // connect(m_doc,SIGNAL(contentsChanged()),this,SLOT(textHasChanged()));
}

QRectF TextItem::boundingRect() const
{
    return m_rect;
}
void TextItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    setChildrenVisible(hasFocusOrChild());
    if(m_showRect)
    {
        QPen pen= painter->pen();
        pen.setColor(m_color);
        pen.setWidth(m_showRect ? m_penWidth : 2);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }
    if(option->state & QStyle::State_MouseOver)
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawRect(m_rect);
        painter->restore();
    }
}
void TextItem::setNewEnd(QPointF& p){Q_UNUSED(p)

} VisualItem::ItemType TextItem::getType() const
{
    return VisualItem::TEXT;
}
void TextItem::updateFont()
{
    QFontMetrics metric(m_font);
    QRectF rect= metric.boundingRect(m_doc->toPlainText());
    if(rect.height() > m_rect.height())
    {
        m_rect.setHeight(rect.height());
    }
    if(rect.width() > m_rect.width())
    {
        m_rect.setWidth(rect.width());
    }
    m_textItem->setFont(m_font);
    updateChildPosition();
    update();
}
void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        setChildrenVisible(false);
        m_menuPos= event->screenPos();
        editText();
        event->accept();
    }
    else
    {
        VisualItem::mouseDoubleClickEvent(event);
    }
}
void TextItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if(event->modifiers() & Qt::ControlModifier)
    {
        if(event->delta() > 0)
        {
            int i= m_font.pointSize();
            m_font.setPointSize(++i);
        }
        else
        {
            int i= m_font.pointSize();
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

void TextItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    switch((int)pointId)
    {
    case 0:
        m_rect.setTopLeft(pos);
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 1:
        m_rect.setTopRight(pos);
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(2)->setPos(m_rect.bottomRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 2:
        m_rect.setBottomRight(pos);
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(3)->setPos(m_rect.bottomLeft());
        break;
    case 3:
        m_rect.setBottomLeft(pos);
        m_child->value(0)->setPos(m_rect.topLeft());
        m_child->value(1)->setPos(m_rect.topRight());
        m_child->value(2)->setPos(m_rect.bottomRight());
        break;
    default:
        break;
    }

    setTransformOriginPoint(m_rect.center());

    // updateTextPosition();
    m_resizing= true;
}
void TextItem::endOfGeometryChange()
{
    updateTextPosition();
    VisualItem::endOfGeometryChange();
}

void TextItem::updateTextPosition()
{
    m_textItem->setTextWidth(m_rect.width() - 10);
    QRectF rectItem= m_textItem->boundingRect();
    setTransformOriginPoint(m_rect.center());
    if(rectItem.height() > m_rect.height())
    {
        m_rect.setTop(m_rect.top() - 5);
        m_rect.setBottom(m_rect.bottom() + 5);
    }
    setTransformOriginPoint(m_rect.center());
    m_textItem->setPos(m_rect.center().x() - rectItem.width() / 2, m_rect.center().y() - rectItem.height() / 2);

    if((nullptr != m_child) && (!m_child->isEmpty()))
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
    if(m_doc == qobject_cast<QTextDocument*>(sender()))
    {
        emit itemGeometryChanged(this);
    }
}

void TextItem::initChildPointItem()
{
    m_rect= m_rect.normalized();
    setTransformOriginPoint(m_rect.center());

    updateTextPosition();

    m_child= new QVector<ChildPointItem*>();

    for(int i= 0; i < 4; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(i, this);
        tmp->setMotion(ChildPointItem::ALL);
        tmp->setRotationEnable(true);
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

void TextItem::editText()
{
    m_dialog->setText(m_textItem->toHtml());
    m_dialog->move(m_menuPos);
    if(QDialog::Accepted == m_dialog->exec())
    {
        m_textItem->setHtml(m_dialog->getText());
        updateTextPosition();
        emit itemGeometryChanged(this);
    }
}

void TextItem::writeData(QDataStream& out) const
{
    out << m_start;
    out << m_showRect;
    out << m_doc->toHtml();
    out << opacity();
    out << m_color;
    out << m_id;
    out << m_rect;
    out << m_penWidth;
    out << scale();
    out << rotation();
    out << pos();
    // out << zValue();
    out << (int)m_layer;
}

void TextItem::readData(QDataStream& in)
{
    in >> m_start;
    QString text;
    in >> m_showRect;
    in >> text;
    m_doc->setHtml(text);
    qreal opa= 0;
    in >> opa;
    setOpacity(opa);
    in >> m_color;
    m_textItem->setDefaultTextColor(m_color);

    in >> m_id;
    in >> m_rect;
    in >> m_penWidth;

    qreal scale;
    in >> scale;
    setScale(scale);

    qreal rotation;
    in >> rotation;
    setRotation(rotation);

    QPointF pos;
    in >> pos;
    setPos(pos);

    int i;
    in >> i;
    m_layer= (VisualItem::Layer)i;
}
void TextItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    msg->uint8((quint8)m_showRect);
    msg->real(scale());
    msg->real(rotation());
    msg->uint8((int)m_layer);
    msg->real(zValue());
    msg->real(opacity());

    // m_rect
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());

    msg->uint32(m_penWidth);

    // pos
    msg->real(pos().x());
    msg->real(pos().y());
    // center
    msg->real(m_start.x());
    msg->real(m_start.y());
    msg->string32(m_doc->toHtml());
    msg->rgb(m_color.rgb());
    msg->uint8((quint8)m_showRect);
}
void TextItem::readItem(NetworkMessageReader* msg)
{
    blockSignals(true);
    m_id= msg->string16();
    m_showRect= static_cast<bool>(msg->uint8());
    setScale(msg->real());
    setRotation(msg->real());
    m_layer= (VisualItem::Layer)msg->uint8();
    setZValue(msg->real());
    setOpacity(msg->real());

    // m_rect
    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());

    m_penWidth= msg->uint32();

    // pos
    qreal x= msg->real();
    qreal y= msg->real();
    setPos(x, y);
    // center
    m_start.setX(msg->real());
    m_start.setY(msg->real());
    m_doc->setHtml(msg->string32());
    m_color= msg->rgb();
    m_showRect= static_cast<bool>(msg->uint8());
    blockSignals(false);
    m_textItem->setDefaultTextColor(m_color);

    update();
}
VisualItem* TextItem::getItemCopy()
{
    TextItem* rectItem= new TextItem(m_start, m_penWidth, m_color);
    return rectItem;
}
void TextItem::addActionContextMenu(QMenu* menu)
{
    QAction* edit= menu->addAction(tr("Edit Text…"));
    connect(edit, SIGNAL(triggered(bool)), this, SLOT(editText()));

    QAction* adapt= menu->addAction(tr("Adapt to content"));
    connect(adapt, SIGNAL(triggered(bool)), this, SLOT(sizeToTheContent()));

    QMenu* state= menu->addMenu(tr("Font Size"));
    state->addAction(m_increaseFontSize);
    state->addAction(m_decreaseFontSize);
}
void TextItem::createActions()
{
    m_increaseFontSize= new QAction(tr("Increase Text Size"), this);
    m_decreaseFontSize= new QAction(tr("Decrease Text Size"), this);

    connect(m_increaseFontSize, SIGNAL(triggered()), this, SLOT(increaseTextSize()));
    connect(m_decreaseFontSize, SIGNAL(triggered()), this, SLOT(decreaseTextSize()));
}
void TextItem::sizeToTheContent()
{
    QRectF rectItem= m_textItem->boundingRect();
    setTransformOriginPoint(m_rect.center());
    if(rectItem.height() < m_rect.height() + 10)
    {
        m_rect.setHeight(rectItem.height() + 10);
        m_resizing= true;
    }
    if(rectItem.width() < m_rect.width() + 10)
    {
        m_rect.setWidth(rectItem.width() + 10);
        m_resizing= true;
    }
    if(m_resizing)
    {
        endOfGeometryChange();
    }
}

void TextItem::increaseTextSize()
{
    int i= m_font.pointSize();
    m_font.setPointSize(++i);
    m_textItem->setFont(m_font);
}
void TextItem::decreaseTextSize()
{
    int i= m_font.pointSize();
    m_font.setPointSize(--i);
    m_textItem->setFont(m_font);
}
void TextItem::setBorderVisible(bool b)
{
    m_showRect= b;
}
void TextItem::setRectSize(qreal x, qreal y, qreal w, qreal h)
{
    VisualItem::setRectSize(x, y, w, h);

    updateTextPosition();
}
void TextItem::updateItemFlags()
{
    VisualItem::updateItemFlags();
    if(!canBeMoved())
    {
        m_textItem->setTextInteractionFlags(Qt::NoTextInteraction);
    }
    else
    {
        m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    }
}
void TextItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event)
    if(canBeMoved())
    {
        VMap* vmap= dynamic_cast<VMap*>(scene());
        if(nullptr != vmap)
        {
            VToolsBar::SelectableTool tool= vmap->getSelectedtool();
            if((VToolsBar::TEXT == tool) || (VToolsBar::TEXTBORDER == tool))
            {
                QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));
            }
        }
    }
    VisualItem::hoverEnterEvent(event);
}
void TextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event)
    if(canBeMoved())
        QApplication::restoreOverrideCursor();

    VisualItem::hoverLeaveEvent(event);
}
void TextItem::keyPressEvent(QKeyEvent* event)
{
    if(m_textItem->hasCursor())
    {
        QGraphicsItem::keyPressEvent(event);
    }
    else
    {
        VisualItem::keyPressEvent(event);
    }
}
