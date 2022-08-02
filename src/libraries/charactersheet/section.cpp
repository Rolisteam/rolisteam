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
#include "charactersheet/section.h"
#include <QGraphicsScene>
#include <QJsonArray>
#include <QJsonObject>

#include "charactersheet/field.h"
#include "charactersheet/tablefield.h"

#include <QDebug>

Section::Section() : CharacterSheetItem(CharacterSheetItem::SectionItem) {}
Section::~Section()
{
    qDeleteAll(m_dataHash);
}

bool Section::hasChildren()
{
    return !m_dataHash.isEmpty();
}

int Section::getChildrenCount() const
{
    return m_dataHash.size();
}

CharacterSheetItem* Section::getChildAt(int i) const
{
    if((i < getChildrenCount()) && (i >= 0))
    {
        return m_dataHash.value(m_keyList.at(i));
    }
    return nullptr;
}
CharacterSheetItem* Section::getChildFromId(const QString& key) const
{
    return m_dataHash.value(key);
}
QVariant Section::getValueFrom(CharacterSheetItem::ColumnId id, int role) const
{
    Q_UNUSED(role);
    if(CharacterSheetItem::ID == id)
        return m_id;
    if(CharacterSheetItem::VALUE == id)
        return m_value;
    return QVariant();
}

void Section::setValueFrom(CharacterSheetItem::ColumnId id, QVariant var)
{
    if(CharacterSheetItem::ID == id)
        m_id= var.toString();
    if(CharacterSheetItem::VALUE == id)
        m_value= var.toString();
}

bool Section::mayHaveChildren() const
{
    return true;
}

void Section::appendChild(CharacterSheetItem* item)
{
    if(nullptr == item)
        return;
    m_dataHash.insert(item->getPath(), item);
    m_keyList.append(item->getPath());
    item->setParent(this);
}
void Section::insertChild(CharacterSheetItem* item, int pos)
{
    if(nullptr == item)
        return;

    m_dataHash.insert(item->getPath(), item);
    m_keyList.insert(pos, item->getPath());
    item->setParent(this);
}
int Section::indexOfChild(CharacterSheetItem* item)
{
    if(nullptr == item)
        return -1;

    return m_keyList.indexOf(item->getPath());
}
QString Section::getName() const
{
    return m_name;
}

void Section::setName(const QString& name)
{
    m_name= name;
}

void Section::save(QJsonObject& json, bool exp)
{
    json["name"]= m_name;
    json["type"]= QStringLiteral("Section");
    QJsonArray fieldArray;
    for(auto& key : m_keyList)
    {
        CharacterSheetItem* item= m_dataHash.value(key);
        if(nullptr == item)
            continue;

        QJsonObject itemObject;
        item->save(itemObject, exp);
        fieldArray.append(itemObject);
    }
    json["items"]= fieldArray;
    qDebug() << json;
}

void Section::load(const QJsonObject& json)
{
    m_name= json["name"].toString();
    QJsonArray fieldArray= json["items"].toArray();
    QJsonArray::Iterator it;
    for(it= fieldArray.begin(); it != fieldArray.end(); ++it)
    {
        QJsonObject obj= (*it).toObject();
        CharacterSheetItem* item;
        if(obj["type"] == QStringLiteral("Section"))
        {
            auto section= new Section();
            connect(section, &Section::addLineToTableField, this, &Section::addLineToTableField);
            item= section;
            item->load(obj);
        }
        else if(obj["type"] == QStringLiteral("TableField"))
        {
            TableField* field= new TableField();
            connect(field, &TableField::lineMustBeAdded, this, &Section::addLineToTableField);
            item= field;
            item->load(obj);
            // gItem= field->getCanvasField();
        }
        else
        {
            auto field= new FieldController(CharacterSheetItem::FieldItem, true);
            item= field;
            item->load(obj);
            // gItem= field->getCanvasField();
        }
        if(!m_dataHash.contains(item->getPath()))
        {
            item->setParent(this);
            /*#ifdef RCSE
                        auto page= std::max(0, item->getPage()); // add item for all pages on the first canvas.
                        if(ctrl)
                            ctrl->addItem(page, gItem);
            #else
                        Q_UNUSED(gItem);
            #endif*/
            // item->initGraphicsItem();
            m_dataHash.insert(item->getPath(), item);
            m_keyList.append(item->getPath());
        }
        else
            qDebug() << "Duplicate found" << item->getPath();
    }
}

