/***************************************************************************
 * Copyright (C) 2016 by Renaud Guezennec                                   *
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
#include "charactersheet/controllers/section.h"
#include <QGraphicsScene>
#include <QJsonArray>
#include <QJsonObject>

#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/tablefield.h"
#include "charactersheet/fieldfactory.h"
#include <QDebug>
#include <utils/directaccessdatastructure.h>

Section::Section() : TreeSheetItem(TreeSheetItem::SectionItem) {}
Section::~Section()
{
    qDeleteAll(m_data.values<QList<TreeSheetItem*>>());
    m_data.clear();
}

bool Section::hasChildren()
{
    return !m_data.isEmpty();
}

int Section::childrenCount() const
{
    return m_data.size();
}

TreeSheetItem* Section::childAt(int i) const
{
    if((i < childrenCount()) && (i >= 0))
    {
        return m_data[i];
    }
    return nullptr;
}
TreeSheetItem* Section::childFromId(const QString& key) const
{
    return m_data.value(key);
}

bool Section::mayHaveChildren() const
{
    return true;
}

void Section::appendChild(TreeSheetItem* item)
{
    if(nullptr == item)
        return;
    m_data.append(item->path(), item);
    item->setParent(this);
}
void Section::insertChild(TreeSheetItem* item, int pos)
{
    if(nullptr == item)
        return;

    m_data.append(item->path(), item);
    item->setParent(this);
}
int Section::indexOfChild(TreeSheetItem* item)
{
    if(nullptr == item)
        return -1;

    return m_data.index(item->path());
}

void Section::save(QJsonObject& json, bool exp)
{
    json["name"]= m_id;
    json["type"]= QStringLiteral("Section");
    QJsonArray fieldArray;
    for(auto const& key : m_data)
    {
        TreeSheetItem* item= key.second;
        if(nullptr == item)
            continue;

        QJsonObject itemObject;
        item->save(itemObject, exp);
        fieldArray.append(itemObject);
    }
    json["items"]= fieldArray;
    qDebug() << json;
}

QList<CSItem*> Section::allChildren() const
{
    QList<CSItem*> res;
    for(auto key : m_data)
    {
        auto item= key.second;
        switch(item->itemType())
        {
        case TreeSheetItem::SectionItem:
        {
            auto sec= dynamic_cast<Section*>(item);
            if(sec)
                res << sec->allChildren();
        }
        break;
        case TreeSheetItem::TableItem:
        case TreeSheetItem::SliderItem:
        case TreeSheetItem::FieldItem:
        {
            auto csItem= dynamic_cast<CSItem*>(item);
            if(csItem)
                res << csItem;
        }
        break;
        }
    }

    return res;
}

void Section::setFieldInDictionnary(QHash<QString, QString>& dict) const
{
    for(auto value : m_data)
    {
        value.second->setFieldInDictionnary(dict);
    }
}

void Section::load(const QJsonObject& json)
{
    m_id= json["name"].toString();
    QJsonArray fieldArray= json["items"].toArray();
    QJsonArray::Iterator it;
    for(it= fieldArray.begin(); it != fieldArray.end(); ++it)
    {
        QJsonObject obj= (*it).toObject();
        TreeSheetItem* item;
        if(obj["type"] == QStringLiteral("Section"))
        {
            auto section= new Section();
            connect(section, &Section::addLineToTableField, this, &Section::addLineToTableField);
            item= section;
            item->load(obj);
        }
        else if(obj["type"] == QStringLiteral("TableField"))
        {
            TableFieldController* field= new TableFieldController();
            connect(field, &TableFieldController::lineMustBeAdded, this, &Section::addLineToTableField);
            field->init();
            item= field;
            item->load(obj);
        }
        else
        {
            auto field= new FieldController(TreeSheetItem::FieldItem, true);
            item= field;
            item->load(obj);
        }
        if(!m_data.contains(item->path()))
        {
            item->setParent(this);
            m_data.append(item->path(), item);
        }
        else
            qDebug() << "Dupplicate found" << item->path();
    }
}

void Section::copySection(Section* oldSection)
{
    if(nullptr == oldSection)
        return;

    setOrig(oldSection);
    for(int i= 0; i < oldSection->childrenCount(); ++i)
    {
        TreeSheetItem* childItem= oldSection->childAt(i);
        if(nullptr != childItem)
        {
            TreeSheetItem* newItem= nullptr;
            if(TreeSheetItem::FieldItem == childItem->itemType())
            {
                auto newField= new FieldController(TreeSheetItem::FieldItem, false);
                newField->copyField(childItem, false);
                newItem= newField;
            }

            if(TreeSheetItem::SectionItem == childItem->itemType())
            {
                Section* sec= new Section();
                sec->copySection(dynamic_cast<Section*>(childItem));

                newItem= sec;
            }
            appendChild(newItem);
        }
    }
}

bool Section::removeChild(TreeSheetItem* child)
{
    if(m_data.contains(child->id()))
    {
        m_data.remove(child->id());
        return true;
    }
    return false;
}

bool Section::deleteChild(TreeSheetItem* child)
{
    if(removeChild(child))
    {
        delete child;
        return true;
    }
    return false;
}
void Section::removeAll()
{
    m_data.clear();
}
void Section::resetAllId(int& i)
{
    QString id("id_%1");

    for(auto const& pair : m_data)
    {
        TreeSheetItem* item= pair.second;

        if(TreeSheetItem::FieldItem == item->itemType())
        {
            ++i;
            item->setId(id.arg(i));
        }
        else if(TreeSheetItem::SectionItem == item->itemType())
        {
            Section* sec= dynamic_cast<Section*>(item);
            if(sec)
                sec->resetAllId(i);
        }
    }
}

void Section::setOrig(TreeSheetItem* orig)
{
    for(auto const& key : m_data)
    {
        auto value= key.second;
        if(nullptr != value)
        {
            auto field= orig->childFromId(key.first);
            if(nullptr != field)
            {
                value->setOrig(field);
            }
        }
    }
}
void Section::changeKeyChild(const QString& oldkey, const QString& newKey, TreeSheetItem* child)
{
    m_data.changeKey(oldkey, newKey, child);
}
void Section::buildDataInto(CharacterSheet* character)
{
    QStringList ids;
    for(auto it= m_data.orderedBegin(); it != m_data.orderedEnd(); ++it)
    {
        TreeSheetItem* childItem= m_data.value((*it));
        auto path= childItem->path();
        auto field= character->getFieldFromKey(path);

        auto csItem= dynamic_cast<CSItem*>(childItem);
        if(csItem)
            ids << csItem->id();

        if(nullptr == childItem || nullptr != field)
            continue;

        auto newItem= FieldFactory::createField(childItem->itemType(), true);

        QString newId;
        switch(childItem->itemType())
        {
        case TreeSheetItem::SliderItem:
        case TreeSheetItem::FieldItem:
        case TreeSheetItem::TableItem:
        {
            auto field= dynamic_cast<FieldController*>(newItem);
            if(field)
            {
                newId= field->id();
                field->copyField(childItem, false);
                field->setValue(character->getValue(newId).toString());
                character->insertCharacterItem(field);
            }
        }
        break;
        case TreeSheetItem::SectionItem:
        {
            Section* sec= dynamic_cast<Section*>(childItem);
            if(sec)
            {
                sec->buildDataInto(character);
                newItem= nullptr;
            }
        }
        break;
        }
    }

    for(int i= character->getFieldCount() - 1; i >= 0; --i)
    {
        auto child= character->getFieldAt(i);
        auto item= dynamic_cast<CSItem*>(child);
        if(nullptr == item)
            continue;
        auto id= item->id();
        if(!ids.contains(id))
        {
            character->removeField(id);
        }
    }
}
void Section::setValueForAll(TreeSheetItem* itemSrc, int col)
{
    auto source= dynamic_cast<CSItem*>(itemSrc);
    for(auto const pair : m_data)
    {
        auto item= dynamic_cast<CSItem*>(pair.second);
        if(nullptr == item)
            continue;

        item->setValueFrom(static_cast<ColumnId>(col), source->valueFrom(static_cast<ColumnId>(col), Qt::DisplayRole));
    }
}

/*void Section::saveDataItem(QJsonObject& json)
{
    save(json);
}

void Section::loadDataItem(const QJsonObject& json)
{
    load(json);
}*/

QList<CSItem*> Section::fieldFromPage(int pagePos)
{
    QList<CSItem*> res;
    for(auto pair : m_data)
    {
        auto child= pair.second;
        if(child->itemType() != SectionItem)
        {
            auto csItem= dynamic_cast<CSItem*>(child);
            if(pagePos == csItem->page())
                res.append(csItem);
        }
        else
        {
            auto childSection= dynamic_cast<Section*>(child);
            if(childSection)
                res.append(childSection->fieldFromPage(pagePos));
        }
    }
    return res;
}
