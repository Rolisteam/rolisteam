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


#include "section.h"
#include "charactersheetbutton.h"
/////////////////////////////////////
//CharacterSheet
/////////////////////////////////////////
/// \brief CharacterSheet::CharacterSheet
///

int CharacterSheet::m_count=0;
CharacterSheet::CharacterSheet()
    : m_name("Character %1"),m_rootSection(NULL)
{
    ++m_count;
    m_name = m_name.arg(m_count);
}
const  QString CharacterSheet::getTitle()
{
    return m_name;
}

int CharacterSheet::getFieldCount()
{
    return m_valuesMap.size();
}

CharacterSheetItem* CharacterSheet::getFieldAt(int i)
{
    return m_valuesMap.value(m_valuesMap.keys().at(i));
}

const  QString CharacterSheet::getValue(QString path,Qt::ItemDataRole role) const
{
    if(m_valuesMap.contains(path))
    {
        if(role == Qt::DisplayRole)
        {
            return m_valuesMap.value(path)->value();
        }
        else
        {
            return m_valuesMap.value(path)->getFormula();
        }
    }
    return QString();
}

void CharacterSheet::setValue(QString key, QString value)
{
    if(m_valuesMap.contains(key))
    {
        CharacterSheetItem* field = m_valuesMap.value(key);
        if(value.startsWith('='))
        {
            field->setFormula(value);
        }
        else
        {
            field->setValue(value);
        }
    }
    else
    {
        Field* field = new Field();
        field->setValue(value);
        field->setId(key);
        m_valuesMap.insert(key,field);
    }
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

void CharacterSheet::setRootSection(Section *rootSection)
{
    m_rootSection = rootSection;
}
void CharacterSheet::save(QJsonObject& json)
{
    json["name"]= m_name;
    QJsonObject array=QJsonObject();
    foreach (QString key, m_valuesMap.keys())
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
    QJsonObject array = json["values"].toObject();
    for(QString key : array.keys() )
    {
        QJsonObject item = array[key].toObject();
        CharacterSheetItem* itemSheet=NULL;
        if(item["type"]==QStringLiteral("field"))
        {
            itemSheet = new Field();
        }
        else if(item["type"]==QStringLiteral("button"))
        {
            itemSheet = new CharacterSheetButton();
        }
        if(NULL!=itemSheet)
        {
            itemSheet->loadDataItem(item);
            itemSheet->setId(key);
            m_valuesMap.insert(key,itemSheet);
        }
    }
}
#ifndef RCSE
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
        if(NULL!=m_valuesMap[key])
        {
            dataDict[key] = m_valuesMap[key]->value();
            dataDict[m_valuesMap[key]->getLabel()] = m_valuesMap[key]->value();
        }
    }
    qDebug() << dataDict;
    return dataDict;
}
