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
#include <QApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFont>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QStyle>
#include <QVBoxLayout>

#include <QGraphicsSceneWheelEvent>
#include <QStyleOptionGraphicsItem>

#include "controller/item_controllers/textcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "editors/mrichtextedit.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "customs/vmap.h"

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
    , m_increaseFontSize(new QAction(tr("Increase Text Size")))
    , m_decreaseFontSize(new QAction(tr("Decrease Text Size")))
    , m_edit(new QAction(tr("Edit Text…")))
    , m_adapt(new QAction(tr("Adapt to content")))
{
    if(nullptr == m_dialog)
        m_dialog= new RichTextEditDialog();

    connect(m_edit.get(), &QAction::triggered, this, &TextItem::editText);
    connect(m_adapt.get(), &QAction::triggered, this, &TextItem::sizeToTheContent);

    connect(m_increaseFontSize.get(), &QAction::triggered, m_textCtrl, &vmap::TextController::increaseFontSize);
    connect(m_decreaseFontSize.get(), &QAction::triggered, m_textCtrl, &vmap::TextController::decreaseFontSize);

    m_textItem->setDocument(m_doc.get());
    connect(m_textCtrl, &vmap::TextController::borderRectChanged, this,
            [this]()
            {
                m_textItem->setTextWidth(m_textCtrl->borderRect().width());
                m_textCtrl->setTextRect(m_textItem->boundingRect());
                updateChildPosition();
            });
    connect(m_textCtrl, &vmap::TextController::textRectChanged, this,
            [this]() { m_textItem->setTextWidth(m_textCtrl->textRect().width()); });
    connect(m_textCtrl, &vmap::TextController::textPosChanged, this,
            [this](const QPointF& pos) { m_textItem->setPos(pos); });

    connect(m_textCtrl, &vmap::TextController::editableChanged, this,
            [this]() { m_textItem->setEnabled(m_textCtrl->editable()); });

    connect(m_textCtrl, &vmap::TextController::textChanged, m_doc.get(), &QTextDocument::setPlainText);
    connect(m_doc.get(), &QTextDocument::contentsChanged, this,
            [this]()
            {
                if(!m_textCtrl)
                    return;
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
    m_textItem->setEnabled(m_textCtrl->editable());
    if(m_textCtrl)
        m_textItem->setDefaultTextColor(m_textCtrl->color());
    m_doc->setPlainText(tr("Text"));
}

QRectF TextItem::boundingRect() const
{
    if(!m_textCtrl)
        return {};
    return m_textCtrl->borderRect();
}
void TextItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(!m_textCtrl)
        return;
    setChildrenVisible(hasFocusOrChild());
    if(m_textCtrl->border())
    {
        QPen pen= painter->pen();
        pen.setColor(m_textCtrl->color());
        pen.setWidth(m_textCtrl->border() ? m_textCtrl->penWidth() : 2);
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }
    if(canBeMoved() && (option->state & QStyle::State_MouseOver || isSelected()))
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(isSelected() ? m_selectedColor : m_highlightColor);
        pen.setWidth(m_highlightWidth);
        painter->setPen(pen);
        painter->drawRect(m_textCtrl->borderRect());
        painter->restore();
    }
}

void TextItem::setNewEnd(const QPointF& p)
{
    Q_UNUSED(p)
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
    if(!m_textCtrl)
        return;

    if(!event->modifiers() & Qt::ControlModifier)
        VisualItem::wheelEvent(event);

    event->delta() > 0 ? m_textCtrl->increaseFontSize() : m_textCtrl->decreaseFontSize();
    event->accept();
}

void TextItem::updateChildPosition()
{
    if(!m_textCtrl)
        return;
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
        emit itemGeometryChanged(this);
    }
}

/*void TextItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change == ChildPointItem::Resizing)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(m_textCtrl->borderRect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_textCtrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                                   oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    VisualItem::endOfGeometryChange(change);
}*/

void TextItem::addActionContextMenu(QMenu& menu)
{
    menu.addAction(m_edit.get());
    menu.addAction(m_adapt.get());

    QMenu* state= menu.addMenu(tr("Font Size"));
    state->addAction(m_increaseFontSize.get());
    state->addAction(m_decreaseFontSize.get());
    VisualItem::addActionContextMenu(menu);
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
    /*if(m_resizing)
    {
        endOfGeometryChange(ChildPointItem::Resizing);
    }*/
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
