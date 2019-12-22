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

#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "vmap/controller/textcontroller.h"
#include "vmap/controller/visualitemcontroller.h"
#include "widgets/MRichTextEditor/mrichtextedit.h"

#include "vmap/vmap.h"

///////////////////////////////////////
/// Code of TextLabel
///////////////////////////////////////
TextLabel::TextLabel(QGraphicsItem* parent) : QGraphicsTextItem(parent)
{
    setFlag(QGraphicsItem::ItemIsFocusable, true);
}

QRectF TextLabel::textRect() const
{
    return m_rect;
}

QRectF TextLabel::boundingRect() const
{
    auto rect= QGraphicsTextItem::boundingRect();

    QRectF maxRect(0, 0, std::max(m_rect.width(), rect.width()), std::max(rect.height(), m_rect.height()));
    return maxRect;
}

void TextLabel::setTextRect(const QRectF& rect)
{
    if(rect == m_rect)
        return;

    m_rect= rect;
    emit textRectChanged();
    setTextWidth(rect.width());
}
void TextLabel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    VMap* map= dynamic_cast<VMap*>(scene());
    if(nullptr != map)
    {
        // if(map->getSelectedtool() == Core::HANDLER)
        {
            // event->accept();
            setFocus(Qt::OtherFocusReason);
            QGraphicsTextItem::mousePressEvent(event);
        }
        // else if((map->getSelectedtool() == Core::TEXT) || (map->getSelectedtool() == Core::TEXTBORDER))
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

    connect(dialogButton, &QDialogButtonBox::accepted, this, &RichTextEditDialog::accept);
    connect(dialogButton, &QDialogButtonBox::rejected, this, &RichTextEditDialog::reject);

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

TextItem::TextItem(vmap::TextController* ctrl)
    : VisualItem(ctrl)
    , m_textCtrl(ctrl)
    , m_doc(new QTextDocument)
    , m_textItem(new TextLabel(this))
    , m_increaseFontSize(new QAction(tr("Increase Text Size"), this))
    , m_decreaseFontSize(new QAction(tr("Decrease Text Size"), this))
{
    if(nullptr == m_dialog)
        m_dialog= new RichTextEditDialog();

    connect(m_increaseFontSize.get(), &QAction::triggered, m_textCtrl, &vmap::TextController::increaseFontSize);
    connect(m_decreaseFontSize.get(), &QAction::triggered, m_textCtrl, &vmap::TextController::decreaseFontSize);

    m_textItem->setDocument(m_doc.get());
    connect(m_textCtrl, &vmap::TextController::borderRectChanged, this, [this]() {
        setTransformOriginPoint(m_textCtrl->borderRect().center());
        m_textItem->setTextWidth(m_textCtrl->borderRect().width());
        m_textCtrl->setTextRect(m_textItem->boundingRect());
        updateChildPosition();
    });
    connect(m_textCtrl, &vmap::TextController::textRectChanged, this,
            [this]() { m_textItem->setTextWidth(m_textCtrl->textRect().width()); });
    connect(m_textCtrl, &vmap::TextController::textPosChanged, this,
            [this](const QPointF& pos) { m_textItem->setPos(pos); });
    connect(m_textCtrl, &vmap::TextController::rotationChanged, this,
            [this]() { setRotation(m_textCtrl->rotation()); });

    connect(m_textCtrl, &vmap::TextController::textChanged, m_doc.get(), &QTextDocument::setPlainText);
    connect(m_doc.get(), &QTextDocument::contentsChanged, this, [this]() {
        m_textCtrl->setText(m_doc->toPlainText());
        auto rect= m_textItem->boundingRect();
        m_textCtrl->setTextRect(rect);
    });

    for(int i= 0; i <= vmap::TextController::BottomLeft; ++i)
    {
        ChildPointItem* tmp= new ChildPointItem(m_textCtrl, i, this);
        tmp->setMotion(ChildPointItem::MOUSE);
        m_children.append(tmp);
    }
    updateChildPosition();

    setAcceptHoverEvents(true);
    m_textItem->setFocus();
    m_textItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
    m_textItem->setPos(QPointF(0, 0));
    m_textItem->setTextWidth(100);
    m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    m_textItem->setDefaultTextColor(m_textCtrl->color());
    m_doc->setPlainText(tr("Text"));
}

QRectF TextItem::boundingRect() const
{
    return m_textCtrl->borderRect();
}
void TextItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    setChildrenVisible(hasFocusOrChild());
    if(m_textCtrl->border())
    {
        QPen pen= painter->pen();
        pen.setColor(m_textCtrl->color());
        pen.setWidth(m_textCtrl->border() ? m_textCtrl->penWidth() : 2);
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
        painter->drawRect(m_textCtrl->borderRect());
        painter->restore();
    }
}

void TextItem::setNewEnd(const QPointF& p){Q_UNUSED(p)}

VisualItem::ItemType TextItem::getType() const
{
    return VisualItem::TEXT;
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
    if(!event->modifiers() & Qt::ControlModifier)
        VisualItem::wheelEvent(event);

    event->delta() > 0 ? m_textCtrl->increaseFontSize() : m_textCtrl->decreaseFontSize();
    event->accept();
}

