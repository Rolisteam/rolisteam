/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include "charactersheet/charactersheet.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include "charactersheet/charactersheetbutton.h"
#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/controllers/section.h"
#include "charactersheet/controllers/tablefield.h"
/////////////////////////////////////////
//          CharacterSheet           ////
/////////////////////////////////////////
/// \brief CharacterSheet::CharacterSheet
///

int CharacterSheet::m_count= 0;
CharacterSheet::CharacterSheet(QObject* parent)
    : QObject(parent), m_name("Character %1"), m_uuid(QUuid::createUuid().toString())
{
    ++m_count;
    m_name= m_name.arg(m_count);
}
CharacterSheet::~CharacterSheet()
{
    qDeleteAll(m_valuesMap);
    m_valuesMap.clear();
}

const QString CharacterSheet::getTitle()
{
    return m_name;
}

int CharacterSheet::getFieldCount() const
{
    return m_valuesMap.size();
}

int CharacterSheet::indexFromId(const QString& id) const
{
    return m_valuesMap.keys().indexOf(id);
}

CSItem* CharacterSheet::getFieldFromIndex(const std::vector<int>& row) const
{
    if(row.empty())
        return nullptr;

    size_t i= 0;
    auto index= row[i];
    auto item= getFieldAt(index);
    ++i;
    while(nullptr != item && i < row.size())
    {
        item= dynamic_cast<CSItem*>(item->childAt(index));
        ++i;
    }
    return item;
}

CSItem* CharacterSheet::getFieldAt(int i) const
{
    if(i < m_valuesMap.size() && i >= 0)
    {
        auto const& keys= m_valuesMap.keys();
        return m_valuesMap.value(keys.at(i));
    }
    return nullptr;
}

CSItem* CharacterSheet::getFieldFromKey(QString key) const
{
    QStringList keyList= key.split('.');
    if(keyList.size() > 1)
    {
        CSItem* field= m_valuesMap[keyList.takeFirst()];
        return dynamic_cast<CSItem*>(field->childFromId(keyList.takeFirst()));
    }
    else if(m_valuesMap.contains(key))
    {
        return m_valuesMap.value(key);
    }
    return nullptr;
}

const QVariant CharacterSheet::getValue(QString path, int role) const
{
    CSItem* item= getFieldFromKey(path);
    if(nullptr == item)
        return {};
    QVariant res;
    switch(role)
    {
    case Qt::DisplayRole:
        res= item->value();
        break;
    case Qt::EditRole:
    {
        QString str= item->formula();
        if(str.isEmpty())
        {
            str= item->value();
        }
        res= str;
    }
    break;
    case Qt::ToolTipRole:
        res= item->id();
        break;
    case CharacterSheetModel::FormulaRole:
        res= item->formula();
        break;
    case Qt::BackgroundRole:
        res= item->readOnly();
        break;
    }

    return res;
}

bool CharacterSheet::removeField(const QString& id)
{
    return m_valuesMap.remove(id);
}

const QVariant CharacterSheet::getValueByIndex(const std::vector<int>& row, QString path, Qt::ItemDataRole role) const
{
    Q_UNUSED(path)
    auto item= getFieldFromIndex(row); // getFieldFromKey(path);
    if(!item)
        return {};

    QVariant res;
    if(role == Qt::DisplayRole)
    {
        res= item->value();
    }
    else if(role == Qt::EditRole)
    {
        QString str= item->formula();
        if(str.isEmpty())
        {
            str= item->value();
        }
        res= str;
    }
    else if(role == Qt::UserRole)
    {
        res= item->formula();
    }
    else if(role == Qt::BackgroundRole)
    {
        res= item->readOnly() ? QColor(Qt::red) : QVariant();
    }

    return res;
}

CSItem* CharacterSheet::setValue(QString key, QString value, QString formula)
{
    CSItem* result= nullptr;

    auto item= getFieldFromKey(key);

    if(item != nullptr)
    {
        item->setFormula(formula);
        item->setValue(value);
        result= nullptr;
    }
    else
    {
        auto field= new FieldController(TreeSheetItem::FieldItem, false);
        result= field;
        field->setValue(value);
        field->setId(key);
        insertField(key, field);
    }
    return result;
}
QList<QString> CharacterSheet::getAllDependancy(QString key)
{
    QList<QString> list;
    auto const& values= m_valuesMap.values();
    for(auto& field : values)
    {
        if(field->hasFormula())
        {
            if(field->formula().contains(key))
            {
                list << field->path();
            }
        }
    }
    return list;
}

