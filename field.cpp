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
#include <QDebug>

#ifndef RCSE
CanvasField::CanvasField()
{

}
#endif

Field::Field(bool addCount,QGraphicsItem* parent)
: CSItem(parent,addCount)
{
 init();
}

Field::Field(QPointF topleft,bool addCount,QGraphicsItem* parent)
    : CSItem(parent,addCount)
{
    Q_UNUSED(topleft);
    m_hasDefaultValue = true;
    m_value = QString("value %1").arg(m_count);
    init();
}
Field::~Field()
{
    #ifdef RCSE
    if(nullptr!=m_canvasField)
        delete m_canvasField;
    m_canvasField=nullptr;
    #endif
}
void Field::init()
{
#ifdef RCSE
    m_canvasField = new CanvasField(this);
#else
    m_canvasField = NULL;
#endif
    m_id = QStringLiteral("id_%1").arg(m_count);
    m_currentType=TEXTINPUT;
    m_clippedText = false;


    m_border=NONE;
    m_textAlign = TopLeft;
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

QVariant Field::getValueFrom(CharacterSheetItem::ColumnId id,int role) const
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
        //return m_rect.x();
        if(NULL!=m_canvasField)
            return m_canvasField->pos().x();
        else
            return 0;
    case Y:
        //return m_rect.y();
        if(NULL!=m_canvasField)
            return m_canvasField->pos().y();
        else
            return 0;
    case WIDTH:
        if(NULL!=m_canvasField)
            return m_canvasField->boundingRect().width();
        else
            return 0;
    case HEIGHT:
        if(NULL!=m_canvasField)
            return m_canvasField->boundingRect().height();
        else
            return 0;
    case BORDER:
        return (int)m_border;
    case TEXT_ALIGN:
        return (int)m_textAlign;
    case BGCOLOR:
        if(role == Qt::DisplayRole)
        {
            return m_bgColor.name(QColor::HexArgb);
        }
        else
        {
            return m_bgColor;
        }
    case TEXTCOLOR:
        return m_textColor;
    case VALUES:
        return m_availableValue.join(',');
    case TYPE:
        return m_currentType;
    case CLIPPED:
        return m_clippedText;
    case FONT:
        return m_font.toString();
    case PAGE:
        return m_page;
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
    case LABEL:
        setLabel(var.toString());
        break;
    case VALUE:
        setValue(var.toString());
        break;
    case X:
        //m_rect.setX(var.toReal());
        if(NULL!=m_canvasField)
        {
            m_canvasField->setPos(var.toReal(),m_canvasField->pos().y());
        }
        break;
    case Y:
        //m_rect.setY(var.toReal());
        if(NULL!=m_canvasField)
        {
            m_canvasField->setPos(m_canvasField->pos().x(),var.toReal());
        }
        break;
    case WIDTH:
        if(NULL!=m_canvasField)
        {
            #ifdef RCSE
            m_canvasField->setWidth(var.toReal());
            #endif
        }
        break;
    case HEIGHT:
        if(NULL!=m_canvasField)
        {
            #ifdef RCSE
            m_canvasField->setHeight(var.toReal());
            #endif
        }
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
        if(var.toString().isEmpty())
        {
            m_availableValue.clear();
            m_currentType = Field::TEXTINPUT;
        }
        else
        {
            m_availableValue = var.toString().split(',');
            m_currentType = Field::SELECT;
        }
        break;
    case TYPE:
        m_currentType= (Field::TypeField)var.toInt();
        break;
    case CLIPPED:
        m_clippedText=var.toBool();
        break;
    case FONT:
        m_font.fromString(var.toString());
        break;
    case PAGE:
        m_page = var.toInt();
        break;
    }
   // update();
}
void Field::setNewEnd(QPointF nend)
{
    #ifdef RCSE
    m_canvasField->setNewEnd(nend);
    #endif
    m_rect.setBottomRight(nend);
    emit widthChanged();
    emit heightChanged();
}
QPointF Field::mapFromScene(QPointF pos)
{
    if(nullptr!=m_canvasField)
    {
        return m_canvasField->mapFromScene(pos);
    }
    return QPointF();
}

