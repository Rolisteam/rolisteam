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


void copyModel(LineModel* src, LineModel* dest,CharacterSheetItem* parent)
{
    QJsonArray array;
    src->saveDataItem(array);
    dest->loadDataItem(array,parent);
}
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
    if(m_fields.size()>k)
        return m_fields.at(k);
    else
        return nullptr;
}

QList<Field *> LineFieldItem::getFields() const
{
    return m_fields;
}

void LineFieldItem::setFields(const QList<Field *> &fields)
{
    m_fields = fields;
}
int LineFieldItem::getFieldCount() const
{
    return m_fields.size();
}
Field* LineFieldItem::getFieldById(const QString& id)
{
   for(auto& field : m_fields)
   {
       if(field->getId() == id)
       {
           return field;
       }
   }
   return nullptr;
}
Field* LineFieldItem::getFieldByLabel(const QString& label)
{
   for(auto& field : m_fields)
   {
       if(field->getLabel() == label)
       {
           return field;
       }
   }
   return nullptr;
}
void LineFieldItem::save(QJsonArray &json)
{
    for(auto& field : m_fields)
    {
        QJsonObject obj;
        field->save(obj);
        json.append(obj);
    }
}
void LineFieldItem::saveDataItem(QJsonArray &json)
{
    for(auto& field : m_fields)
    {
        QJsonObject obj;
        field->saveDataItem(obj);
        json.append(obj);
    }
}
void LineFieldItem::load(QJsonArray &json, QList<QGraphicsScene *> scene,  CharacterSheetItem* parent)
{
    for( auto const& value : json)
    {
        Field* field = new Field();
        field->setParent(parent);
        QJsonObject obj = value.toObject();
        field->load(obj,scene);
        m_fields.append(field);
    }
}
void LineFieldItem::loadDataItem(QJsonArray &json, CharacterSheetItem* parent)
{
    for( auto const& value : json)
    {
        Field* field = new Field();
        field->setParent(parent);
        connect(field, &Field::sendOffData, parent, &CharacterSheetItem::sendOffData);
        connect(field, &Field::updateNeeded, parent, &CharacterSheetItem::updateNeeded);
        QJsonObject obj = value.toObject();
        field->loadDataItem(obj);
        m_fields.append(field);
    }
}

////////////////////////////////////////
//
////////////////////////////////////////
LineModel::LineModel()
{

}
int LineModel::rowCount(const QModelIndex&) const
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
        return QVariant::fromValue<Field*>(item->getField(key/2));
    }
    //return QVariant();
}

QHash<int, QByteArray>  LineModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LineRole] = "line";
    int i = 1;
    auto first = m_lines.first();
    for(auto& fieldLine : first->getFields() )
    {
        roles[LineRole+i]=fieldLine->getId().toUtf8();
        i++;
        roles[LineRole+i]=fieldLine->getLabel().toUtf8();
        i++;
    }
    return roles;
}

void LineModel::insertLine(LineFieldItem* line)
{
    beginInsertRows(QModelIndex(),m_lines.size(),m_lines.size());
    m_lines.append(line);
    endInsertRows();
}

void LineModel::appendLine(TableField* field)
{
    if(m_lines.isEmpty())
        return;

    auto line = m_lines.last();
    QJsonArray array;
    line->save(array);
    auto fieldLine = new LineFieldItem();
    fieldLine->loadDataItem(array, field);
    insertLine(fieldLine);
}

void LineModel::clear()
{
    beginResetModel();
    qDeleteAll(m_lines);
    m_lines.clear();
    endResetModel();
}

int LineModel::getChildrenCount() const
{
    if(!m_lines.isEmpty())
    {
        return m_lines.size()*getColumnCount();
    }
    return 0;
}

Field*  LineModel::getFieldById(const QString& id)
{
    for(auto& line : m_lines)
    {
        auto item = line->getFieldById(id);
        if(nullptr != item)
            return item;
    }
    return nullptr;

}

int LineModel::getColumnCount() const
{
    if(!m_lines.isEmpty())
    {
        auto line = m_lines.first();
        return line->getFieldCount();
    }
    return -1;
}

Field* LineModel::getField(int line, int col)
{
    if(m_lines.size()>line)
    {
        return m_lines.at(line)->getField(col);
    }
    return nullptr;
}

void LineModel::save(QJsonArray &json)
{
    for(auto& line : m_lines)
    {
        QJsonArray lineJson;
        line->save(lineJson);
        json.append(lineJson);
    }
}

void LineModel::saveDataItem(QJsonArray &json)
{
    for(auto& line : m_lines)
    {
        QJsonArray lineJson;
        line->saveDataItem(lineJson);
        json.append(lineJson);
    }
}

