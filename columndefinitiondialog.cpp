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


#include "ui_columndefinitiondialog.h"
#include "borderlisteditor.h"
#include "tablecanvasfield.h"
#include "delegate/alignmentdelegate.h"
#include "delegate/typedelegate.h"
#include "delegate/fontdelegate.h"
#include "delegate/pagedelegate.h"


ColumnDefinitionDialog::ColumnDefinitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnDefinitionDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Table Properties"));
    m_model = new FieldModel(this);
    ui->m_column2Field->setFieldModel(m_model);
    ui->m_column2Field->setCurrentPage(nullptr);
    ui->m_column2Field->setCanvasList(nullptr);
    ui->m_column2Field->setUndoStack(new QUndoStack(this));

    connect(ui->m_columnCountEdit,SIGNAL(valueChanged(int)),this,SIGNAL(columnCountChanged(int)));
    connect(ui->m_lineCountEdit,SIGNAL(valueChanged(int)),this,SIGNAL(lineCountChanged(int)));
    connect(ui->m_controlPositionCb,SIGNAL(currentIndexChanged(int)),this,SIGNAL(positionChanged(int)));
}

ColumnDefinitionDialog::~ColumnDefinitionDialog()
{
    delete ui;
}

void ColumnDefinitionDialog::setData(QList<HandleItem *> list,qreal widthTotal, int line, qreal height)
{
    qreal currentX = 0;
    qreal currentY = height/line;
    m_model->clearModel();
    int i = 0;
    for(auto handle : list)
    {
        auto field = new Field();
        field->setCurrentType(CharacterSheetItem::TEXTINPUT);
        m_model->appendField(field);
        field->setX(currentX);
        field->setY(i*currentY);
        field->setWidth(handle->pos().x()-currentX);
        field->setHeight(currentY);
        currentX  = handle->pos().x();
        ++i;
    }
    auto field = new Field();
    field->setCurrentType(CharacterSheetItem::TEXTINPUT);
    m_model->appendField(field);
    field->setX(currentX);
    field->setY(i*currentY);
    field->setWidth(widthTotal-currentX);
    field->setHeight(currentY);


    ui->m_columnCountEdit->setValue(list.size()+1);
    ui->m_lineCountEdit->setValue(line);
}

FieldModel* ColumnDefinitionDialog::model() const
{
    return m_model;
}

void ColumnDefinitionDialog::setModel(FieldModel *model)
{
    m_model = model;
}
void ColumnDefinitionDialog::load(QJsonObject& json, EditorController* ctrl)
{
    m_model->load(json, ctrl);
}
void ColumnDefinitionDialog::save(QJsonObject &json)
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