QFont Field::font() const
{
    return m_font;
}

void Field::setFont(const QFont &font)
{
    m_font = font;
    //drawField();
}

void Field::mousePressEvent(QMouseEvent* ev)
{
    if(ev->button() == Qt::LeftButton)
    {
       // emit clickOn(this);
    }
}

bool Field::getClippedText() const
{
    return m_clippedText;
}

void Field::setClippedText(bool clippedText)
{
    m_clippedText = clippedText;
}


CharacterSheetItem* Field::getChildAt(QString) const
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
CharacterSheetItem::CharacterSheetItemType Field::getItemType() const
{
    return CharacterSheetItem::FieldItem;
}

void Field::save(QJsonObject& json,bool exp)
{
    if(exp)
    {
        json["type"]="field";
        json["id"]=m_id;
        json["label"]=m_label;
        json["value"]=m_value;
        return;
    }
    json["type"]="field";
    json["id"]=m_id;
    json["typefield"]=m_currentType;
    json["label"]=m_label;
    json["value"]=m_value;
    json["border"]=m_border;
    json["page"]=m_page;
    json["formula"]=m_formula;

    json["clippedText"]=m_clippedText;

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
    json["x"]=getValueFrom(CharacterSheetItem::X,Qt::DisplayRole).toDouble();
    json["y"]=getValueFrom(CharacterSheetItem::Y,Qt::DisplayRole).toDouble();
    json["width"]=getValueFrom(CharacterSheetItem::WIDTH,Qt::DisplayRole).toDouble();
    json["height"]=getValueFrom(CharacterSheetItem::HEIGHT,Qt::DisplayRole).toDouble();
    QJsonArray valuesArray;
    valuesArray=QJsonArray::fromStringList(m_availableValue);
    json["values"]=valuesArray;
}

void Field::load(QJsonObject &json,QList<QGraphicsScene*> scene)
{
    Q_UNUSED(scene);
    m_id = json["id"].toString();
    m_border = (BorderLine)json["border"].toInt();
    m_value= json["value"].toString();
    m_label = json["label"].toString();

    m_currentType=(Field::TypeField)json["typefield"].toInt();
    m_clippedText=json["clippedText"].toBool();

    m_formula = json["formula"].toString();

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
    a = textcolor["a"].toInt();

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
    #ifdef RCSE
    m_canvasField->setPos(x,y);
    m_canvasField->setWidth(w);
    m_canvasField->setHeight(h);
    #endif

    //update();
}
void Field::initGraphicsItem()
{
    m_canvasField->setPos(m_rect.x(),m_rect.y());
#ifdef RCSE
    m_canvasField->setWidth(m_rect.width());
    m_canvasField->setHeight(m_rect.height());
#endif
}

void Field::storeQMLCode()
{
    if(m_generatedCode.isEmpty())
    {
        QTextStream out(&m_generatedCode);
        generateQML(out,CharacterSheetItem::FieldSec,0,false);
    }
}

qreal Field::getWidth() const
{
    return getValueFrom(CharacterSheetItem::WIDTH, Qt::DisplayRole).toReal();
}

void Field::setWidth(qreal width)
{
    setValueFrom(CharacterSheetItem::WIDTH,width);
}

qreal Field::getHeight() const
{
    return getValueFrom(CharacterSheetItem::HEIGHT, Qt::DisplayRole).toReal();
}

void Field::setHeight(qreal height)
{
    setValueFrom(CharacterSheetItem::HEIGHT,height);
}

void Field::setX(qreal x)
{
    setValueFrom(CharacterSheetItem::X,x);
}

qreal Field::getX() const
{
    return getValueFrom(CharacterSheetItem::X, Qt::DisplayRole).toReal();
}