void LineModel::load(QJsonArray &json, QList<QGraphicsScene *> scene, CharacterSheetItem* parent)
{
    beginResetModel();
    QJsonArray::Iterator it;
    for(it = json.begin(); it != json.end(); ++it)
    {
        QJsonArray obj = (*it).toArray();
        LineFieldItem* line = new LineFieldItem();
        line->load(obj,scene,parent);
        m_lines.append(line);
    }
    endResetModel();
}

void LineModel::loadDataItem(QJsonArray &json, CharacterSheetItem* parent)
{
    beginResetModel();
    m_lines.clear();
    QJsonArray::Iterator it;
    for(it = json.begin(); it != json.end(); ++it)
    {
        QJsonArray obj = (*it).toArray();
        LineFieldItem* line = new LineFieldItem();
        line->loadDataItem(obj,parent);
        m_lines.append(line);
    }
    endResetModel();
}

void LineModel::setChildFieldData(QJsonObject &json)
{
    for(auto& line : m_lines)
    {
        auto field = line->getFieldById(json["id"].toString());
        if(field)
        {
            field->loadDataItem(json);
            return;
        }
    }
}

void LineModel::setFieldInDictionnary(QHash<QString, QString> &dict) const
{

}

void LineModel::removeLine(int index)
{
    if(m_lines.isEmpty())
        return;
    if(m_lines.size()<=index)
        return;
    if(index<0)
        return;
    beginRemoveRows(QModelIndex(),index,index);
    m_lines.removeAt(index);
    endRemoveRows();
}

bool LineModel::setData(const QModelIndex& index, const QVariant &data, int role)
{
    return QAbstractListModel::setData(index, data, role);
}

