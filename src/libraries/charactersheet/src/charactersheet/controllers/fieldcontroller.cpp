/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * https://rolisteam.org/                                                *
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
#include "charactersheet/controllers/fieldcontroller.h"
#include <QDebug>
#include <QJsonArray>
#include <QMouseEvent>
#include <QPainter>
#include <QUuid>

/*#ifndef RCSE
CanvasFieldController::CanvasField() {}
#endif*/

FieldController::FieldController(TreeSheetItem::TreeItemType itemType, bool addCount, QObject* parent)
    : CSItem(itemType, parent, addCount)
{
    init();
}

FieldController::FieldController(TreeSheetItem::TreeItemType itemType, QPointF topleft, bool addCount, QObject* parent)
    : CSItem(itemType, parent, addCount)
{
    Q_UNUSED(topleft);
    m_hasDefaultValue= true;
    m_value= QString("value %1").arg(m_count);
    init();
}
FieldController::~FieldController() {}
void FieldController::init()
{
    // m_canvasField= new CanvasField(this);

    m_id= QStringLiteral("id_%1").arg(m_count);
    m_fieldType= TEXTINPUT;

    m_border= NONE;
    m_textAlign= CenterMiddle;
    m_bgColor= Qt::transparent;
    m_textColor= Qt::black;
    m_font= font();
    /*if(nullptr != m_canvasField)
    {
        m_canvasField->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges
                                | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable
                                | QGraphicsItem::ItemClipsToShape);

        connect(m_canvasField, &CanvasField::xChanged, this, [this]() { emit updateNeeded(this); });
        connect(m_canvasField, &CanvasField::yChanged, this, [this]() { emit updateNeeded(this); });
        connect(m_canvasField, &CanvasField::lockedChanged, this, [this]() {
            m_locked= m_canvasField->locked();
            emit updateNeeded(this);
        });
    }*/
}

QVariant FieldController::valueFrom(TreeSheetItem::ColumnId id, int role) const
{
    QVariant ret;

    static QSet<TreeSheetItem::ColumnId> columnWithBackground{TreeSheetItem::BGCOLOR, TreeSheetItem::TEXTCOLOR};

    if((role == Qt::BackgroundRole) && !columnWithBackground.contains(id))
    {

        if(!generatedCode().isEmpty())
            ret= QColor(Qt::green).lighter();
        if(m_readOnly && id >= TreeSheetItem::X && id <= TreeSheetItem::HEIGHT)
            ret= QColor(Qt::gray);

        return ret;
    }

    switch(id)
    {
    case ID:
        ret= m_id;
        break;
    case LABEL:
        ret= m_label;
        break;
    case VALUE:
        ret= role == Qt::EditRole ? m_formula : role == Qt::DisplayRole ? m_value.left(50) : m_value;
        break;
    case FORMULA:
        ret= m_formula;
        break;
    case X:
        ret= x();
        break;
    case Y:
        ret= y();
        break;
    case WIDTH:
        ret= width();
        break;
    case HEIGHT:
        ret= height();
        break;
    case BORDER:
        ret= static_cast<int>(m_border);
        break;
    case TEXT_ALIGN:
        ret= static_cast<int>(m_textAlign);
        break;
    case BGCOLOR:
        if(role == Qt::DisplayRole)
        {
            ret= m_bgColor.name(QColor::HexArgb);
        }
        else
        {
            ret= m_bgColor;
        }
        break;
    case TEXTCOLOR:
        ret= m_textColor;
        break;
    case VALUES:
        ret= m_availableValue.join(',');
        break;
    case TYPE:
        ret= fieldType();
        break;
    case FitFont:
        ret= m_fitFont;
        break;
    case FONT:
        ret= m_font.toString();
        break;
    case PAGE:
        ret= m_page;
        break;
    case TOOLTIP:
        ret= m_tooltip;
        break;
    }
    return ret;
}

