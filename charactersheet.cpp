/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
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

#include "charactersheet.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUuid>

#include "section.h"
#include "charactersheetbutton.h"
#include "tablefield.h"
/////////////////////////////////////////
//          CharacterSheet           ////
/////////////////////////////////////////
/// \brief CharacterSheet::CharacterSheet
///

int CharacterSheet::m_count=0;
CharacterSheet::CharacterSheet()
    : m_name("Character %1"),m_rootSection(nullptr),m_uuid(QUuid::createUuid().toString())
{
    ++m_count;
    m_name = m_name.arg(m_count);
}
CharacterSheet::~CharacterSheet()
{
    qDeleteAll(m_valuesMap);
    m_valuesMap.clear();
}

const  QString CharacterSheet::getTitle()
{
    return m_name;
}

int CharacterSheet::getFieldCount()
{
    return m_valuesMap.size();
}

CharacterSheetItem* CharacterSheet::getFieldFromIndex(const std::vector<int>& row) const
{
    if(row.empty())
        return nullptr;

    size_t i = 0;
    auto index = row[i];
    auto item = getFieldAt(index);
    ++i;
    while(nullptr != item && i < row.size())
    {
        item = item->getChildAt(index);
        ++i;
    }
    return item;
}

CharacterSheetItem* CharacterSheet::getFieldAt(int i) const
{
    if(i<m_valuesMap.size() && i >=0)
    {
        return m_valuesMap.value(m_valuesMap.keys().at(i));
    }
    return nullptr;
}

CharacterSheetItem* CharacterSheet::getFieldFromKey(QString key) const
{
    QStringList keyList = key.split('.');
    if(keyList.size() > 1)
    {
        CharacterSheetItem* field = m_valuesMap[keyList.takeFirst()];
        field = field->getChildAt(keyList.takeFirst());
        return field;
    }
    else if(m_valuesMap.contains(key))
    {
        return m_valuesMap.value(key);
    }
    return nullptr;
}

const  QVariant CharacterSheet::getValue(QString path,Qt::ItemDataRole role) const
{
    CharacterSheetItem* item = getFieldFromKey(path);
    if(nullptr!=item)
    {
        if(role == Qt::DisplayRole)
        {
             return item->value();
        }
        else if(role == Qt::EditRole)
        {
            QString str = item->getFormula();
            if(str.isEmpty())
            {
                str = item->value();
            }
            return str;
        }
        else if(role == Qt::UserRole)
        {
            return item->getFormula();
        }
        else if(role == Qt::BackgroundRole)
        {
            return item->isReadOnly() ? QColor(Qt::red) : QVariant();
        }
    }
    return QString();
}

const  QVariant CharacterSheet::getValueByIndex(const std::vector<int>& row,QString path,Qt::ItemDataRole role) const
{
    CharacterSheetItem* item = getFieldFromIndex(row);//getFieldFromKey(path);
    if(nullptr!=item)
    {
        if(role == Qt::DisplayRole)
        {
             return item->value();
        }
        else if(role == Qt::EditRole)
        {
            QString str = item->getFormula();
            if(str.isEmpty())
            {
                str = item->value();
            }
            return str;
        }
        else if(role == Qt::UserRole)
        {
            return item->getFormula();
        }
        else if(role == Qt::BackgroundRole)
        {
            return item->isReadOnly() ? QColor(Qt::red) : QVariant();
        }
    }
    return QString();
}

CharacterSheetItem* CharacterSheet::setValue(QString key, QString value, QString formula)
{
    CharacterSheetItem* result = nullptr;

    auto item = getFieldFromKey(key);

    if(item != nullptr)
    {
        item->setFormula(formula);
        item->setValue(value);
        result = nullptr;
    }
    else
    {
        Field* field = new Field(false);
        result = field;
        field->setValue(value);
        field->setId(key);
        insertField(key,field);
    }
    return result;
}
QList<QString> CharacterSheet::getAllDependancy(QString key)
{
    QList<QString> list;
    for(auto field : m_valuesMap.values())
    {
        if(field->hasFormula())
        {
            if(field->getFormula().contains(key))
            {
               list << field->getPath();
            }
        }
    }
    return list;
}

