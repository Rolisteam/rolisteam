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
#include "tablefield.h"
#include "charactersheetbutton.h"

#include <QDebug>

Section::Section()
{

}
Section::~Section()
{
    qDeleteAll(m_dataHash.values());
    m_dataHash.clear();
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
    return nullptr;
}
CharacterSheetItem* Section::getChildAt(QString key) const
{
    return m_dataHash.value(key);
}
QVariant Section::getValueFrom(CharacterSheetItem::ColumnId id,int role) const
{
    Q_UNUSED(role);
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
void Section::insertChild(CharacterSheetItem* item,int pos)
{
    m_dataHash.insert(item->getPath(),item);
    m_keyList.insert(pos,item->getPath());
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
        if(nullptr != item)
        {
            QJsonObject itemObject;
            item->save(itemObject,exp);
            fieldArray.append(itemObject);
        }
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
        QGraphicsItem* gItem=nullptr;
        if(obj["type"]==QStringLiteral("Section"))
        {
            item = new Section();
            item->load(obj,scenes);
        }
        else if(obj["type"]==QStringLiteral("TableField"))
        {
            TableField* field=new TableField();
            item = field;
            item->load(obj,scenes);
            gItem = field->getCanvasField();
        }
        else
        {
            Field* field=new Field();
            item = field;
            item->load(obj,scenes);
            gItem = field->getCanvasField();
        }
        if(scenes.size()>item->getPage())
        {
            QGraphicsScene* scene = scenes.at(item->getPage());
            if((nullptr!=scene)&&(nullptr!=gItem))
            {
                scene->addItem(gItem);
                item->initGraphicsItem();
            }
        }
        item->setParent(this);
        m_dataHash.insert(item->getPath(),item);
        m_keyList.append(item->getPath());
    }
}
void Section::generateQML(QTextStream &out,CharacterSheetItem::QMLSection sec,int i, bool isTable)
{
    //for(CharacterSheetItem* item: m_dataHash.values())
    for(auto key : m_keyList)
    {
        CharacterSheetItem* item = m_dataHash.value(key);
        item->generateQML(out,sec,i, isTable);
    }
}

CharacterSheetItem::CharacterSheetItemType Section::getItemType() const
{
    return CharacterSheetItem::SectionItem;
}

void Section::copySection(Section* oldSection)
{
    setOrig(oldSection);
    for(int i = 0; i < oldSection->getChildrenCount();++i)
    {
        CharacterSheetItem* childItem = oldSection->getChildAt(i);
        if(nullptr!=childItem)
        {
            CharacterSheetItem* newItem = nullptr;
            if(CharacterSheetItem::FieldItem == childItem->getItemType())
            {
                Field* newField = new Field(false);
                newField->copyField(childItem,false);
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
       return true;
    }
    return false;
}

bool Section::deleteChild(CharacterSheetItem * child)
{
    if(removeChild(child))
    {
        delete child;
        return true;
    }
    return false;
}
void  Section::removeAll()
{
    m_dataHash.clear();
    m_keyList.clear();
}
void Section::resetAllId(int& i)
{
  QString id("id_%1");

  for(QString key : m_keyList)
  {
      CharacterSheetItem* item = m_dataHash.value(key);

      if(CharacterSheetItem::FieldItem == item->getItemType())
      {
          ++i;
          item->setId(id.arg(i));
      }
      else if(CharacterSheetItem::SectionItem == item->getItemType())
      {
          Section* sec = dynamic_cast<Section*>(item);
          sec->resetAllId(i);
      }
  }
}

void Section::setOrig(CharacterSheetItem* orig)
{
    CharacterSheetItem::setOrig(orig);
    for(auto key : m_dataHash.keys())
    {
        auto value = m_dataHash.value(key);
        if(nullptr != value)
        {
            auto field = orig->getChildAt(key);
            if(nullptr != field)
            {
                value->setOrig(field);
            }
        }
    }
}
void Section::changeKeyChild(QString oldkey, QString newKey, CharacterSheetItem *child)
{
    m_dataHash.remove(oldkey);
    m_dataHash.insert(newKey,child);


    auto index = m_keyList.indexOf(oldkey);
    m_keyList.removeAt(index);
    m_keyList.insert(index,newKey);
}
void Section::buildDataInto( CharacterSheet* character)
{
    for(int i = 0; i< getChildrenCount();++i)
    {
        CharacterSheetItem* childItem = getChildAt(i);
        if(nullptr!=childItem)
        {
            CharacterSheetItem* newItem = nullptr;
            if(CharacterSheetItem::FieldItem == childItem->getItemType())
            {
                Field* newField = new Field(false);
                newField->copyField(childItem,false);
                newItem = newField;
            }
            if(nullptr!=newItem)
            {
                newItem->setValue(character->getValue(newItem->getId()).toString());
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
        if(nullptr!=item)
        {
            item->setValueFrom((ColumnId)col,itemSrc->getValueFrom((ColumnId)col,Qt::DisplayRole));
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
void Section::getFieldFromPage(int pagePos, QList<CharacterSheetItem*>& list)
{
    for(int i = getChildrenCount()-1;i>=0;--i)
    {
      auto child = getChildAt(i);
      if(child->getItemType()!=SectionItem)
      {
          if(pagePos == child->getPage())
          {
              list.append(child);
          }
      }
      else
      {
        auto childSection = dynamic_cast<Section*>(child);
        childSection->getFieldFromPage(pagePos,list);
      }

    }
}
