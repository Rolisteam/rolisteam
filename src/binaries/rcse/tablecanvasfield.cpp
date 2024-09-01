/***************************************************************************
 * Copyright (C) 2018 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "tablecanvasfield.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QComboBox>
#include <QPainter>
#include <QStyle>
#include <QMetaEnum>
#include <QStyleOptionGraphicsItem>
#include <QInputDialog>

#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/tablefield.h"
#define SQUARE_SIZE 12

bool compareHandles(HandleItem* first, HandleItem* two)
{
    return (first->pos().x() < two->pos().x());
}

bool isValid(int listSize, int index)
{
    if(listSize == 0 || index < 0 || index >= listSize )
    {
        qWarning() << "Number of columns does not fit the index:" << index << listSize;
        return false;
    }
    return true;
}

TableCanvasField::TableCanvasField(TableFieldController* field)
    : CanvasField(field),m_ctrl(field),m_addColumn{new ButtonCanvas},m_addLine{new ButtonCanvas}
{

    m_addColumn->setParentItem(this);
    m_addLine->setParentItem(this);
    using cs = CSItem::TypeField;

    m_typeToText = { {  cs::TEXTINPUT,tr("TextInput")},
                   {   cs::TEXTFIELD,tr("TextField")},
                   {   cs::TEXTAREA,tr("TextArea")},
                   {   cs::SELECT,tr("Select")},
                   {   cs::CHECKBOX,tr("CheckBox")},
                   {   cs::IMAGE,tr("Image")},
                   {   cs::RLABEL,tr("Label")},
                   {   cs::DICEBUTTON,tr("Dice Button")},
                   {   cs::FUNCBUTTON,tr("Function Button")},
                   {   cs::WEBPAGE,tr("Web page")},
                   {  cs::SLIDER,tr("Slider")}};

    auto func = [this](bool add, int index){
        if(add)
            addColumn(index);
        else
            removeColumn(index);
    };
    connect(m_ctrl, &TableFieldController::columnCountChanged, this, func);

    for(int i = 0; i < m_ctrl->columnCount(); ++i)
    {
        func(true, i);
    }


    auto updateColGeometry = [this](){

        auto model = m_ctrl->model();
        if(!model)
            return;
        auto cols = model->columns();
        if(m_columnSelector.size()<=0)
            return;
        bool add = m_columnSize <= cols.size();
        auto wCols = std::accumulate(std::begin(cols), std::end(cols), 0., [](qreal b, FieldController* col){
            return b + col->width();
        });

        if(add)
        {
            qreal wStep = 1./cols.size();
            auto offsetX=0;
            for(auto & col : cols)
            {
                //qDebug() << "table canvas set width in for loop" << col->width();
                if(qFuzzyCompare(col->width(), 0.))
                {
                    col->setWidth(m_ctrl->width() * wStep);
                }
                else
                {

                    col->setWidth(col->width() * (1.-wStep));
                }
                col->setHeight(20);
                col->setX(offsetX);
                offsetX = col->x() + col->width();
                col->setY(0);
            }
        }
        else if(m_ctrl->width() > 0)
        {

            auto space = m_ctrl->width() - wCols;
            auto offset = 0;
            for(auto& col : cols)
            {
                auto x = col->width()/wCols;
                col->setWidth(col->width()+x*space);
                col->setX(offset);
                offset+= col->width();
            }
        }
        m_columnSize=cols.size();
    };

    connect(m_ctrl, &TableFieldController::widthChanged, this, updateColGeometry);
    connect(m_ctrl, &TableFieldController::columnCountChanged, this, updateColGeometry);
    connect(m_ctrl, &TableFieldController::displayedRowChanged, this, [this](){
        update();
    });

    auto m = m_ctrl->model();
    if(m && m->columns().isEmpty())
        updateColGeometry();

    m_addLine->setMsg("+");
    m_addColumn->setMsg("+");

    connect(m_addColumn.get(), &ButtonCanvas::clicked, m_ctrl, &TableFieldController::addColumn);
    connect(m_addLine.get(), &ButtonCanvas::clicked, m_ctrl, &TableFieldController::addLine);
    connect(m_ctrl.get(), &TableFieldController::requestUpdate, this, [this](){update();});

    if(nullptr == m_ctrl->model())
        m_ctrl->init();
}
TableCanvasField::~TableCanvasField() {}


void TableCanvasField::setMenu(QMenu& menu)
{
  //menu.addAction(m_defineColumns);
}

void TableCanvasField::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    if(nullptr == m_ctrl)
        return;

    auto rect= boundingRect();
    painter->save();
    painter->fillRect(rect, m_ctrl->bgColor());
    painter->setPen(Qt::black);
    painter->drawRect(rect);

    m_addLine->setPos(-m_addLine->boundingRect().width(), boundingRect().height() / 2);
    m_addColumn->setPos((boundingRect().width()-m_addColumn->boundingRect().width()) / 2, -m_addColumn->boundingRect().height());

    if(hasFocusOrChild())
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(Qt::red);
        pen.setWidth(5);
        painter->setPen(pen);
        painter->drawRect(rect);
        painter->restore();
    }

    auto yStep= boundingRect().height() / (m_ctrl->rowCount());
    auto model = m_ctrl->model();
    auto cols = model->columns();

    int i = 0;
    for(auto const& col : cols)
    {
        auto const& widget = m_columnSelector[i];
        auto const& remove = m_columnRemover[i];
        auto const& handle = m_handles[i];

        //qDebug() << "Field parents: " <<widget->parentItem() << remove->parentItem() << handle->parentItem();

        auto w = col->width();
        if(qFuzzyCompare(w, 0))
        {
            w = m_ctrl->width()/cols.size();
        }
        widget->setRect({-w/2, col->height()/2, w, col->height()});
        widget->setPos(col->x()+w/2, 20);
        widget->setOpacity(0.6);
        widget->setVisible(true);

        remove->setVisible(cols.size()>1);
        remove->setPos(col->x()+w/2-remove->boundingRect().width()/2, m_ctrl->height()-remove->boundingRect().height());

        handle->setVisible(i!=0 && hasFocusOrChild());
        handle->setPos(col->x(), m_ctrl->height()/2-handle->boundingRect().height()/2);

        if(i > 0)
            painter->drawLine(col->x(), 0, col->x(), boundingRect().height() );

        ++i;
    }

    for(int i= 0; i < m_ctrl->displayedRow(); ++i)
    {
        painter->drawLine(0, i * yStep, m_ctrl->width(), i * yStep);
    }
    painter->drawText(QPoint(0, 0), m_ctrl->id());
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
        for(int i= 0; i < m_handles.size(); ++i)
        {
            if((m_handles.at(i) != nullptr) && (m_handles.at(i)->isSelected()))
            {
                return true;
            }
        }
    }

    return false;
}

void TableCanvasField::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "table canvas event " << event->scenePos() << " " << event->pos();
    QGraphicsObject::mousePressEvent(event);
}

void TableCanvasField::addColumn(int index)
{
    using cs = CSItem::TypeField;
    auto const& model = m_ctrl->model();
    auto const& cols = model->columns();

    if(!isValid(cols.size(), index))
        return;

    auto const& col = cols[index];
    auto type = col->fieldType();
    auto key=  m_typeToText.value(type);

    auto wid = new ButtonCanvas();
    auto remover = new ButtonCanvas();
    auto handler = new HandleItem();

    connect(col, &CSItem::fieldTypeChanged, this, [this](){
        auto field = qobject_cast<FieldController*>(sender());

        if(!field)
            return;

        auto m = m_ctrl->model();
        auto const& cols = m->columns();
        auto index = cols.indexOf(field);
        auto const& selector = m_columnSelector[index];

        selector->setMsg(m_typeToText.value(field->fieldType()));
    });


    handler->setParentItem(this);
    m_handles.append(handler);
    connect(handler, &HandleItem::xChanged,col, [this, handler](){
        auto index = m_handles.indexOf(handler);;
        updateColumnSize(index, handler->x());
    });

    remover->setMsg("-");
    remover->setParentItem(this);

    connect(remover, &ButtonCanvas::clicked, this, [this](){
        auto remover = qobject_cast<ButtonCanvas*>(sender());
        auto it = std::find_if(std::begin(m_columnRemover), std::end(m_columnRemover), [remover](const std::unique_ptr<ButtonCanvas>& ptr)
                              {
                                  return remover == ptr.get();
                              });
        if(it == std::end(m_columnRemover))
            return;

        m_ctrl->removeColumn(std::distance(std::begin(m_columnRemover), it));
    });

    connect(wid, &ButtonCanvas::clicked, this, [this](){
        auto wid = qobject_cast<ButtonCanvas*>(sender());

        bool ok;
        auto vals = m_typeToText.values();
        vals.sort();
        qDebug() << vals << wid->msg() << vals.indexOf(wid->msg());
        QString item = QInputDialog::getItem(nullptr, tr("Select the column type"),
                                            tr("Type of Field:"),vals , vals.indexOf(wid->msg()), false, &ok);

        if(!ok)
            return;

        auto const& model = m_ctrl->model();
        auto const& cols = model->columns();

        auto it = std::find_if(std::begin(m_columnSelector), std::end(m_columnSelector), [wid](const std::unique_ptr<ButtonCanvas>& ptr)
                              {
                                  return wid == ptr.get();
                              });

        if(it == std::end(m_columnSelector))
            return;

        auto index = std::distance(std::begin(m_columnSelector), it);

        if(!isValid(cols.size(), index))
            return;

        auto const& col = cols[index];


        auto typeval = m_typeToText.key(item);
        col->setFieldType(static_cast<cs>(typeval));
        wid->setMsg(item);
    });
    wid->setMsg(key);
    wid->setParentItem(this);
    wid->setVisible(false);

    m_columnSelector.push_back(std::unique_ptr<ButtonCanvas>(wid));
    m_columnRemover.push_back(std::unique_ptr<ButtonCanvas>(remover));
}
void TableCanvasField::removeColumn(int index)
{
    m_columnSelector.erase(std::begin(m_columnSelector)+index);
    m_columnRemover.erase(std::begin(m_columnRemover)+index);
    auto h = m_handles.at(index);
    m_handles.removeAt(index);
    h->deleteLater();
}


void TableCanvasField::updateColumnSize(int index, qreal pos)
{
    auto const& model = m_ctrl->model();
    auto const& cols = model->columns();
    if(!isValid(cols.size(), index) || !isValid(cols.size(), index-1))
        return;

    auto col = cols.at(index);
    auto pcol = cols.at(index-1);
    auto handler = m_handles.at(index);

    auto change = pos-col->x();
    col->setX(handler->x());
    col->setWidth(col->width()-change);
    pcol->setWidth(pcol->width()+change);

}


ButtonCanvas::ButtonCanvas() : m_rect(QRectF(-SQUARE_SIZE, -SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
}

QRectF ButtonCanvas::boundingRect() const
{
    return m_rect;
}

void ButtonCanvas::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->save();
    painter->fillRect(boundingRect(), Qt::cyan);
    painter->save();
    auto p = painter->pen();
    p.setColor(hasFocus() ? Qt::red : Qt::black);
    p.setWidth(2);
    painter->setPen(p);
    painter->drawRect(boundingRect());
    painter->restore();
    painter->drawText(boundingRect(), Qt::AlignCenter, m_msg);
    painter->restore();
}

QString ButtonCanvas::msg() const
{
    return m_msg;
}

void ButtonCanvas::setMsg(const QString& msg)
{
    if(msg == m_msg)
        return;
    m_msg= msg;
    update();
}

void ButtonCanvas::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mousePressEvent(event);

    if(!boundingRect().contains(event->pos()))
    {
        event->ignore();
        return;
    }

    auto p = parentItem();
    qDebug() << "button pressed1" << m_msg << event->scenePos() << pos() << boundingRect() << this << p << hasFocus();
    if(!p || p->boundingRect().isEmpty())
        return;

    qDebug() << "button pressed2" << m_msg << event->isAccepted() << boundingRect() << parentItem()->boundingRect() << event->buttons();
    if(event->buttons() & Qt::LeftButton )//&& event->isAccepted()
    {
        qDebug() << "button pressed5 inside" << event->pos();
        if(boundingRect().contains(event->pos()))
        {
            qDebug() << event->type() << "MousePressEvent";
            emit clicked();
        }
    }
}

QRectF ButtonCanvas::rect() const
{
    return m_rect;
}

void ButtonCanvas::setRect(const QRectF& rect)
{
    if(rect == m_rect)
        return;
    m_rect= rect;
    update();
}
