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



TreeItem::TreeItem(Section* p)
    : m_section(p)
{
}

void TreeItem::setSection(Section* p)
{
    m_section = p;
}
Section* TreeItem::getSection()
{
    return m_section;
}



CharacterSheetModel::CharacterSheetModel()
{
    m_characterList = new QList<CharacterSheet*>;
    m_sectionList =new QList<Section*>;
}

int CharacterSheetModel::rowCount ( const QModelIndex & parent  ) const
{


qDebug() << " merde rowcount ";
    if (parent.isValid())
    {
        Section* tmp = static_cast<Section*>(parent.internalPointer());
        if(tmp)
        {
            qDebug() << "section " << m_sectionList->size();
            return tmp->size();
        }
        else
        {
            qDebug() << "section zero 0 ";
            return 0;
        }
    }
    else
    {
        qDebug() << "sectionlist " << m_sectionList->size();
        return m_sectionList->size();
    }
    /*if(parent.isValid())
    {
        int row = parent.row();

        if(row < m_sectionList->size())
        {
            qDebug() << "rowcount" << m_sectionList->at(row)->count();
            return m_sectionList->at(row)->count();
        }
        else
        {
            qDebug() << "rowcount zero";
            return 0;
        }
    }
    else
    {
        qDebug() << "rowcount section" << m_sectionList->size();
        return m_sectionList->size();
    }*/


    /*int max = 0;
    for(int i = 0;i<m_characterList->size();i++)
    {
        max += m_characterList->at(i)->getIndexCount();

    }

    return max;*/
}
int CharacterSheetModel::columnCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent)
    qDebug() << "columnCount" << m_characterList->size()+1;
    return m_characterList->size()+1;
}
QModelIndex CharacterSheetModel::index ( int row, int column, const QModelIndex & parent ) const
{



    if (!parent.isValid())
    {
        Section *childItem = m_sectionList->at(row);
        if (childItem)
            return createIndex(row, column, childItem);
        else
            return QModelIndex();
    }
    else
    {
        Section* parentItem = static_cast<Section*>(parent.internalPointer());

        return createIndex(row,column,new TreeItem(parentItem));
    }
}
QModelIndex CharacterSheetModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return QModelIndex();

    Section* childItem = static_cast<Section*>(index.internalPointer());
    if(childItem)
    {
       /* Section* parentItem = childItem->parent();

        if(parentItem == rootItem)*/
            return QModelIndex();

        //return createIndex(parentItem->row(), 0, parentItem);
    }
    else
    {
        TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());

        return createIndex(m_sectionList->indexOf(childItem->getSection()), 0, childItem->getSection());
    }
}

QVariant CharacterSheetModel::data ( const QModelIndex & index, int role  ) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        Section* childItem = static_cast<Section*>(index.internalPointer());
        if(childItem)
            return childItem->getName();
        else
        {
             TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
             childItem->getSection();
                return QVariant();
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
            tmp->setData(index.row(),value);
        }
        else
        {
             Section* childItem = static_cast<Section*>(index.internalPointer());
             if(childItem)
                 childItem->setName(value.toString());
            //CharacterSheet* tmp = m_characterList->at(index.column()-1);
            //tmp->setData(index.row(),0,true);
        }
        return true;
    }
    return false;

}
void CharacterSheetModel::addCharacterSheet()
{

    beginInsertColumns(QModelIndex(),m_characterList->size() ,m_characterList->size() );
    //if(m_characterList->size()==0)

    CharacterSheet* sheet = new CharacterSheet;
    //sheet->appendSection();
    m_characterList->append(sheet);

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
                return tr("field name");
            default:
                m_characterList->at(section-1);
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
    qDebug() << "in add section";
    beginInsertRows(QModelIndex(),m_sectionList->size(),m_sectionList->size());
    Section* sec=new Section();
    sec->setName(tr("Empty Section %1").arg(m_sectionList->size()+1));
    m_sectionList->append(sec);
    foreach(CharacterSheet* sheet,*m_characterList)
    {
        sheet->appendSection(sec);
    }
    endInsertRows();

}

void CharacterSheetModel::addLine(int index)
{

}
bool CharacterSheetModel::hasChildren ( const QModelIndex & parent  ) const
{

    if(!parent.isValid())//root
        return  m_sectionList->size()>0?true:false;
    else
    {
        Section* childItem = static_cast<Section*>(parent.internalPointer());
        if(childItem)
            return  childItem->size()>0?true:false;
        else
        {
          return false;
        }
    }
}