void Field::setY(qreal y)
{
    setValueFrom(CharacterSheetItem::Y,y);
}

qreal Field::getY() const
{
    return getValueFrom(CharacterSheetItem::Y, Qt::DisplayRole).toReal();
}
void Field::loadDataItem(QJsonObject &json)
{
    m_id = json["id"].toString();
    setValue(json["value"].toString(),true);
    setLabel(json["label"].toString());
    setFormula(json["formula"].toString());
    setReadOnly(json["readonly"].toBool());
    m_currentType=(Field::TypeField)json["typefield"].toInt();
}

void Field::saveDataItem(QJsonObject &json)
{
    json["type"]="field";
    json["typefield"]=m_currentType;
    json["id"]=m_id;
    json["label"]=m_label;
    json["value"]=m_value;
    json["formula"]=m_formula;
    json["readonly"]=m_readOnly;
}
QString Field::getQMLItemName()
{
    if(!m_availableValue.isEmpty())
    {
        return "SelectField";
    }
    switch(m_currentType)
    {
    case Field::TEXTFIELD:
        return "TextFieldField";
    case Field::TEXTINPUT:
        return "TextInputField";
    case Field::TEXTAREA:
        return "TextAreaField";
    case Field::CHECKBOX:
        return "CheckBoxField";
    case Field::SELECT:
        return "SelectField";
    case Field::IMAGE:
        return "ImageField";
    case Field::TABLE:
        return "Item";
    case Field::FUNCBUTTON:
    case Field::BUTTON:
        return "DiceButton";
    default:
        return "";
        break;
    }
}

CanvasField* Field::getCanvasField() const
{
    return m_canvasField;
}

