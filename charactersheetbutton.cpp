/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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
#include "charactersheetbutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QJsonArray>
#include <QJsonObject>

CharacterSheetButton::CharacterSheetButton(QPointF topleft,QGraphicsItem* parent)
: CSItem(parent)
{
    m_rect.setTopLeft(topleft);
    m_rect.setBottomRight(topleft);
    m_bgColor = QColor(Qt::red);
    m_textColor = QColor(Qt::white);
    m_value = QStringLiteral("value %1").arg(m_count);
    init();
}
CharacterSheetButton::CharacterSheetButton(QGraphicsItem* parent)
: CSItem(parent)
{
    init();
}
void CharacterSheetButton::init()
{
    ++m_count;
    m_id = QStringLiteral("id_%1").arg(m_count);
    setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);


    connect(this,&CharacterSheetButton::xChanged,[=](){
        emit updateNeeded(this);
    });
    connect(this,&CharacterSheetButton::yChanged,[=](){
        emit updateNeeded(this);
    });
}
QRectF CharacterSheetButton::boundingRect() const
{
    return m_rect;
}
QVariant CharacterSheetButton::getValueFrom(CharacterSheetItem::ColumnId id,int role) const
{
    switch(id)
    {
    case ID:
        return m_id;
    case LABEL:
        return m_label;
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
        if(role == Qt::DisplayRole)
        {
            return m_bgColor.name(QColor::HexArgb);
        }
        else
        {
            return m_bgColor;
        }
    case TEXT_ALIGN:
        return QVariant();
    case BGCOLOR:
        return m_bgColor.name(QColor::HexArgb);
    case TEXTCOLOR:
        return m_textColor.name(QColor::HexArgb);
    case TYPE:
        return getCurrentType();
    }
    return QVariant();
}

void CharacterSheetButton::setValueFrom(CharacterSheetItem::ColumnId id, QVariant var)
{
    switch(id)
    {
    case ID:
        m_id = var.toString();
        break;
    case LABEL:
        m_label = var.toString();
        break;
    case VALUE:
         m_value = var.toString();
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
        break;
    case BGCOLOR:
        m_bgColor= var.value<QColor>();
        break;
    case TEXTCOLOR:
        m_textColor= var.value<QColor>();
        break;
    }
    update();
}
void CharacterSheetButton::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();


    painter->fillRect(m_rect,m_bgColor);
    painter->drawText(m_rect,Qt::AlignCenter,m_value);

    painter->restore();


}
void CharacterSheetButton::setNewEnd(QPointF nend)
{
    m_rect.setBottomRight(nend);
}

void CharacterSheetButton::save(QJsonObject &json, bool exp)
{
    json["type"]="button";
    json["id"]=m_id;
    json["value"]=m_value;
    json["label"]=m_label;
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
    json["x"]=m_rect.x();
    json["y"]=m_rect.y();
    json["width"]=m_rect.width();
    json["height"]=m_rect.height();

    json["page"]=m_page;

}

void CharacterSheetButton::load(QJsonObject &json, QList<QGraphicsScene*> scene)
{

    m_id = json["id"].toString();
    m_value = json["value"].toString();

    m_label = json["label"].toString();

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

    qreal x,y,w,h;
    x=json["x"].toDouble();
    y=json["y"].toDouble();
    w=json["width"].toDouble();
    h=json["height"].toDouble();

    m_rect.setRect(x,y,w,h);

    m_page = json["page"].toInt();


    update();

}
void CharacterSheetButton::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec)
{
    if(sec==CharacterSheetItem::FieldSec)
    {
        out << "DiceButton {\n";
        out << "    id:_"<<m_id<< "\n";
        out << "    text: "<<m_id<<".label\n";
        out << "    textColor:\""<< m_textColor.name(QColor::HexArgb) <<"\"\n";
        out << "    x:" << m_rect.x() << "*parent.realscale"<<"\n";
        out << "    y:" << m_rect.y()<< "*parent.realscale"<<"\n";
        out << "    width:" << m_rect.width() <<"*parent.realscale"<<"\n";
        out << "    height:"<< m_rect.height()<<"*parent.realscale"<<"\n";
        out << "    color: \"" << m_bgColor.name(QColor::HexArgb)<<"\"\n";
        out << "    onClicked:rollDiceCmd("<<m_id<<".value)\n";
        out << "}\n";
    }

}

CharacterSheetItem::TypeField CharacterSheetButton::getCurrentType()const
{
    return Field::BUTTON;
}
CharacterSheetItem* CharacterSheetButton::getChildAt(QString) const
{
    return nullptr;
}

CharacterSheetItem::CharacterSheetItemType CharacterSheetButton::getItemType() const
{
    return CharacterSheetItem::ButtonItem;
}

void CharacterSheetButton::saveDataItem(QJsonObject &json)
{
    json["type"]="button";
    json["id"]=m_id;
    json["label"]=m_label;
    json["value"]=m_value;
}

void CharacterSheetButton::loadDataItem(QJsonObject &json)
{
    m_id = json["id"].toString();
    m_value= json["value"].toString();
    m_label = json["label"].toString();
}
void CharacterSheetButton::copyField(CharacterSheetItem* newBtn)
{
    CharacterSheetButton* newField = dynamic_cast<CharacterSheetButton*>(newBtn);
    if(nullptr!=newField)
    {
        setId(newField->getId());
        //setValue(newField->value());
        setRect(newField->getRect());
        setBgColor(newField->bgColor());
        setTextColor(newField->textColor());
        setLabel(newField->getLabel());
    }
}