void TextItem::setGeometryPoint(qreal pointId, QPointF& pos)
{
    if(m_holdSize)
        return;

    /* switch(static_cast<int>(pointId))
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

     setTransformOriginPoint(m_rect.center());*/

    // updateTextPosition();
    m_resizing= true;
}

void TextItem::setHoldSize(bool holdSize)
{
    VisualItem::setHoldSize(holdSize);
    for(auto child : m_children)
    {
        auto motion= holdSize ? ChildPointItem::NONE : ChildPointItem::ALL;
        child->setMotion(motion);
    }
}

void TextItem::initChildPointItem()
{
    // updateTextPosition();
    updateChildPosition();
}
void TextItem::updateChildPosition()
{
    auto rect= m_textCtrl->borderRect();
    m_children.value(0)->setPos(rect.topLeft());
    m_children.value(0)->setPlacement(ChildPointItem::TopLeft);
    m_children.value(1)->setPos(rect.topRight());
    m_children.value(1)->setPlacement(ChildPointItem::TopRight);
    m_children.value(2)->setPos(rect.bottomRight());
    m_children.value(2)->setPlacement(ChildPointItem::ButtomRight);
    m_children.value(3)->setPos(rect.bottomLeft());
    m_children.value(3)->setPlacement(ChildPointItem::ButtomLeft);
    update();
}

void TextItem::editText()
{
    m_dialog->setText(m_textItem->toHtml());
    m_dialog->move(m_menuPos);
    if(QDialog::Accepted == m_dialog->exec())
    {
        m_textItem->setHtml(m_dialog->getText());
        // updateTextPosition();
        emit itemGeometryChanged(this);
    }
}

void TextItem::writeData(QDataStream& out) const
{
    /*out << m_start;
    out << m_showRect;
    out << m_doc->toHtml();
    out << opacity();
    out << m_color;
    out << m_id;
    // out << m_rect;
    // out << m_penWidth;
    out << scale();
    out << rotation();
    out << pos();
    // out << zValue();
    // out << static_cast<int>(m_layer);*/
}

void TextItem::readData(QDataStream& in)
{
    /* in >> m_start;
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
     // in >> m_rect;
     // in >> m_penWidth;

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
     in >> i;*/
    // m_layer= static_cast<Core::Layer>(i);
}
void TextItem::fillMessage(NetworkMessageWriter* msg)
{
    /*  msg->string16(m_id);
      msg->uint8(static_cast<quint8>(m_showRect));
      msg->real(scale());
      msg->real(rotation());
      // msg->uint8(static_cast<quint8>(m_layer));
      msg->real(zValue());
      msg->real(opacity());

      // m_rect
      / *   msg->real(m_rect.x());
         msg->real(m_rect.y());
         msg->real(m_rect.width());
         msg->real(m_rect.height());* /

      // msg->uint16(m_penWidth);

      // pos
      msg->real(pos().x());
      msg->real(pos().y());
      // center
      msg->real(m_start.x());
      msg->real(m_start.y());
      msg->string32(m_doc->toHtml());
      msg->rgb(m_color.rgb());
      msg->uint8(static_cast<quint8>(m_showRect));*/
}
void TextItem::readItem(NetworkMessageReader* msg)
{
    /* blockSignals(true);
     m_id= msg->string16();
     m_showRect= static_cast<bool>(msg->uint8());
     setScale(msg->real());
     setRotation(msg->real());
     // m_layer= static_cast<Core::Layer>(msg->uint8());
     setZValue(msg->real());
     setOpacity(msg->real());

     // m_rect
     / *  m_rect.setX(msg->real());
       m_rect.setY(msg->real());
       m_rect.setWidth(msg->real());
       m_rect.setHeight(msg->real());* /

     // m_penWidth= msg->uint16();

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
     m_textItem->setDefaultTextColor(m_color);*/

    update();
}
VisualItem* TextItem::getItemCopy()
{
    // TextItem* rectItem= new TextItem(m_start, m_penWidth, m_color);
    return nullptr; // rectItem;
}
void TextItem::addActionContextMenu(QMenu& menu)
{
    QAction* edit= menu.addAction(tr("Edit Text…"));
    connect(edit, SIGNAL(triggered(bool)), this, SLOT(editText()));

    QAction* adapt= menu.addAction(tr("Adapt to content"));
    connect(adapt, SIGNAL(triggered(bool)), this, SLOT(sizeToTheContent()));

    QMenu* state= menu.addMenu(tr("Font Size"));
    state->addAction(m_increaseFontSize.get());
    state->addAction(m_decreaseFontSize.get());
}

void TextItem::sizeToTheContent()
{
    QRectF rectItem= m_textItem->boundingRect();
    /*  setTransformOriginPoint(m_rect.center());
      if(rectItem.height() < m_rect.height() + 10)
      {
          m_rect.setHeight(rectItem.height() + 10);
          m_resizing= true;
      }
      if(rectItem.width() < m_rect.width() + 10)
      {
          m_rect.setWidth(rectItem.width() + 10);
          m_resizing= true;
      }*/
    if(m_resizing)
    {
        endOfGeometryChange();
    }
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
            /*Core::SelectableTool tool= vmap->getSelectedtool();
            if((Core::TEXT == tool) || (Core::TEXTBORDER == tool))
            {
                QApplication::setOverrideCursor(QCursor(Qt::IBeamCursor));
            }*/
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
