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
#include "columndefinitiondialog.h"
#include <QUndoStack>

#include "borderlisteditor.h"
#include "delegate/alignmentdelegate.h"
#include "delegate/fontdelegate.h"
#include "delegate/pagedelegate.h"
#include "delegate/typedelegate.h"
#include "tablecanvasfield.h"
#include "ui_columndefinitiondialog.h"

#include <QJsonArray>
#include <QPainter>

#define SQUARE_SIZE 12

HandleItem::HandleItem(QGraphicsObject* parent) : QGraphicsObject(parent)
{
    m_currentMotion= X_AXIS;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsMovable
             | QGraphicsItem::ItemIsFocusable);
}

HandleItem::~HandleItem() {}

QVariant HandleItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if(change == ItemPositionChange && isSelected())
    {
        QPointF newPos= value.toPointF();
        if(m_currentMotion == X_AXIS)
        {
            newPos.setY(pos().y());
        }
        else if(Y_AXIS == m_currentMotion)
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
    return QRectF(0, 0, SQUARE_SIZE, SQUARE_SIZE);
}

void HandleItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(Qt::black);
    painter->fillRect(m_startPoint.x(), m_startPoint.y(), SQUARE_SIZE, SQUARE_SIZE, Qt::gray);
    painter->drawRect(m_startPoint.x(), m_startPoint.y(), SQUARE_SIZE, SQUARE_SIZE);
}

void HandleItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseMoveEvent(event);
}

void HandleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseReleaseEvent(event);
}
void HandleItem::load(QJsonObject& json)
{
    m_currentMotion= static_cast<MOTION>(json["motion"].toInt());
    qreal x= json["x"].toDouble();
    qreal y= json["y"].toDouble();

    qreal posx= json["posx"].toDouble();
    qreal posy= json["posy"].toDouble();

    setPos(posx, posy);
    m_posHasChanged= json["haschanged"].toBool();

    m_startPoint.setX(x);
    m_startPoint.setY(y);
}
void HandleItem::save(QJsonObject& json)
{
    json["motion"]= static_cast<int>(m_currentMotion);
    json["x"]= m_startPoint.x();
    json["y"]= m_startPoint.y();
    json["posx"]= pos().x();
    json["posy"]= pos().y();
    json["haschanged"]= m_posHasChanged;
}
//////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////
ColumnDefinitionDialog::ColumnDefinitionDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ColumnDefinitionDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Table Properties"));
    m_model= new FieldModel(this);
    ui->m_column2Field->setFieldModel(m_model);
    ui->m_column2Field->setCurrentPage(0);
    ui->m_column2Field->setCanvasList(nullptr);
    // ui->m_column2Field->setUndoStack(new QUndoStack(this));

    connect(ui->m_columnCountEdit, SIGNAL(valueChanged(int)), this, SIGNAL(columnCountChanged(int)));
    connect(ui->m_lineCountEdit, SIGNAL(valueChanged(int)), this, SIGNAL(lineCountChanged(int)));
    connect(ui->m_controlPositionCb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(positionChanged(int)));
}

ColumnDefinitionDialog::~ColumnDefinitionDialog()
{
    delete ui;
}

void ColumnDefinitionDialog::setData(QList<HandleItem*> list, qreal widthTotal, int line, qreal height)
{
    qreal currentX= 0;
    qreal currentY= height / line;
    m_model->clearModel();
    int i= 0;
    for(auto handle : list)
    {
        auto field= new FieldController(CharacterSheetItem::FieldItem, true);
        field->setCurrentType(CharacterSheetItem::TEXTINPUT);
        m_model->appendField(field);
        field->setX(currentX);
        field->setY(i * currentY);
        field->setWidth(handle->pos().x() - currentX);
        field->setHeight(currentY);
        currentX= handle->pos().x();
        ++i;
    }
    auto field= new FieldController(CharacterSheetItem::FieldItem, true);
    field->setCurrentType(CharacterSheetItem::TEXTINPUT);
    m_model->appendField(field);
    field->setX(currentX);
    field->setY(i * currentY);
    field->setWidth(widthTotal - currentX);
    field->setHeight(currentY);

    ui->m_columnCountEdit->setValue(list.size() + 1);
    ui->m_lineCountEdit->setValue(line);
}

FieldModel* ColumnDefinitionDialog::model() const
{
    return m_model;
}

void ColumnDefinitionDialog::setModel(FieldModel* model)
{
    m_model= model;
}
void ColumnDefinitionDialog::load(QJsonObject& json)
{
    m_model->load(json);
}
void ColumnDefinitionDialog::save(QJsonObject& json)
{
    m_model->save(json);
}
/*void ColumnDefinitionDialog::positionChanged(int i)
{

}
void ColumnDefinitionDialog::lineCountChanged(int i)
{

}
void ColumnDefinitionDialog::columnCountChanged(int i)
{

}*/
