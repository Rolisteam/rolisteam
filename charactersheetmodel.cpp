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

#include "charactersheetmodel.h"
#include "charactersheet.h"
#include "section.h"
#include "field.h"

#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>

/////////////////////////////
/// CharacterSheetModel
/////////////////////////////

CharacterSheetModel::CharacterSheetModel()
 : m_characterCount(0)
{
    m_characterList = new QList<CharacterSheet*>;
    m_rootSection = new Section();
}

int CharacterSheetModel::rowCount ( const QModelIndex & parent  ) const
{
    if (parent.isValid())
    {
        CharacterSheetItem* tmp = static_cast<CharacterSheetItem*>(parent.internalPointer());
        return tmp->getChildrenCount();
    }
    else
    {
        return m_rootSection->getChildrenCount();
    }
}
CharacterSheet*  CharacterSheetModel::getCharacterSheet(int id)
{
    if((!m_characterList->isEmpty())&&(m_characterList->size()>id)&&(0<=id))
    {
        return m_characterList->at(id);
    }
    return NULL;
}

int CharacterSheetModel::columnCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent)
    return m_characterList->size()+1;
}
QModelIndex CharacterSheetModel::index ( int row, int column, const QModelIndex & parent ) const
{
    
    if(row<0)
        return QModelIndex();
    
    CharacterSheetItem* parentItem = NULL;
    
    if (!parent.isValid())
        parentItem = m_rootSection;
    else
        parentItem = static_cast<CharacterSheetItem*>(parent.internalPointer());
    
    CharacterSheetItem* childItem = parentItem->getChildAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
    
    
}
QModelIndex CharacterSheetModel::parent ( const QModelIndex & index ) const
{
    
    if (!index.isValid())
        return QModelIndex();
    
    CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
    CharacterSheetItem* parentItem = childItem->getParent();
    
    if (parentItem == m_rootSection)
        return QModelIndex();
    
    return createIndex(parentItem->rowInParent(), 0, parentItem);
}

QVariant CharacterSheetModel::data ( const QModelIndex & index, int role  ) const
{
    if(!index.isValid())
        return QVariant();
    
    if((role == Qt::TextAlignmentRole) && (index.column()!=0))
        return Qt::AlignHCenter;
    if((role == Qt::DisplayRole)||(role == Qt::EditRole))
    {
      /*  if(index.column()==0)
        {*/
                CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
                if(NULL!=childItem)
                {
                    if(index.column()==0)
                    {
                        return childItem->getValue();
                    }
                    else
                    {
                        QString path = childItem->getId();
                        CharacterSheet* sheet = m_characterList->at(index.column()-1);
                        return sheet->getValue(path);
                        //childItem->setValue(value.toString(),index.column()-1);
                    }
                }
       /* }
        else
        {
            QModelIndex tmp = index.sibling(index.row(),0);
            CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(tmp.internalPointer());
            if(NULL!=childItem)
            {
                return m_characterList->at(index.column()-1)->getValue(childItem->getPath());
            }
        }*/

    }
    return QVariant();
}
bool CharacterSheetModel::setData ( const QModelIndex& index, const QVariant & value, int role  )
{
    if(Qt::EditRole==role)
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
        if(NULL!=childItem)
        {
            if(index.column()==0)
            {
                childItem->setValue(value.toString());
            }
            else
            {
                QString path = childItem->getId();
                CharacterSheet* sheet = m_characterList->at(index.column()-1);
                sheet->setValue(path,value.toString());
            }
            return true;
        }
    }
    return false;
    
}
CharacterSheet* CharacterSheetModel::addCharacterSheet()
{
    beginInsertColumns(QModelIndex(),m_characterList->size()+1 ,m_characterList->size()+1 );
    ++m_characterCount;
    CharacterSheet* sheet = new CharacterSheet;
    m_characterList->append(sheet);
    //sheet->setRootChild(m_rootSection);
    endInsertColumns();
    return sheet;
}

