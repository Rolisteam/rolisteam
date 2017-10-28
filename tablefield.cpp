/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
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
#include "tablefield.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonArray>
#include <QUuid>
#include <QDebug>
#include "field.h"

#ifndef RCSE
TableCanvasField::TableCanvasField()
{

}
#endif
//////////////////////////////////////////
/// @brief LineFieldItem::createLineItem
/// @return
//////////////////////////////////////////



LineFieldItem::LineFieldItem(QObject *parent)
    : QObject(parent)
{

}

LineFieldItem::~LineFieldItem()
{

}

void LineFieldItem::insertField(Field *field)
{
    m_fields.append(field);
}

Field* LineFieldItem::getField(int k) const
{
    return m_fields.at(k);
}

QList<Field *> LineFieldItem::getFields() const
{
    return m_fields;
}

void LineFieldItem::setFields(const QList<Field *> &fields)
{
    m_fields = fields;
}

////////////////////////////////////////
//
////////////////////////////////////////
LineModel::LineModel()
{

}
int LineModel::rowCount(const QModelIndex& parent) const
{
    return m_lines.size();
}

QVariant LineModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto item = m_lines.at(index.row());

    if(role == LineRole)
    {

        return QVariant::fromValue<LineFieldItem*>(item);
    }
    else
    {
        int key = role - (LineRole+1);
        return QVariant::fromValue<Field*>(item->getField(key));
    }
    return QVariant();
}

QHash<int, QByteArray>  LineModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LineRole] = "line";
    int i = 1;
    auto first = m_lines.first();
    for(auto fieldLine : first->getFields() )
    {
        roles[LineRole+i]=fieldLine->getId().toUtf8();
        i++;
        roles[LineRole+i]=fieldLine->getLabel().toUtf8();
        i++;
    }
    return roles;
}
void LineModel::generateAllField(int count)
{
    LineFieldItem* Firstline = m_lines.first();
    while(m_lines.size()<count)
    {
        //LineFieldItem* line = new LineFieldItem(&Firstline);
       // m_lines.append(line);
    }
}

///////////////////////////////////
/// \brief TableField::TableField
/// \param addCount
/// \param parent
///////////////////////////////////



TableField::TableField(bool addCount,QGraphicsItem* parent)
    : Field(addCount,parent)
{
    init();
}

TableField::TableField(QPointF topleft,bool addCount,QGraphicsItem* parent)
    : Field(topleft,addCount,parent)
{
    Q_UNUSED(topleft);
    m_value = QStringLiteral("value");
    init();
}
TableField::~TableField()
{
#ifdef RCSE
    if(nullptr!=m_tableCanvasField)
    {
        delete m_tableCanvasField;
    }
    m_canvasField = nullptr;
    m_tableCanvasField = nullptr;
#endif
}

LineModel *TableField::getModel() const
{
    return m_model;
}

void TableField::addLine()
{

}

void TableField::removeLine(int)
{

}
void TableField::init()
{
#ifdef RCSE
    m_tableCanvasField = new TableCanvasField(this);
    m_canvasField = m_tableCanvasField;
#else
    m_canvasField = nullptr;
    m_tableCanvasField = nullptr;
#endif
    m_id = QStringLiteral("id_%1").arg(m_count);
    m_currentType=Field::TABLE;
    m_clippedText = false;


    m_border=NONE;
    m_textAlign = Field::TopLeft;
    m_bgColor = Qt::transparent;
    m_textColor = Qt::black;
    m_font = font();
#ifdef RCSE
    if(nullptr!=m_canvasField)
    {
        m_canvasField->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemClipsToShape);

        connect(m_canvasField,&CanvasField::xChanged,[=](){
            emit updateNeeded(this);
        });
        connect(m_canvasField,&CanvasField::yChanged,[=](){
            emit updateNeeded(this);
        });
    }
#endif
}

int TableField::getRowCount() const
{
    return m_rowCount;
}

void TableField::setRowCount(int rowCount)
{
    m_rowCount = rowCount;
}
void TableField::makeLines()
{
   m_model->generateAllField(m_rowCount);
}
bool TableField::mayHaveChildren() const
{
    return true;
}
void TableField::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec,int i, bool isTable)
{
    if(nullptr==m_tableCanvasField)
    {
        return;
    }
    if(sec==CharacterSheetItem::FieldSec)
    {
        out << "    Column{//"<< m_label <<"\n";
        out << "        Repeater{\n";
        out << "            model:"<< m_id << "model\n";
        out << "            Row {";
        //m_tableCanvasField->generateSubFields(out);
        out << "            }";
        out << "        }";
        out << "    }";

    }
}