void FieldController::setValueFrom(TreeSheetItem::ColumnId id, const QVariant& var)
{
    if(readOnly())
        return;
    switch(id)
    {
    case ID:
        setId(var.toString().trimmed());
        break;
    case LABEL:
        setLabel(var.toString().trimmed());
        break;
    case VALUE:
        setValue(var.toString());
        break;
    case X:
        setX(var.toReal());
        break;
    case Y:
        setY(var.toReal());
        break;
    case WIDTH:
        setWidth(var.toReal());
        break;
    case HEIGHT:
        setHeight(var.toReal());
        break;
    case BORDER:
        m_border= static_cast<BorderLine>(var.toInt());
        break;
    case TEXT_ALIGN:
        m_textAlign= static_cast<TextAlign>(var.toInt());
        break;
    case BGCOLOR:
        m_bgColor= var.value<QColor>();
        break;
    case TEXTCOLOR:
        m_textColor= var.value<QColor>();
        break;
    case VALUES:
        // to move into model
        setAvailableValues(var.toString().split(','));
        if(m_fieldType == FieldController::SELECT && var.toString().isEmpty())
        {
            setFieldType(FieldController::TEXTINPUT);
        }
        else if(m_fieldType == FieldController::TEXTINPUT)
        {
            m_availableValue= var.toString().split(',');
            setFieldType(FieldController::SELECT);
        }
        break;
    case TYPE:
        setFieldType(static_cast<FieldController::TypeField>(var.toInt()));
        break;
    case FitFont:
        m_fitFont= var.toBool();
        break;
    case FONT:
        m_font.fromString(var.toString());
        break;
    case PAGE:
        m_page= var.toInt();
        break;
    case TOOLTIP:
        m_tooltip= var.toString();
        break;
    }
    // update();
}

QFont FieldController::font() const
{
    return m_font;
}

void FieldController::setFont(const QFont& font)
{
    if(readOnly())
        return;
    m_font= font;
    // drawField();
}

bool FieldController::fitFont() const
{
    return m_fitFont;
}

void FieldController::setFitFont(bool clippedText)
{
    if(readOnly() || m_fitFont == clippedText)
        return;
    m_fitFont= clippedText;
    emit fitFontChanged(m_fitFont);
}

QStringList FieldController::availableValues() const
{
    return m_availableValue;
}

void FieldController::setAvailableValues(const QStringList& availableValue)
{
    if(readOnly() || m_availableValue == availableValue)
        return;
    m_availableValue= availableValue;
    emit availableValuesChanged();

    if(fieldType() != FieldController::SLIDER)
        setFieldType(m_availableValue.isEmpty() ? FieldController::TEXTINPUT : FieldController::SELECT);
}

void FieldController::save(QJsonObject& json, bool exp)
{
    if(exp)
    {
        json["type"]= "field";
        json["id"]= m_id;
        json["label"]= m_label;
        json["value"]= m_value;
        return;
    }
    json["type"]= "field";
    json["id"]= m_id;
    json["typefield"]= m_fieldType;
    json["label"]= m_label;
    json["value"]= m_value;
    json["border"]= m_border;
    json["page"]= m_page;
    json["formula"]= m_formula;
    json["tooltip"]= m_tooltip;
    json["generatedCode"]= m_generatedCode;

    json["clippedText"]= m_fitFont;

    QJsonObject bgcolor;
    bgcolor["r"]= QJsonValue(m_bgColor.red());
    bgcolor["g"]= m_bgColor.green();
    bgcolor["b"]= m_bgColor.blue();
    bgcolor["a"]= m_bgColor.alpha();
    json["bgcolor"]= bgcolor;

    QJsonObject textcolor;
    textcolor["r"]= m_textColor.red();
    textcolor["g"]= m_textColor.green();
    textcolor["b"]= m_textColor.blue();
    textcolor["a"]= m_textColor.alpha();
    json["textcolor"]= textcolor;

    json["font"]= m_font.toString();
    json["textalign"]= m_textAlign;
    json["x"]= valueFrom(TreeSheetItem::X, Qt::DisplayRole).toDouble();
    json["y"]= valueFrom(TreeSheetItem::Y, Qt::DisplayRole).toDouble();
    json["width"]= valueFrom(TreeSheetItem::WIDTH, Qt::DisplayRole).toDouble();
    json["height"]= valueFrom(TreeSheetItem::HEIGHT, Qt::DisplayRole).toDouble();
    QJsonArray valuesArray;
    valuesArray= QJsonArray::fromStringList(m_availableValue);
    json["values"]= valuesArray;
}