const QString CharacterSheet::getkey(int index)
{
    if(index == 0)
    {
        return getTitle();
    }
    else
    {
        --index;
        auto const& keys= m_valuesMap.keys();
        if((index < keys.size()) && (index >= 0) && (!m_valuesMap.isEmpty()))
        {
            return keys.at(--index);
        }
    }
    return QString();
}
QStringList CharacterSheet::explosePath(QString str)
{
    return str.split('.');
}

QString CharacterSheet::uuid() const
{
    return m_uuid;
}

void CharacterSheet::setUuid(const QString& uuid)
{
    if(uuid == m_uuid || uuid.isEmpty())
        return;

    m_uuid= uuid;
    emit uuidChanged();
}

QString CharacterSheet::name() const
{
    return m_name;
}

void CharacterSheet::setName(const QString& name)
{
    if(name == m_name)
        return;

    m_name= name;
    emit nameChanged();
}

void CharacterSheet::setFieldData(const QJsonObject& obj, const QString& parent)
{
    QString id= obj["id"].toString();
    TreeSheetItem* value= m_valuesMap.value(id);
    if(nullptr != value)
    {
        value->loadDataItem(obj);
    }
    else
    {
        auto item= m_valuesMap[parent];
        if(nullptr == item)
            return;
        auto table= dynamic_cast<TableFieldController*>(item);
        // TODO Make setChildFieldData part of TreeSheetItem to make this algorithem generic
        if(table)
        {
            table->setChildFieldData(obj);
        }
    }
}

void CharacterSheet::save(QJsonObject& json) const
{
    json["name"]= m_name;
    json["idSheet"]= m_uuid;
    QJsonObject array= QJsonObject();
    auto const& keys= m_valuesMap.keys();
    for(const QString& key : keys)
    {
        QJsonObject item;
        m_valuesMap[key]->saveDataItem(item);
        array[key]= item;
    }
    json["values"]= array;
}

void CharacterSheet::load(const QJsonObject& json)
{
    setName(json["name"].toString());
    setUuid(json["idSheet"].toString());
    QJsonObject array= json["values"].toObject();
    for(auto& key : array.keys())
    {
        QJsonObject item= array[key].toObject();
        CSItem* itemSheet= nullptr;
        if((item["type"] == QStringLiteral("field")) || (item["type"] == QStringLiteral("button")))
        {
            itemSheet= new FieldController(TreeSheetItem::FieldItem, true);
        }
        else if(item["type"] == QStringLiteral("TableField"))
        {
            auto table= new TableFieldController();
            itemSheet= table;
            connect(table, &TableFieldController::lineMustBeAdded, this,
                    [this](TableFieldController* field) { emit addLineToTableField(this, field); });
        }

        if(nullptr != itemSheet)
        {
            itemSheet->loadDataItem(item);
            itemSheet->setId(key);
            insertField(key, itemSheet);
        }
    }
}
void CharacterSheet::setOrigin(Section* sec)
{
    for(auto it= m_valuesMap.keyValueBegin(); it != m_valuesMap.keyValueEnd(); ++it)
    {
        if(nullptr == it->second)
            continue;

        auto field= sec->childFromId(it->first);
        if(nullptr == field)
            continue;

        it->second->setOrig(field);
    }
}

void CharacterSheet::insertField(QString key, CSItem* itemSheet)
{
    m_valuesMap.insert(key, itemSheet);

    connect(itemSheet, &TreeSheetItem::characterSheetItemChanged, this,
            [itemSheet, this](TreeSheetItem* item)
            {
                QString path;
                auto parent= item->parentTreeItem();
                if(nullptr != parent)
                    path= parent->path();

                emit updateField(this, itemSheet, path);
            });

    connect(itemSheet, &CSItem::valueChanged, this, [this](const QString& id) { emit fieldValueChanged(this, id); });
}

QHash<QString, QString> CharacterSheet::getVariableDictionnary()
{
    QHash<QString, QString> dataDict;
    auto const& keys= m_valuesMap.keys();
    for(const QString& key : keys)
    {
        if(nullptr != m_valuesMap[key])
        {
            m_valuesMap[key]->setFieldInDictionnary(dataDict);
        }
    }
    return dataDict;
}

void CharacterSheet::insertCharacterItem(CSItem* item)
{
    if(nullptr == item)
        return;
    insertField(item->id(), item);
}