void Section::copySection(Section* oldSection)
{
    if(nullptr == oldSection)
        return;

    setOrig(oldSection);
    for(int i= 0; i < oldSection->getChildrenCount(); ++i)
    {
        CharacterSheetItem* childItem= oldSection->getChildAt(i);
        if(nullptr != childItem)
        {
            CharacterSheetItem* newItem= nullptr;
            if(CharacterSheetItem::FieldItem == childItem->itemType())
            {
                auto newField= new FieldController(CharacterSheetItem::FieldItem, false);
                newField->copyField(childItem, false);
                newItem= newField;
            }

            if(CharacterSheetItem::SectionItem == childItem->itemType())
            {
                Section* sec= new Section();
                sec->copySection(dynamic_cast<Section*>(childItem));

                newItem= sec;
            }
            appendChild(newItem);
        }
    }
}

bool Section::removeChild(CharacterSheetItem* child)
{
    if(m_dataHash.contains(child->getId()))
    {
        m_dataHash.remove(child->getId());
        m_keyList.removeOne(child->getId());
        return true;
    }
    return false;
}

bool Section::deleteChild(CharacterSheetItem* child)
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
    m_dataHash.clear();
    m_keyList.clear();
}
void Section::resetAllId(int& i)
{
    QString id("id_%1");

    for(auto& key : m_keyList)
    {
        CharacterSheetItem* item= m_dataHash.value(key);

        if(CharacterSheetItem::FieldItem == item->itemType())
        {
            ++i;
            item->setId(id.arg(i));
        }
        else if(CharacterSheetItem::SectionItem == item->itemType())
        {
            Section* sec= dynamic_cast<Section*>(item);
            if(sec)
                sec->resetAllId(i);
        }
    }
}

void Section::setOrig(CharacterSheetItem* orig)
{
    CharacterSheetItem::setOrig(orig);
    auto const& keys= m_dataHash.keys();
    for(auto& key : keys)
    {
        auto value= m_dataHash.value(key);
        if(nullptr != value)
        {
            auto field= orig->getChildFromId(key);
            if(nullptr != field)
            {
                value->setOrig(field);
            }
        }
    }
}
void Section::changeKeyChild(QString oldkey, QString newKey, CharacterSheetItem* child)
{
    m_dataHash.remove(oldkey);
    m_dataHash.insert(newKey, child);

    auto index= m_keyList.indexOf(oldkey);
    m_keyList.removeAt(index);
    m_keyList.insert(index, newKey);
}
void Section::buildDataInto(CharacterSheet* character)
{
    QStringList ids;
    for(int i= 0; i < getChildrenCount(); ++i)
    {
        CharacterSheetItem* childItem= getChildAt(i);
        auto path= childItem->getPath();
        auto field= character->getFieldFromKey(path);
        ids << childItem->getId();
        if(nullptr != childItem && nullptr == field)
        {
            CharacterSheetItem* newItem= nullptr;
            if(CharacterSheetItem::FieldItem == childItem->itemType())
            {
                auto newField= new FieldController(CharacterSheetItem::FieldItem, false);
                newField->copyField(childItem, false);
                newItem= newField;
            }
            else if(CharacterSheetItem::TableItem == childItem->itemType())
            {
                TableField* tablefield= new TableField(false);
                tablefield->copyField(childItem, false);
                newItem= tablefield;
            }

            if(nullptr != newItem)
            {
                newItem->setValue(character->getValue(newItem->getId()).toString());
                character->insertCharacterItem(newItem);
            }
            if(CharacterSheetItem::SectionItem == childItem->itemType())
            {
                Section* sec= dynamic_cast<Section*>(childItem);
                sec->buildDataInto(character);
            }
        }
    }

    for(int i= character->getFieldCount() - 1; i >= 0; --i)
    {
        auto child= character->getFieldAt(i);
        auto id= child->getId();
        if(!ids.contains(id))
        {
            character->removeField(id);
        }
    }
}
void Section::setValueForAll(CharacterSheetItem* itemSrc, int col)
{
    for(auto& key : m_keyList)
    {
        CharacterSheetItem* item= m_dataHash.value(key);
        if(nullptr != item)
        {
            item->setValueFrom(static_cast<ColumnId>(col),
                               itemSrc->getValueFrom(static_cast<ColumnId>(col), Qt::DisplayRole));
        }
    }
}

void Section::saveDataItem(QJsonObject& json)
{
    save(json);
}

void Section::loadDataItem(const QJsonObject& json)
{
    load(json);
}
void Section::getFieldFromPage(int pagePos, QList<CharacterSheetItem*>& list)
{
    for(int i= getChildrenCount() - 1; i >= 0; --i)
    {
        auto child= getChildAt(i);
        if(child->itemType() != SectionItem)
        {
            if(pagePos == child->page())
            {
                list.append(child);
            }
        }
        else
        {
            auto childSection= dynamic_cast<Section*>(child);
            if(childSection)
                childSection->getFieldFromPage(pagePos, list);
        }
    }
}