void CharacterSheetModel::setRootSection(Section *rootSection)
{
    m_rootSection = rootSection;
}


QVariant CharacterSheetModel::headerData(int section, Qt::Orientation orientation,
                                         int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Fields name");
        default:
            return QString();
        }
    }
    return QVariant();
}
Qt::ItemFlags CharacterSheetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}
void CharacterSheetModel::addSection()
{
    addSection(tr("Empty Section %1").arg(m_rootSection->getChildrenCount()+1));
}
CharacterSheetItem* CharacterSheetModel::addSection(QString title)
{
    beginInsertRows(QModelIndex(),m_rootSection->getChildrenCount(),m_rootSection->getChildrenCount());
    Section* rootSection = static_cast<Section*>(m_rootSection);
    Section* sec=new Section();
    sec->setName(title);
    rootSection->appendChild(sec);
    endInsertRows();
    return sec;
}

void CharacterSheetModel::addLine(const QModelIndex& index)
{
    QModelIndex parent = index;
    CharacterSheetItem* parentItem = NULL;
    if(index.isValid())
    {
        parentItem = static_cast<CharacterSheetItem*>(index.internalPointer());
    }
    else
    {
        parentItem = m_rootSection;
    }
    if(!parentItem->mayHaveChildren())
    {
        parentItem = parentItem->getParent();
        parent = parent.parent();
    }
    addLine(parentItem,tr("Field %1").arg(parentItem->getChildrenCount()),parent);

}
void CharacterSheetModel::addLine(CharacterSheetItem* parentItem,QString name,const QModelIndex& parent)
{
    if(parentItem->mayHaveChildren())
    {
        beginInsertRows(parent,parentItem->getChildrenCount(),parentItem->getChildrenCount());
        Section* section = static_cast<Section*>(parentItem);
        Field* field = new Field();
        field->setId(name);
        section->appendChild(field);
        endInsertRows();
    }
}
bool CharacterSheetModel::hasChildren ( const QModelIndex & parent  ) const
{
    
    if(!parent.isValid())//root
        return  m_rootSection->getChildrenCount()>0?true:false;
    else
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(parent.internalPointer());
        if(childItem->getChildrenCount()==0)
            return false;
        else
            return true;


    }
}
CharacterSheetItem* CharacterSheetModel::indexToSection(const QModelIndex & index)
{
    if(index.isValid())
        return static_cast<CharacterSheetItem*>(index.internalPointer());
    else
        return NULL;
}
QModelIndex CharacterSheetModel::indexToSectionIndex(const QModelIndex & index)
{
    if(index.parent().isValid())//if parent is valid it's a field, return its parent (the section).
        return index.parent();
    else
        return index;
}
bool CharacterSheetModel::writeModel(QJsonObject& jsonObj, bool data)
{
    m_rootSection->save(jsonObj);
    qDebug() << "characterCount:" <<m_characterCount;
    jsonObj["characterCount"]=m_characterCount;

    QJsonArray characters;
    foreach (CharacterSheet* item, *m_characterList)
    {
        qDebug() << "inside The foreach";
        QJsonObject charObj;
        item->save(charObj);
        characters.append(charObj);
    }
    jsonObj["characters"]=characters;
    return true;
}

bool CharacterSheetModel::readModel(QJsonObject& jsonObj)
{
    beginResetModel();
    m_rootSection->load(jsonObj,QList<QGraphicsScene*>());
    m_characterCount = jsonObj["characterCount"].toInt();
    QJsonArray characters = jsonObj["characters"].toArray();
    foreach(auto charJson, characters)
    {
        QJsonObject obj = charJson.toObject();
        CharacterSheet* sheet = new CharacterSheet();
        sheet->load(obj);
        m_characterList->append(sheet);
        emit characterSheetHasBeenAdded(sheet);
    }


    endResetModel();
}