const QString CharacterSheet::getkey(int index)
{  
    if(index==0)
    {
        return getTitle();
    }
    else
    {
        --index;
        if((index<m_valuesMap.keys().size())&&(index>=0)&&(!m_valuesMap.isEmpty()))
        {
            return m_valuesMap.keys().at(--index);
        }

    }
    return QString();
}
QStringList CharacterSheet::explosePath(QString str)
{
    return str.split('.');
}

QString CharacterSheet::getUuid() const
{
    return m_uuid;
}

void CharacterSheet::setUuid(const QString &uuid)
{
    m_uuid = uuid;
}

void CharacterSheet::setFieldData(QJsonObject &obj)
{
    QString id = obj["id"].toString();
    CharacterSheetItem* item = m_valuesMap.value(id);
    if(nullptr!=item)
    {
        item->loadDataItem(obj);
    }
}

QString CharacterSheet::getName() const
{
    return m_name;
}

void CharacterSheet::setName(const QString &name)
{
    m_name = name;
}

Section *CharacterSheet::getRootSection() const
{
    return m_rootSection;
}

void CharacterSheet::buildDataFromSection(Section *rootSection)
{
    rootSection->buildDataInto(this);
}
void CharacterSheet::save(QJsonObject& json)
{
    json["name"]= m_name;
    json["idSheet"]= m_uuid;
    QJsonObject array=QJsonObject();
    for (QString key : m_valuesMap.keys())
    {
        QJsonObject item;
        m_valuesMap[key]->saveDataItem(item);
        array[key]=item;
    }
    json["values"]=array;
}

void CharacterSheet::load(QJsonObject& json)
{
    m_name = json["name"].toString();
    m_uuid = json["idSheet"].toString();
    QJsonObject array = json["values"].toObject();
    for(QString key : array.keys() )
    {
        QJsonObject item = array[key].toObject();
        CharacterSheetItem* itemSheet=nullptr;
        if((item["type"]==QStringLiteral("field"))||(item["type"]==QStringLiteral("button")))
        {
            itemSheet = new Field();
        }
        else if(item["type"]== QStringLiteral("TableField"))
        {
            auto table = new TableField();
            itemSheet = table;
            connect(table,&TableField::lineMustBeAdded,this,[this](TableField* field){
                emit addLineToTableField(this,field);
            });
        }

        if(nullptr!=itemSheet)
        {
            itemSheet->loadDataItem(item);
            itemSheet->setId(key);
            insertField(key,itemSheet);
        }
    }
}
void CharacterSheet::setOrigin(Section* sec)
{
    for(auto key : m_valuesMap.keys())
    {
        auto value = m_valuesMap.value(key);
        if(nullptr != value)
        {
            auto field = sec->getChildAt(key);
            if(nullptr != field)
            {
                value->setOrig(field);
            }
        }
    }
}

void CharacterSheet::insertField(QString key, CharacterSheetItem* itemSheet)
{
    m_valuesMap.insert(key,itemSheet);

    connect(itemSheet,&CharacterSheetItem::sendOffData,
            this,[=](CharacterSheetItem* item){
        emit updateField(this,item);
    });
}

#if !defined(RCSE)  && !defined(UNIT_TEST)
void CharacterSheet::fill(NetworkMessageWriter & msg)
{
    QJsonObject object;
    save(object);
    QJsonDocument doc;
    doc.setObject(object);
    msg.byteArray32(doc.toBinaryData());
}
void CharacterSheet::read(NetworkMessageReader& msg)
{
    QJsonDocument doc=QJsonDocument::fromBinaryData(msg.byteArray32());
    QJsonObject object = doc.object();
    load(object);
}
#endif
QHash<QString, QString> CharacterSheet::getVariableDictionnary()
{
    QHash<QString, QString> dataDict;
    for(QString key : m_valuesMap.keys())
    {
        if(nullptr!=m_valuesMap[key])
        {
            dataDict[key] = m_valuesMap[key]->value();
            dataDict[m_valuesMap[key]->getLabel()] = m_valuesMap[key]->value();
        }
    }
    return dataDict;
}

void CharacterSheet::insertCharacterItem(CharacterSheetItem *item)
{
    if(nullptr == item)
        return;
   insertField(item->getId(),item);
}