void Field::setCanvasField(CanvasField *canvasField)
{
    m_canvasField = canvasField;
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

void Field::setTextAlign(const Field::TextAlign &textAlign)
{
    m_textAlign = textAlign;
}
Field::TextAlign Field::getTextAlignValue()
{
    return m_textAlign;
}

void Field::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec,int i, bool isTable)
{
    if(!m_generatedCode.isEmpty())
    {
        out << m_generatedCode;
        return;
    }
    if(nullptr==m_canvasField)
    {
        return;
    }
    if(sec==CharacterSheetItem::FieldSec)
    {

        out << getQMLItemName() <<" {//"<< m_label <<"\n";
        if(!m_availableValue.isEmpty())
        {
            out << "    availableValues:" << QStringLiteral("[\"%1\"]").arg(m_availableValue.join("\",\""))<<"\n";
            out << "    currentIndex: combo.find(text)\n";
            out << "    onCurrentIndexChanged:{\n";
            out << "    if(count>0)\n";
            out << "    {\n";
            out << "    "<<m_id<<".value = currentText\n";
            out << "    }}\n";
        }
        if(!isTable)
        {
            out << "    id: _"<<m_id<< "\n";
            if((m_currentType==Field::BUTTON)||(m_currentType==Field::FUNCBUTTON))
            {
                out << "    text: "<<m_id<<".label\n";
            }
            else if(m_currentType!=Field::FUNCBUTTON)
            {
                out << "    text: "<<m_id << ".value\n";
            }
        }
        else
        {
            out << "    text:"<<m_label<<".value\n";
        }
        out << "    textColor:\""<< m_textColor.name(QColor::HexArgb) <<"\"\n";
        if(m_clippedText)
        {
            out << "    clippedText:true\n";
        }
        if(!isTable)
        {
            out << "    x:" << m_canvasField->pos().x() << "*root.realscale"<<"\n";
            out << "    y:" <<  m_canvasField->pos().y()<< "*root.realscale"<<"\n";
        }

        out << "    width:" << m_canvasField->boundingRect().width() <<"*root.realscale"<<"\n";
        out << "    height:"<< m_canvasField->boundingRect().height()<<"*root.realscale"<<"\n";
        out << "    color: \"" << m_bgColor.name(QColor::HexArgb)<<"\"\n";
        if(m_page>=0)
        {
            out << "    visible: root.page == "<< m_page << "? true : false\n";
        }
        out << "    readOnly: "<<m_id<<".readOnly\n";
        if(hasFontField())
        {
            out << "    font.family: \"" << m_font.family() <<"\"\n";
            out << "    font.bold: " << (m_font.bold()?"true":"false") <<"\n";
            out << "    font.italic: " << (m_font.italic()?"true":"false") <<"\n";
            out << "    font.underline: " << (m_font.underline()?"true":"false") <<"\n";
            out << "    font.pointSize: " << m_font.pointSize() <<"\n";
            out << "    font.overline: " << (m_font.overline()?"true":"false") <<"\n";
            out << "    font.strikeout: " << (m_font.strikeOut()?"true":"false") <<"\n";

        }
        if(m_currentType==Field::BUTTON)
        {
           out << "    onClicked:rollDiceCmd("<<m_id<<".value)\n";
        }
        else if(m_currentType==Field::FUNCBUTTON)
        {
            out << "    onClicked:{\n    " << m_value <<"\n    }\n";
        }
        if(m_currentType== Field::TEXTINPUT)
        {
            QPair<QString,QString> pair = getTextAlign();
            out << "    hAlign: "<< pair.first<<"\n";
            out << "    vAlign: "<< pair.second <<"\n";
        }
        if((m_availableValue.isEmpty())&&(m_currentType!=Field::BUTTON)&&(m_currentType!=Field::FUNCBUTTON))
        {
            out << "    onTextChanged: {\n";
            if(!isTable)
            {
                out << "    "<<m_id<<".value = text\n    }\n";
            }
            else
            {
                    out << "    "<<m_label<<".value = text\n    }\n";
            }

        }
        out << "}\n";
    }
}
bool Field::hasFontField()
{
    switch (m_currentType)
    {
    case Field::TEXTINPUT:
    case Field::TEXTAREA:
    case Field::TEXTFIELD:
    case Field::BUTTON:
    case Field::FUNCBUTTON:
        return true;
        break;
    case Field::SELECT:
    case Field::CHECKBOX:
    case Field::IMAGE:
        return false;
    default:
        return false;
    }
    return false;
}

QString Field::getGeneratedCode() const
{
    return m_generatedCode;
}

void Field::setGeneratedCode(const QString &generatedCode)
{
    m_generatedCode = generatedCode;
}

QPair<QString,QString> Field::getTextAlign()
{
    QPair<QString,QString> pair;


    QString hori;
    if(m_textAlign%3 == 0)
    {
        hori = "TextInput.AlignRight";
    }
    else if(m_textAlign%3==1)
    {
        hori = "TextInput.AlignHCenter";
    }
    else
    {
        hori = "TextInput.AlignLeft";
    }
    pair.first= hori;
    QString verti;
    if(m_textAlign/3 == 0)
    {
        verti = "TextInput.AlignTop";
    }
    else if(m_textAlign/3==1)
    {
        verti = "TextInput.AlignVCenter";
    }
    else
    {
        verti = "TextInput.AlignBottom";
    }
    pair.second = verti;

    return pair;
}

void Field::copyField(CharacterSheetItem* oldItem,bool copyData)
{
    Field* oldField =  dynamic_cast<Field*>(oldItem);
    if(NULL!=oldField)
    {
        setId(oldField->getId());
        setCurrentType(oldField->getCurrentType());
        setRect(oldField->getRect());
        setBorder(oldField->border());
        setFont(oldField->font());
        setBgColor(oldField->bgColor());
        setTextColor(oldField->textColor());
        setLabel(oldField->getLabel());
        setFormula(oldField->getFormula());
        if(copyData)
        {
            if(!m_hasDefaultValue)
            {
                setValue(oldField->value());
            }
        }
        setOrig(oldField);
    }
}