int LineModel::sumColumn(const QString& name) const
{
    int sum = 0;
    for(auto line : m_lines)
    {
        auto field = line->getFieldByLabel(name);
        if(nullptr == field)
        {
            //should not happen
            field = line->getFieldById(name);
        }
        if(nullptr == field)
            continue;

        sum += field->value().toInt();
    }
    return sum;
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

void TableField::removeLine(int index)
{
    m_model->removeLine(index);
}

void TableField::removeLastLine()
{
    QModelIndex index;
    m_model->removeLine(m_model->rowCount(index)-1);
}

void TableField::addLine()
{
    emit lineMustBeAdded(this);
}

void TableField::appendChild(CharacterSheetItem*)
{
    m_model->appendLine(this);
}

void TableField::init()
{
    m_canvasField = nullptr;
    m_tableCanvasField = nullptr;
    m_id = QStringLiteral("id_%1").arg(m_count);
    m_currentType=Field::TABLE;
    m_clippedText = false;
    m_model = new LineModel();


    m_border=NONE;
    m_textAlign = Field::TopLeft;
    m_bgColor = Qt::transparent;
    m_textColor = Qt::black;
    m_font = font();
}

TableField::ControlPosition TableField::getPosition() const
{
    return m_position;
}

void TableField::setPosition(const ControlPosition &position)
{
    m_position = position;
}

void TableField::setCanvasField(CanvasField *canvasField)
{
    m_tableCanvasField = dynamic_cast<TableCanvasField*>(canvasField);
    Field::setCanvasField(canvasField);
}

QVariant TableField::getValueFrom(CharacterSheetItem::ColumnId col, int role) const
{
    if(col == CharacterSheetItem::VALUE)
    {
        return m_model->getChildrenCount();
    }
    return Field::getValueFrom(col,role);
}


bool TableField::hasChildren()
{
    if(m_model->rowCount(QModelIndex())>0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int TableField::getChildrenCount() const
{
   return m_model->getChildrenCount();
}

int TableField::getMaxVisibleRowCount() const
{
    #ifdef RCSE
        return m_tableCanvasField->lineCount();
    #else
        return 0;
    #endif

}

CharacterSheetItem *TableField::getRoot()
{
    #ifdef RCSE
        return m_tableCanvasField->getRoot();
    #else
        return nullptr;
#endif
}

CharacterSheetItem* TableField::getChildAt(QString id) const
{
    return m_model->getFieldById(id);
}

CharacterSheetItem* TableField::getChildAt(int index) const
{
    int itemPerLine = m_model->getColumnCount();
    int line = index/itemPerLine;
    int col = index - (line*itemPerLine);
    auto item = m_model->getField(line,col);
    return item;
}

void TableField::save(QJsonObject &json, bool exp)
{
    if(exp)
    {
        json["type"]="TableField";
        json["id"]=m_id;
        json["label"]=m_label;
        json["value"]=m_value;
        json["typefield"]=m_currentType;
        QJsonArray childArray;
        m_model->save(childArray);
        json["children"]=childArray;
        return;
    }
    json["type"]="TableField";
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

    json["positionControl"] = m_position;

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

    QJsonArray childArray;
    m_model->save(childArray);
    json["children"]=childArray;

    #ifdef RCSE
    if(nullptr != m_tableCanvasField)
    {
        QJsonObject obj;
        m_tableCanvasField->save(obj);
        json["canvas"]=obj;
    }
    #endif

}
CharacterSheetItem::CharacterSheetItemType TableField::getItemType() const
{
    return CharacterSheetItemType::TableItem;
}
void TableField::load(QJsonObject &json, QList<QGraphicsScene *> scene)
{
    Q_UNUSED(scene);
    m_id = json["id"].toString();
    m_border = static_cast<BorderLine>(json["border"].toInt());
    m_value= json["value"].toString();
    m_label = json["label"].toString();

    m_currentType=static_cast<Field::TypeField>(json["typefield"].toInt());
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

    m_position = static_cast<ControlPosition>(json["positionControl"].toInt());

    m_font.fromString(json["font"].toString());

    m_textAlign = static_cast<Field::TextAlign>(json["textalign"].toInt());
    qreal x,y,w,h;
    x=json["x"].toDouble();
    y=json["y"].toDouble();
    w=json["width"].toDouble();
    h=json["height"].toDouble();
    m_page=json["page"].toInt();

    QJsonArray valuesArray=json["values"].toArray();
    auto const& list = valuesArray.toVariantList();
    for(auto& value : list)
    {
        m_availableValue << value.toString();
    }
    m_rect.setRect(x,y,w,h);
    QJsonArray childArray=json["children"].toArray();

    QList<QGraphicsScene*> emptyList;
    m_model->load(childArray,emptyList,this);

    #ifdef RCSE
    if(json.contains("canvas"))
    {
        m_tableCanvasField = new TableCanvasField(this);
        auto obj = json["canvas"].toObject();
        m_tableCanvasField->load(obj,emptyList);
        m_canvasField = m_tableCanvasField;
    }
    m_canvasField->setPos(x,y);
    m_canvasField->setWidth(w);
    m_canvasField->setHeight(h);
    #endif

}

void TableField::copyField(CharacterSheetItem *oldItem, bool copyData, bool sameId)
{
    Q_UNUSED(copyData);
    auto const oldField =  dynamic_cast<TableField*>(oldItem);
    if(nullptr!=oldField)
    {
        if(sameId)
        {
            setId(oldField->getId());
        }
        setCurrentType(oldField->getFieldType());
        setRect(oldField->getRect());
        setBorder(oldField->border());
        setFont(oldField->font());
        setBgColor(oldField->bgColor());
        setTextColor(oldField->textColor());
        setLabel(oldField->getLabel());
        setFormula(oldField->getFormula());
        copyModel(oldField->getModel(),m_model,this);
        setOrig(oldField);
    }
}


bool TableField::mayHaveChildren() const
{
    return true;
}

int TableField::lineNumber() const
{
    if(nullptr == m_model)
        return -1;

    return m_model->rowCount(QModelIndex());
}

int TableField::itemPerLine() const
{
    if(nullptr == m_model)
        return -1;

    return m_model->getColumnCount();
}

void TableField::fillModel()
{
    m_model->clear();
    #ifdef RCSE
    m_tableCanvasField->fillLineModel(m_model,this);
    #endif
}

void TableField::loadDataItem(QJsonObject &json)
{
    m_id = json["id"].toString();
    setValue(json["value"].toString(),true);
    setLabel(json["label"].toString());
    setFormula(json["formula"].toString());
    setReadOnly(json["readonly"].toBool());
    m_currentType=static_cast<Field::TypeField>(json["typefield"].toInt());

    QJsonArray childArray=json["children"].toArray();
    m_model->loadDataItem(childArray,this);
}

void TableField::setChildFieldData(QJsonObject &json)
{
    m_model->setChildFieldData(json);
}

void TableField::setFieldInDictionnary(QHash<QString, QString> & dict) const
{
    Field::setFieldInDictionnary(dict);

    m_model->setFieldInDictionnary(dict);
    auto val = value();
    dict[m_id] = val;
    dict[getLabel()] = val;
}

void TableField::saveDataItem(QJsonObject &json)
{
    json["type"]="TableField";
    json["typefield"]=m_currentType;
    json["id"]=m_id;
    json["label"]=m_label;
    json["value"]=m_value;
    json["formula"]=m_formula;
    json["readonly"]=m_readOnly;

    QJsonArray childArray;
    m_model->saveDataItem(childArray);
    json["children"]=childArray;
}
int TableField::sumColumn(const QString& name ) const
{
    return m_model->sumColumn(name);
}
