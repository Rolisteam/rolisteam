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

#ifndef RCSE
TableCanvasField::TableCanvasField()
{

}
#endif

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
void TableField::init()
{
#ifdef RCSE
    m_tableCanvasField = new TableCanvasField(this);
    m_canvasField = m_tableCanvasField;
#else
    m_canvasField = nullptr;
    m_tableCanvasField = nullptr;
#endif

    m_lines = new QQmlObjectListModel<LineFieldItem>();
    m_id = QStringLiteral("id_%1").arg(m_count);
    m_currentType=Field::TABLE;
    m_clippedText = false;


    m_border=NONE;
    m_textAlign = Field::TopLeft;
    m_bgColor = Qt::transparent;
    m_textColor = Qt::black;
    m_font = font();
    #ifdef RCSE
    if(NULL!=m_canvasField)
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
void TableField::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec,int i, bool isTable)
{
    if(NULL==m_tableCanvasField)
    {
        return;
    }
    if(sec==CharacterSheetItem::FieldSec)
    {
        out << "    Item{//"<< m_label <<"\n";
        out << "        Repeater{\n";
        out << "            model:"<< m_id << "Model\n";
        out << "            Row {";
        //m_tableCanvasField->generateSubFields(out);
        out << "            }";
        out << "        }";
        out << "    }";

    }
}


QObject* LineFieldItem::createLineItem()
{

}
