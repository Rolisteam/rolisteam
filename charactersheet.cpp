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
