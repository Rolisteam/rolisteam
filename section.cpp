/***************************************************************************
* Copyright (C) 2016 by Renaud Guezennec                                   *
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
#include "section.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QGraphicsScene>

#include "field.h"
#include "charactersheetbutton.h"

#include <QDebug>

Section::Section()
{

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
    if((i<getChildrenCount())&&(i>=0))
    {
        return m_dataHash.value(m_keyList.at(i));
    }
    return NULL;
}
CharacterSheetItem* Section::getChildAt(QString key) const
{
    return m_dataHash.value(key);
}
QVariant Section::getValueFrom(CharacterSheetItem::ColumnId id) const
{
    if(CharacterSheetItem::ID==id)
        return m_id;
    if(CharacterSheetItem::VALUE==id)
        return m_value;
    return QVariant();
}

void Section::setValueFrom(CharacterSheetItem::ColumnId id, QVariant var)
{
    if(CharacterSheetItem::ID==id)
        m_id = var.toString();
    if(CharacterSheetItem::VALUE==id)
        m_value = var.toString();
}

bool Section::mayHaveChildren()
{
    return true;
}

void Section::appendChild(CharacterSheetItem* item)
{
    m_dataHash.insert(item->getPath(),item);
    m_keyList.append(item->getPath());
    item->setParent(this);
}
int Section::indexOfChild(CharacterSheetItem* itm)
{
    return m_keyList.indexOf(itm->getPath());
}
QString Section::getName() const
{
    return m_name;
}

void Section::setName(const QString &name)
{
    m_name = name;
}

void Section::save(QJsonObject& json,bool exp)
{
    json["name"] = m_name;
    json["type"] = QStringLiteral("Section");
    QJsonArray fieldArray;
    foreach (QString key, m_keyList)
    {
        CharacterSheetItem* item = m_dataHash.value(key);
       QJsonObject itemObject;
       item->save(itemObject,exp);
       fieldArray.append(itemObject);
    }
    json["items"] = fieldArray;
}

void Section::load(QJsonObject &json,QList<QGraphicsScene*> scenes)
{
    m_name = json["name"].toString();
    QJsonArray fieldArray = json["items"].toArray();
    QJsonArray::Iterator it;
    for(it = fieldArray.begin(); it != fieldArray.end(); ++it)
    {
        QJsonObject obj = (*it).toObject();
        CharacterSheetItem* item;
        QGraphicsItem* gItem=NULL;
        if(obj["type"]==QStringLiteral("Section"))
        {
            item = new Section();
        }
        if(obj["type"]==QStringLiteral("button"))
        {
            CharacterSheetButton* btn = new CharacterSheetButton();
            item = btn;
            gItem = btn;

        }
        else
        {
            Field* field=new Field();
            item = field;
            gItem = field;
        }
        item->load(obj,scenes);
        if(scenes.size()>item->getPage())
        {
            QGraphicsScene* scene = scenes.at(item->getPage());
            if((NULL!=scene)&&(NULL!=gItem))
            {
                scene->addItem(gItem);
            }
        }
        item->setParent(this);
        m_dataHash.insert(item->getPath(),item);
        m_keyList.append(item->getPath());
    }
}
void Section::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec)
{
    foreach (CharacterSheetItem* item, m_dataHash.values())
    {
        item->generateQML(out,sec);
    }
}

CharacterSheetItem::CharacterSheetItemType Section::getItemType() const
{
    return CharacterSheetItem::SectionItem;
}

void Section::copySection(Section* oldSection)
{
    for(int i = 0; i < oldSection->getChildrenCount();++i)
    {
        CharacterSheetItem* childItem = oldSection->getChildAt(i);
        if(NULL!=childItem)
        {
            CharacterSheetItem* newItem = NULL;
            if(CharacterSheetItem::FieldItem == childItem->getItemType())
            {
                Field* newField = new Field();
                newField->copyField(childItem);
                newItem = newField;
            }
            if(CharacterSheetItem::ButtonItem == childItem->getItemType())
            {
                CharacterSheetButton* newField = new CharacterSheetButton();
                newField->copyField(childItem);
                newItem = newField;
            }
            if(CharacterSheetItem::SectionItem == childItem->getItemType())
            {
                Section* sec = new Section();
                sec->copySection(dynamic_cast<Section*>(childItem));
                newItem = sec;
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
        delete child;
       return true;
    }
    return false;
}


void Section::buildDataInto( CharacterSheet* character)
{
    for(int i = 0; i< getChildrenCount();++i)
    {
        CharacterSheetItem* childItem = getChildAt(i);
        if(NULL!=childItem)
        {
            CharacterSheetItem* newItem = NULL;
            if(CharacterSheetItem::FieldItem == childItem->getItemType())
            {
                Field* newField = new Field();
                newField->copyField(childItem);
                newItem = newField;
            }
            if(CharacterSheetItem::ButtonItem == childItem->getItemType())
            {
                CharacterSheetButton* newField = new CharacterSheetButton();
                newField->copyField(childItem);
                newItem = newField;
            }
            if(NULL!=newItem)
            {
                newItem->setValue(character->getValue(newItem->getId()));
                character->insertCharacterItem(newItem);
            }
            if(CharacterSheetItem::SectionItem == childItem->getItemType())
            {
                Section* sec = dynamic_cast<Section*>(childItem);
                sec->buildDataInto(character);
            }
        }
    }
}
void Section::setValueForAll(CharacterSheetItem* itemSrc,int col)
{
    for(auto key : m_keyList)
    {
        CharacterSheetItem* item = m_dataHash.value(key);
        if(NULL!=item)
        {
            item->setValueFrom((ColumnId)col,itemSrc->getValueFrom((ColumnId)col));
        }
    }
}

void Section::saveDataItem(QJsonObject &json)
{
    save(json);
}

void Section::loadDataItem(QJsonObject &json)
{
    load(json,QList<QGraphicsScene*>());
}
