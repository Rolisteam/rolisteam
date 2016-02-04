/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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



//////////////////////////////////////
//CharacterSheetItem
/////////////////////////////////////
CharacterSheetItem::CharacterSheetItem()
    : m_parent(NULL)
{

}

QString CharacterSheetItem::name() const
{
    return m_name;
}

void CharacterSheetItem::setName(const QString &name)
{
    m_name = name;
}

QString CharacterSheetItem::getValue(int idColumn) const
{
    if((idColumn<m_value.size())&&(idColumn>-1)&&(!m_value.empty()))
    {
        return m_value[idColumn];
    }
    else
    {
        return QString();
    }
}

void CharacterSheetItem::setValue(QString val,int idColumn)
{
    m_value.insert(idColumn,val);
}

CharacterSheetItem *CharacterSheetItem::getParent() const
{
    return m_parent;
}

void CharacterSheetItem::setParent(CharacterSheetItem *parent)
{
    m_parent = parent;
}
QString CharacterSheetItem::getPath()
{
    QString path;
    if(NULL!=m_parent)
    {
        path=m_parent->getPath();
        path.append('.');
    }
    return path.append(m_name);
}
int CharacterSheetItem::rowInParent()
{
    return m_parent->indexOf(this);
}
bool CharacterSheetItem::isReadOnly() const
{
    return m_readOnly;
}

void CharacterSheetItem::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
}

CharacterSheetItem*  CharacterSheetItem::getChildAt(int) const
{
    return NULL;
}
//////////////////////////////////////
//Field
/////////////////////////////////////
Field::Field()
{

}
int Field::getChildrenCount()const
{
    return 0;
}
CharacterSheetItem* Field::getChildAt(QString i) const
{
    return NULL;
}
bool Field::mayHaveChildren() const
{
    return false;
}
int Field::indexOf(CharacterSheetItem* child)
{
    return 0;
}
void Field::save(QJsonObject& json)
{
    json["type"]="field";
    json["key"]=m_name;
    json["value"]=QJsonArray::fromStringList(m_value);
}

void Field::load(QJsonObject& json)
{
    m_name=json["key"].toString();
    QVariantList list = json["value"].toArray().toVariantList();

    foreach(QVariant variant, list)
    {
        m_value.append(variant.toString());
    }

}
//////////////////////////////////////
//Section
/////////////////////////////////////
Section::Section()
{
    m_name = "";
}

int Section::getChildrenCount()const
{
    return m_dataHash.size();
}

bool Section::mayHaveChildren() const
{
    return true;
}
bool  Section::appendChild(CharacterSheetItem* item)
{
    item->setParent(this);
    m_dataHash.insert(item->name(),item);
    m_keyList.append(item->name());
}
CharacterSheetItem* Section::getChildAt(int i) const
{
    QString key = m_keyList[i];
    return m_dataHash[key];
}
CharacterSheetItem*  Section::getChildAt(QString i) const
{    
    return m_dataHash.value(i);
}
int Section::indexOf(CharacterSheetItem* child)
{
    QString key = m_dataHash.key(child);
    return m_keyList.indexOf(key);
}
void Section::save(QJsonObject& json)
{
    json["name"] = m_name;
    json["type"] = QStringLiteral("Section");
    QJsonArray fieldArray;
    foreach (QString key, m_keyList)
    {
       CharacterSheetItem* item = m_dataHash[key];
       QJsonObject itemObject;
       item->save(itemObject);
       fieldArray.append(itemObject);
    }
    json["items"] = fieldArray;
}

void Section::load(QJsonObject& json)
{
    m_name = json["name"].toString();
    QJsonArray fieldArray = json["items"].toArray();
    QJsonArray::Iterator it;
    for(it = fieldArray.begin(); it != fieldArray.end(); ++it)
    {
        QJsonObject obj = (*it).toObject();
        CharacterSheetItem* item;
        if(obj["type"]==QStringLiteral("Section"))
        {
            item = new Section();
        }
        else
        {
            Field* field=new Field();
            item = field;
        }
        item->load(obj);
        item->setParent(this);
        appendChild(item);
    }
}
QStringList Section::keyList() const
{
    return m_keyList;
}

void Section::setKeyList(const QStringList &keyList)
{
    m_keyList = keyList;
}


/////////////////////////////////////
//CharacterSheet
/////////////////////////////////////////
/// \brief CharacterSheet::CharacterSheet
///

int CharacterSheet::m_count=0;
CharacterSheet::CharacterSheet()
    : m_name("Character %1")
{
    ++m_count;
    m_name.arg(m_count);
}
const  QString CharacterSheet::getTitle()
{
        return m_name;
}

const  QString CharacterSheet::getValue(QString path) const
{
    qDebug() << path << "CharacterSheet::getValue";
    if(m_values.contains(path))
    {
        return m_values.value(path);
    }
    return QString();
}

void CharacterSheet::setValue(QString key, QString value)
{
    qDebug() << key << "setValue" << value;
    m_values.insert(key,value);
    emit valuesChanged(key,value);
}
int CharacterSheet::getIndexCount()
{
    return m_values.keys().size();
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
        if((index<m_values.keys().size())&&(index>=0)&&(!m_values.isEmpty()))
        {
            return m_values.keys().at(--index);
        }

    }
    return QString();
}
QStringList CharacterSheet::explosePath(QString str)
{
    return str.split('.');
}

void CharacterSheet::save(QJsonObject& json)
{
    m_name = json["name"].toString();
    foreach (QString key, m_values)
    {
        json[key]=m_values[key];
    }
}

void CharacterSheet::load(QJsonObject& json)
{
    json["name"]= m_name;
    foreach(QString key, json.keys() )
    {
        m_values[key] = json[key].toString();
    }
}
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
