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

QString CharacterSheetItem::getValue() const
{
    return m_value;
}

void CharacterSheetItem::setValue(QString val)
{
    m_value=val;
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
/////////////////////////////////////
//CharacterSheet
/////////////////////////////////////////


CharacterSheet::CharacterSheet()
{
}
const  QString CharacterSheet::getTitleChild(int section)
{
    Q_ASSERT(m_rootChild->getChildrenCount()>section);
    return m_rootChild->getChildAt(section)->name();
}
const QString CharacterSheet::getChildrenValue(int section)
{
    Q_ASSERT(m_rootChild->getChildrenCount()>section);
    return m_rootChild->getChildAt(section)->getValue();
}
void CharacterSheet::setChildrenValue(int section,QString& value)
{
    Q_ASSERT(m_rootChild->getChildrenCount()>section);
    m_rootChild->getChildAt(section)->setValue(value);
}

int CharacterSheet::getChildAt(int index)
{
    /*int currentSection=0;
    while((currentSection<m_children.size()&&(index>m_children[currentSection].size()+1)))
    {
        index -=m_sectionList[currentSection].size()+1;
        ++currentSection;
    }*/
    return 0;
}

const  QString CharacterSheet::getData(QString path)
{
    return QString();
    /// @todo implement the access though path.
   /* int currentSection = getChildAt(index);
    if(index==0)
    {
        return getTitleChild(currentSection);
    }
    else
    {
        index--;
        //QList<QString> tmp = ;
        return m_children[currentSection].at(index);
    }*/
    
}
int CharacterSheet::getIndexCount()
{
    int index =0;
//    for(int i = 0;i<m_children.size();i++)
//    {
//        index+=m_children[i].size()+1;//size plus the title
//    }
    return index;
}

const QString CharacterSheet::getkey(int index)
{
    int currentSection = getChildAt(index);
    
    if(index==0)
    {
        return getTitleChild(currentSection);
    }
    else
    {
        index--;
        //  QList<QString> tmp = m_sectionList[currentSection].keys();
        //return tmp.at(index);
        return QString();

    }
}
void  CharacterSheet::setOwner(QString owner)
{
    m_owner = owner;
}

const QString& CharacterSheet::owner() const
{
    return m_owner;
}

CharacterSheetItem *CharacterSheet::getRootChild() const
{
    return m_rootChild;
}

void CharacterSheet::setRootChild(CharacterSheetItem *rootChild)
{
    m_rootChild = rootChild;
}

void CharacterSheet::setData(QString path,QVariant value,bool isHeader)
{
    /*while(index>=m_children[indexSec].size())
        m_sectionList[indexSec].insert(m_sectionList[indexSec].size(),"");
    m_sectionList[indexSec].replace(index,value.toString());*/
}
void CharacterSheet::addData(QString path,QVariant value)
{
    //m_children[indexSec].insert(index,value.toString());
}
void CharacterSheet::appendLine()
{
    Section* sec = dynamic_cast<Section*>(m_rootChild);
    if(NULL!=sec)
    {
        sec->appendChild(new Field());
    }
}
int CharacterSheet::getSectionCount()
{
    return m_rootChild->getChildrenCount();
}

void CharacterSheet::appendItem(CharacterSheetItem* item)
{
    Section* sec = dynamic_cast<Section*>(m_rootChild);
    if(NULL!=sec)
    {
        sec->appendChild(item);
    }
}
bool CharacterSheet::removeItem(CharacterSheetItem* item)
{
    Section* sec = dynamic_cast<Section*>(m_rootChild);
    if(NULL!=sec)
    {
        //sec->remove(item);
    }
}

void CharacterSheet::removeSectionAt(int index)
{
    Section* sec = dynamic_cast<Section*>(m_rootChild);
    if(NULL!=sec)
    {
        //sec->remove(sec);
    }
}