void FieldController::load(const QJsonObject& json)
{
    m_id= json["id"].toString();
    m_border= static_cast<BorderLine>(json["border"].toInt());
    m_value= json["value"].toString();
    m_label= json["label"].toString();
    m_tooltip= json["tooltip"].toString();

    m_fieldType= static_cast<FieldController::TypeField>(json["typefield"].toInt());
    m_fitFont= json["clippedText"].toBool();

    m_formula= json["formula"].toString();

    QJsonObject bgcolor= json["bgcolor"].toObject();
    int r, g, b, a;
    r= bgcolor["r"].toInt();
    g= bgcolor["g"].toInt();
    b= bgcolor["b"].toInt();
    a= bgcolor["a"].toInt();

    m_bgColor= QColor(r, g, b, a);

    QJsonObject textcolor= json["textcolor"].toObject();

    r= textcolor["r"].toInt();
    g= textcolor["g"].toInt();
    b= textcolor["b"].toInt();
    a= textcolor["a"].toInt();

    m_textColor= QColor(r, g, b, a);

    m_font.fromString(json["font"].toString());

    m_textAlign= static_cast<TextAlign>(json["textalign"].toInt());
    qreal x, y, w, h;
    x= json["x"].toDouble();
    y= json["y"].toDouble();
    w= json["width"].toDouble();
    h= json["height"].toDouble();
    m_page= json["page"].toInt();
    m_generatedCode= json["generatedCode"].toString();

    auto const& valuesArray= json["values"].toArray().toVariantList();
    for(auto& value : valuesArray)
    {
        m_availableValue << value.toString();
    }

    setX(x);
    setY(y);
    setWidth(w);
    setHeight(h);

    // update();
}

/*void FieldController::storeQMLCode()
{
    if(m_generatedCode.isEmpty())
    {
        QTextStream out(&m_generatedCode);
        //QmlGeneratorVisitor visitor(out, this);
        visitor.generateQmlCodeForRoot();

        // generateQML(out,TreeSheetItem::FieldSec,0,false);
    }
}*/

void FieldController::loadDataItem(const QJsonObject& json)
{
    m_id= json["id"].toString();
    setValue(json["value"].toString(), true);
    setLabel(json["label"].toString());
    setFormula(json["formula"].toString());
    setReadOnly(json["readonly"].toBool());
    m_fieldType= static_cast<FieldController::TypeField>(json["typefield"].toInt());
}

void FieldController::saveDataItem(QJsonObject& json)
{
    json["type"]= "field";
    json["typefield"]= m_fieldType;
    json["id"]= m_id;
    json["label"]= m_label;
    json["value"]= m_value;
    json["formula"]= m_formula;
    json["readonly"]= readOnly();
}

void FieldController::setTextAlign(const FieldController::TextAlign& textAlign)
{
    if(readOnly())
        return;
    m_textAlign= textAlign;
}
FieldController::TextAlign FieldController::textAlign()
{
    return m_textAlign;
}

QString FieldController::generatedCode() const
{
    return m_generatedCode;
}

void FieldController::setGeneratedCode(const QString& generatedCode)
{
    if(readOnly() || m_generatedCode == generatedCode)
        return;
    m_generatedCode= generatedCode;
    emit generatedCodeChanged();
}

QPair<QString, QString> FieldController::getTextAlign()
{
    QPair<QString, QString> pair;

    QString hori;
    if(m_textAlign % 3 == 0)
    {
        hori= "TextInput.AlignRight";
    }
    else if(m_textAlign % 3 == 1)
    {
        hori= "TextInput.AlignHCenter";
    }
    else
    {
        hori= "TextInput.AlignLeft";
    }
    pair.first= hori;
    QString verti;
    if(m_textAlign / 3 == 0)
    {
        verti= "TextInput.AlignTop";
    }
    else if(m_textAlign / 3 == 1)
    {
        verti= "TextInput.AlignVCenter";
    }
    else
    {
        verti= "TextInput.AlignBottom";
    }
    pair.second= verti;

    return pair;
}

void FieldController::copyField(TreeSheetItem* oldItem, bool copyData, bool sameId)
{
    FieldController* oldField= dynamic_cast<FieldController*>(oldItem);
    if(nullptr == oldField)
        return;

    if(sameId)
    {
        setId(oldField->id());
    }
    setFitFont(oldField->fitFont());
    setTextAlign(oldField->textAlign());
    setFieldType(oldField->fieldType());
    setGeneratedCode(oldField->generatedCode());
    setAvailableValues(oldField->availableValues());

    setX(oldField->x());
    setY(oldField->y());
    setWidth(oldField->width());
    setHeight(oldField->height());
    setBorder(oldField->border());
    setBgColor(oldField->bgColor());

    setValue(oldField->value());
    setPage(oldField->page());
    setReadOnly(oldField->readOnly());
    setFormula(oldField->formula());
    setLabel(oldField->label());

    setFont(oldField->font());
    setTextColor(oldField->textColor());
    if(copyData)
    {
        if(!m_hasDefaultValue)
        {
            setValue(oldField->value());
        }
    }
    setOrig(oldField);
}

bool FieldController::visible() const
{
    return m_visible;
}

void FieldController::setVisible(bool newVisible)
{
    if(m_visible == newVisible)
        return;
    m_visible= newVisible;
    emit visibleChanged();
}
