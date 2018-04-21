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
   for(const auto field : m_fields)
   {
       if(field->getId() == id)
       {
           return field;
       }
   }
   return nullptr;
}
void LineFieldItem::save(QJsonArray &json)
{
    for(const auto field : m_fields)
    {
        QJsonObject obj;
        field->save(obj);
        json.append(obj);
    }
}
void LineFieldItem::saveDataItem(QJsonArray &json)
{
    for(const auto field : m_fields)
    {
        QJsonObject obj;
        field->saveDataItem(obj);
        json.append(obj);
    }
}
void LineFieldItem::load(QJsonArray &json, QList<QGraphicsScene *> scene,  CharacterSheetItem* parent)
{
    for( auto value : json)
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
    for( auto value : json)
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
    for(auto fieldLine : first->getFields() )
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
    for(const auto& line : m_lines)
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
    return 0;
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
    for(const auto& line : m_lines)
    {
        QJsonArray lineJson;
        line->save(lineJson);
        json.append(lineJson);
    }
}

void LineModel::saveDataItem(QJsonArray &json)
{
    for(const auto& line : m_lines)
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
    return m_tableCanvasField->lineCount();
}

CharacterSheetItem *TableField::getRoot()
{
    return m_tableCanvasField->getRoot();
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
    return m_model->getField(line,col);
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
    for(auto value : valuesArray.toVariantList())
    {
        m_availableValue << value.toString();
    }
    m_rect.setRect(x,y,w,h);
    QJsonArray childArray=json["children"].toArray();
    m_model->load(childArray,scene,this);

    #ifdef RCSE
    if(json.contains("canvas"))
    {
        m_tableCanvasField = new TableCanvasField(this);
        auto obj = json["canvas"].toObject();
        m_tableCanvasField->load(obj,scene);
        m_canvasField = m_tableCanvasField;
    }
    m_canvasField->setPos(x,y);
    m_canvasField->setWidth(w);
    m_canvasField->setHeight(h);
    #endif

}

void TableField::copyField(CharacterSheetItem *oldItem, bool copyData, bool sameId)
{
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
void TableField::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec,int i, bool isTable)
{
    #ifdef RCSE
    Q_UNUSED(i)
    Q_UNUSED(isTable)

    if(nullptr==m_tableCanvasField)
    {
        return;
    }

    m_model->clear();

    m_tableCanvasField->fillLineModel(m_model,this);
    emit updateNeeded(this);
    if(sec==CharacterSheetItem::FieldSec)
    {
        out << "    ListView{//"<< m_label <<"\n";
        out << "        id: _" << m_id<<"list\n";
        out << "        x:" << m_tableCanvasField->pos().x() << "*root.realscale"<<"\n";
        out << "        y:" <<  m_tableCanvasField->pos().y()<< "*root.realscale"<<"\n";
        out << "        width:" << m_tableCanvasField->boundingRect().width() <<"*root.realscale"<<"\n";
        out << "        height:"<< m_tableCanvasField->boundingRect().height()<<"*root.realscale"<<"\n";
        if(m_page>=0)
        {
            out << "    visible: root.page == "<< m_page << "? true : false\n";
        }
        out << "        readonly property int maxRow:"<<m_tableCanvasField->lineCount() << "\n";
        out << "        interactive: count>maxRow?true:false;\n";
        out << "        clip: true;\n";
        out << "        model:"<< m_id << ".model\n";
        out << "        delegate: RowLayout {\n";
        out << "            height: _"<< m_id<<"list.height/_"<< m_id<<"list.maxRow\n";
        out << "            width:  _"<< m_id<<"list.width\n";
        out << "            spacing:0\n";
        m_tableCanvasField->generateSubFields(out);
        out << "        }\n";
        out << "     }\n";
        out << "     Button {\n";
        out << computeControlPosition();
        out << "        text: \""<< tr("Add line") << "\"\n";
        out << "        onClicked: "<< m_id <<".addLine()\n";
        out << "     }\n";
    }
    #else
        Q_UNUSED(out)
        Q_UNUSED(sec)
        Q_UNUSED(isTable)
        Q_UNUSED(i)
    #endif
}

QString TableField::computeControlPosition()
{
    #ifdef RCSE
    QString Line1("anchors.%1: _%5list.%2\nanchors.%3: _%5list.%4\n");//top,button//left,right

    m_position = static_cast<ControlPosition>(m_tableCanvasField->getPosition());
    switch (m_position)
    {
        case TableField::CtrlLeftTop:
            Line1=Line1.arg(QLatin1String("top"))
                    .arg(QLatin1String("top"))
                    .arg(QLatin1String("right"))
                    .arg(QLatin1String("left"));
        break;
        case TableField::CtrlTopLeft:
            Line1=Line1.arg(QLatin1String("bottom"))
                    .arg(QLatin1String("top"))
                    .arg(QLatin1String("left"))
                    .arg(QLatin1String("left"));
        break;
        case TableField::CtrlLeftBottom:
            Line1=Line1.arg(QLatin1String("bottom"))
                    .arg(QLatin1String("bottom"))
                    .arg(QLatin1String("right"))
                    .arg(QLatin1String("left"));
        break;
        case TableField::CtrlBottomLeft:
        Line1=Line1.arg(QLatin1String("top"))
                .arg(QLatin1String("bottom"))
                .arg(QLatin1String("left"))
                .arg(QLatin1String("left"));
        break;
        case TableField::CtrlBottomRight:
        Line1=Line1.arg(QLatin1String("top"))
                .arg(QLatin1String("bottom"))
                .arg(QLatin1String("right"))
                .arg(QLatin1String("right"));
        break;
        case TableField::CtrlRightTop:
        Line1=Line1.arg(QLatin1String("top"))
                .arg(QLatin1String("top"))
                .arg(QLatin1String("left"))
                .arg(QLatin1String("right"));
        break;
        case TableField::CtrlTopRight:
        Line1=Line1.arg(QLatin1String("bottom"))
                .arg(QLatin1String("top"))
                .arg(QLatin1String("right"))
                .arg(QLatin1String("right"));
        break;
        case TableField::CtrlRightBottom:
        Line1=Line1.arg(QLatin1String("bottom"))
                .arg(QLatin1String("bottom"))
                .arg(QLatin1String("left"))
                .arg(QLatin1String("right"));
        break;
    }
    return Line1.arg(m_id);
    #else
    return {};
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
