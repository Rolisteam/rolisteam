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
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

/////////////////////////////
/// CharacterSheetModel
/////////////////////////////

CharacterSheetModel::CharacterSheetModel()
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
        if(index.column()==0)
        {
            CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
            if(NULL!=childItem)
            {
                return childItem->name();
            }
        }
        else
        {
            QModelIndex tmp = index.sibling(index.row(),0);
            CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(tmp.internalPointer());
            if(NULL!=childItem)
            {
                return m_characterList->at(index.column()-1)->getData(childItem->getPath());
            }
        }

    }
    return QVariant();
}
bool CharacterSheetModel::setData ( const QModelIndex & index, const QVariant & value, int role  )
{
    if(Qt::EditRole==role)
    {
        if(index.column()!=0)
        {
            CharacterSheet* tmp = m_characterList->at(index.column()-1);
            if(index.parent().isValid())
            {
                CharacterSheetItem* parentItem = static_cast<CharacterSheetItem*>(index.parent().internalPointer());
                tmp->setData(parentItem->getPath(),value);
            }
            else
            {
                QString tmpstring=value.toString();
                tmp->setChildrenValue(index.row(),tmpstring);
            }
        }
        else
        {

            CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
            if(NULL!=childItem)
            {
                childItem->setName(value.toString());
            }

        }
        return true;
    }
    return false;
    
}
CharacterSheet* CharacterSheetModel::addCharacterSheet()
{
    
    beginInsertColumns(QModelIndex(),m_characterList->size() ,m_characterList->size() );
    CharacterSheet* sheet = new CharacterSheet;
    m_characterList->append(sheet);
    sheet->setRootChild(m_rootSection);
    endInsertColumns();
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
            return m_characterList->at(section-1)->owner();
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
    foreach(CharacterSheet* sheet,*m_characterList)
    {
        sheet->appendItem(sec);
    }
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
        field->setName(name);
        section->appendChild(field);
        foreach(CharacterSheet* sheet,*m_characterList)
        {
            Field* field = new Field();
            field->setName(name);
            sheet->appendItem(field);
        }
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
bool CharacterSheetModel::writeModel(QTextStream& file, bool data)
{
    QJsonDocument doc;
    QJsonObject jsonObj;
    m_rootSection->save(jsonObj);
    doc.setObject(jsonObj);

    QJsonArray listOfCharacter;
    foreach(CharacterSheet* sheet, *m_characterList)
    {
        QJsonObject itemObject;
        sheet->save(itemObject);
        listOfCharacter.append(itemObject);
    }
    jsonObj["characters"] = listOfCharacter;
    file<< doc.toJson();
    return true;
}

bool CharacterSheetModel::readModel(QFile& file)
{
    beginResetModel();

    QJsonDocument json = QJsonDocument::fromJson(file.readAll());
    QJsonObject jsonObj = json.object();
    m_rootSection->load(jsonObj);

    QJsonArray fieldArray = jsonObj["characters"].toArray();
    QJsonArray::Iterator it;
    for(it = fieldArray.begin(); it != fieldArray.end(); ++it)
    {
        QJsonObject obj = (*it).toObject();
        CharacterSheet* item;
        item = new CharacterSheet();
        item->load(obj);
        m_characterList->append(item);
    }

    endResetModel();
}
