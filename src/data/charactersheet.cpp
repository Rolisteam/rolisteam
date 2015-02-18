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


QString Section::getName()
{
    return m_name;
}

void Section::setName(QString name)
{
    m_name=name;
}







CharacterSheet::CharacterSheet()
{
}

const QString CharacterSheet::getData(int section, const  QString& key)
{
    Q_ASSERT(m_sectionList.size()>section);
    return m_sectionList[section].value(key);

}

const  QString CharacterSheet::getTitleSection(int section)
{
    Q_ASSERT(m_sectionList.size()>section);
        return m_sectionList[section].getName();



}
int CharacterSheet::getSection(int index)
{
    int currentSection=0;
    while((currentSection<m_sectionList.size()&&(index>m_sectionList[currentSection].size()+1)))
    {
        index -=m_sectionList[currentSection].size()+1;
        currentSection++;
    }
    return currentSection;
}

const  QString CharacterSheet::getData(int index)
{

    int currentSection = getSection(index);
    if(index==0)
    {
        return getTitleSection(currentSection);
    }
    else
    {
        index--;
        QList<QString> tmp = m_sectionList[currentSection].keys();
        return getData(currentSection,tmp.at(index));
    }

}
int CharacterSheet::getIndexCount()
{
    int index =0;
    for(int i = 0;i<m_sectionList.size();i++)
    {
        index+=m_sectionList[i].size()+1;//size plus the title
    }
    return index;
}

const QString CharacterSheet::getkey(int index)
{
    int currentSection = getSection(index);

    if(index==0)
    {
        return getTitleSection(currentSection);
    }
    else
    {
        index--;
        QList<QString> tmp = m_sectionList[currentSection].keys();
        return tmp.at(index);

    }
}
void CharacterSheet::setData(int index,QVariant value,bool isHeader)
{
    if(!isHeader)
    {
            m_sectionList[getSection(index)].insert(getkey(index),value.toString());
    }
    else
    {
            m_sectionList[getSection(index)].insert(value.toString(),"");
    }

}
void CharacterSheet::appendSection()
{

}
