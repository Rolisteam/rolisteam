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
#include "field.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonArray>
#include <QUuid>

Field::Field(QGraphicsItem* parent)
: CSItem(parent)
{
 init();
}

Field::Field(QPointF topleft,QGraphicsItem* parent)
    : CSItem(parent)
{
    m_rect.setTopLeft(topleft);
    m_rect.setBottomRight(topleft);

    m_bgColor = QColor(Qt::white);
    m_textColor = QColor(Qt::black);
    m_font = font();
    m_value = QStringLiteral("value");

    init();

}
void Field::init()
{
    m_id = QStringLiteral("id_%1").arg(m_count);
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);

    m_textAlign = ALignLEFT;
    m_bgColor = Qt::transparent;
    m_textColor = Qt::black;
    connect(this,&Field::xChanged,[=](){
        emit updateNeeded(this);
    });
    connect(this,&Field::yChanged,[=](){
        emit updateNeeded(this);
    });
}

QRectF Field::boundingRect() const
{
    return m_rect;
}

QVariant Field::getValueFrom(CharacterSheetItem::ColumnId id) const
{
    switch(id)
    {
    case ID:
        return m_id;
    case VALUE:
        return m_value;
    case X:
        return m_rect.x();
    case Y:
        return m_rect.y();
    case WIDTH:
        return m_rect.width();
    case HEIGHT:
        return m_rect.height();
    case BORDER:
        return (int)m_border;
    case TEXT_ALIGN:
        return m_textAlign;
    case BGCOLOR:
        return m_bgColor.name(QColor::HexArgb);
    case TEXTCOLOR:
        return m_textColor.name(QColor::HexArgb);
    case VALUES:
        return m_availableValue.join(',');
    }
    return QVariant();
}

void Field::setValueFrom(CharacterSheetItem::ColumnId id, QVariant var)
{
    switch(id)
    {
    case ID:
        setId(var.toString());
        break;
    case VALUE:
        setValue(var.toString());
        break;
    case X:
        m_rect.setX(var.toReal());
        break;
    case Y:
        m_rect.setY(var.toReal());
        break;
    case WIDTH:
        m_rect.setWidth(var.toReal());
        break;
    case HEIGHT:
        m_rect.setHeight(var.toReal());
        break;
    case BORDER:
        m_border = (BorderLine)var.toInt();
        break;
    case TEXT_ALIGN:
        m_textAlign= (TextAlign)var.toInt();
        break;
    case BGCOLOR:
        m_bgColor= var.value<QColor>();
        break;
    case TEXTCOLOR:
        m_textColor= var.value<QColor>();
        break;
    case VALUES:
        m_availableValue = var.toString().split(',');
        break;
    }
    update();
}
void Field::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();


    painter->drawRect(m_rect);
    painter->drawText(m_rect,m_id);

    painter->restore();


}

void Field::drawField()
{

}

Field::BorderLine Field::border() const
{
    return m_border;
}

void Field::setBorder(const Field::BorderLine &border)
{
    m_border = border;
    drawField();
}

QFont Field::font() const
{
    return m_font;
}

void Field::setFont(const QFont &font)
{
    m_font = font;
    drawField();
}

void Field::mousePressEvent(QMouseEvent* ev)
{
    if(ev->button() == Qt::LeftButton)
    {
       // emit clickOn(this);
    }
}
CharacterSheetItem* Field::getChildAt(QString key) const
{
    return NULL;
}
QStringList Field::getAvailableValue() const
{
    return m_availableValue;
}

void Field::setAvailableValue(const QStringList &availableValue)
{
    m_availableValue = availableValue;
}


void Field::save(QJsonObject& json,bool exp)
{
    if(exp)
    {
        json["type"]="field";
        json["id"]=m_id;
        return;
    }
    json["type"]="field";
    json["id"]=m_id;
    json["value"]=m_value;
    json["border"]=m_border;
    json["page"]=m_page;

    QJsonObject bgcolor;
    bgcolor["r"]=QJsonValue(m_bgColor.red());
    bgcolor["g"]=m_bgColor.green();
    bgcolor["b"]=m_bgColor.blue();
    bgcolor["a"]=m_bgColor.alpha();
    json["bgcolor"]=bgcolor;

    QJsonObject textcolor;
    textcolor["r"]=m_textColor.red();
    textcolor["g"]=m_textColor.green();
    textcolor["b"]=m_textColor.blue();
    textcolor["a"]=m_textColor.alpha();
    json["textcolor"]=textcolor;

    json["font"]=m_font.toString();
    json["textalign"]=m_textAlign;
    json["x"]=m_rect.x();
    json["y"]=m_rect.y();
    json["width"]=m_rect.width();
    json["height"]=m_rect.height();
    QJsonArray valuesArray;
    valuesArray=QJsonArray::fromStringList(m_availableValue);
    json["values"]=valuesArray;
}

void Field::load(QJsonObject &json,QList<QGraphicsScene*> scene)
{
    m_id = json["id"].toString();
    m_border = (BorderLine)json["border"].toInt();
    m_value= json["value"].toString();

    QJsonObject bgcolor = json["bgcolor"].toObject();
    int r,g,b,a;
    r = bgcolor["r"].toInt();
    g = bgcolor["g"].toInt();
    b = bgcolor["b"].toInt();
    a = bgcolor["a"].toInt();

    m_bgColor=QColor(r,g,b,a);

    QJsonObject textcolor = json["textcolor"].toObject();

    r = textcolor["r"].toInt();
    g = textcolor["g"].toInt();
    b = textcolor["b"].toInt();
    m_textColor=QColor(r,g,b,a);

    m_font.fromString(json["font"].toString());

    m_textAlign = (TextAlign)json["textalign"].toInt();
    qreal x,y,w,h;
    x=json["x"].toDouble();
    y=json["y"].toDouble();
    w=json["width"].toDouble();
    h=json["height"].toDouble();
    m_page=json["page"].toInt();

    QJsonArray valuesArray=json["values"].toArray();
    for(auto value : valuesArray.toVariantList())
    {
        m_availableValue << value.toString();
    }
    m_rect.setRect(x,y,w,h);

    update();
}

void Field::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec)
{
    if(sec==CharacterSheetItem::FieldSec)
    {
        out << "Field {\n";
        out << "    id:"<<m_id<< "\n";
        out << "    text: _"<<m_id << ".name\n";
        out << "    x:" << m_rect.x() << "*parent.realscale"<<"\n";
        out << "    y:" << m_rect.y()<< "*parent.realscale"<<"\n";
        out << "    width:" << m_rect.width() <<"*parent.realscale"<<"\n";
        out << "    height:"<< m_rect.height()<<"*parent.realscale"<<"\n";
        out << "    color: \"" << m_bgColor.name(QColor::HexArgb)<<"\"\n";
        out << "    visible: root.page == "<< m_page << "? true : false\n";
        if(m_availableValue.isEmpty())
        {
            out << "    state:\"field\"\n";
        }
        else
        {
            out << "    state:\"combo\"\n";
            out << "    availableValues:" << QStringLiteral("[\"%1\"]").arg(m_availableValue.join("\",\""));
        }
        out << "}\n";
    }
   /* else if(sec==CharacterSheetItem::ConnectionSec)
    {
        out << "        if(valueKey==\""<<m_name<<"\")"<<"\n";
        out << "        {"<<"\n";
        out << "            "<<m_id.toLower().trimmed()<<".text=value;"<<"\n";
        out << "        }"<<"\n";
    }*/
}







