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



TreeItem::TreeItem(Section* p,bool leaf)
    : m_section(p),m_isLeaf(leaf)
{
    qDebug() << " Creation TreeItem";
    m_children =new QList<TreeItem*>;
}
TreeItem* TreeItem::getParent()
{
    return m_parent;
}

void TreeItem::setParent(TreeItem* p)
{
    m_parent = p;
}

void TreeItem::setSection(Section* p)
{
    m_section = p;
}
Section* TreeItem::getSection()
{
    return m_section;
}

bool TreeItem::isLeaf()
{
    return m_isLeaf;
}

void TreeItem::setLeaf(bool leaf)
{
    m_isLeaf=leaf;

}
int TreeItem::childrenCount()
{
    return m_children->size();
}

void TreeItem::addChild(TreeItem* child)
{
    m_children->append(child);
}

TreeItem* TreeItem::child(int row)
{
    if(row < m_children->size())
        return m_children->at(row);

    return NULL;
}
int TreeItem::row()
{
    return m_parent->indexOfChild(this);
}
int TreeItem::indexOfChild(TreeItem* itm)
{
    return m_children->indexOf(itm);
}

CharacterSheetModel::CharacterSheetModel()
{
    m_characterList = new QList<CharacterSheet*>;
    m_sectionList =new QList<Section*>;
    m_rootItem = new TreeItem(NULL,false);
}

int CharacterSheetModel::rowCount ( const QModelIndex & parent  ) const
{



    if (parent.isValid())
    {
        TreeItem* tmp = static_cast<TreeItem*>(parent.internalPointer());
        if(!tmp->isLeaf())
        {
            if(tmp->getSection() == NULL)
                qDebug() << "section is null";
            return tmp->getSection()->size();
        }
        else
        {
            return 0;
        }
    }
    else
    {

        return m_sectionList->size();
    }

}
int CharacterSheetModel::columnCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent)
    qDebug() << "columnCount" << m_characterList->size()+1;
    return m_characterList->size()+1;
}
QModelIndex CharacterSheetModel::index ( int row, int column, const QModelIndex & parent ) const
{

    TreeItem* parentItem = NULL;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());



    TreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

  /*  if (!parent.isValid())
    {
        Section *childItem = m_sectionList->at(row);
        if (childItem)
            return createIndex(row, column, new TreeItem(childItem,false) );//add section


    }
    else
    {
        TreeItem* parentItem = static_cast<TreeItem*>(parent.internalPointer());
        if (parentItem)
            return createIndex(row,column,new TreeItem(parentItem->getSection(),true));//add leaf

    }
    return QModelIndex();*/
}
QModelIndex CharacterSheetModel::parent ( const QModelIndex & index ) const
{

    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->getParent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);


}

QVariant CharacterSheetModel::data ( const QModelIndex & index, int role  ) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
        qDebug() << "pointeur=" << childItem << " feuille" <<childItem->isLeaf() << index;

        if(childItem)
        {
            if(!childItem->isLeaf())
            {
                return childItem->getSection()->getName();
            }
            else
            {



                 int row = index.row();

                 if(row < childItem->getSection()->size())
                    return  childItem->getSection()->at(row);

                 return QString("line");
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
            tmp->setData(index.row(),value);
        }
        else
        {

             TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
             if(!childItem->isLeaf())
                 childItem->getSection()->setName(value.toString());
             else
             {
                childItem->getSection()->replace(index.row(),value.toString());
             }
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
    beginInsertRows(QModelIndex(),m_rootItem->childrenCount(),m_rootItem->childrenCount());
    Section* sec=new Section();
    sec->setName(tr("Empty Section %1").arg(m_rootItem->childrenCount()+1));
    TreeItem* tmp = new TreeItem(sec,false);
    tmp->setParent(m_rootItem);
    m_rootItem->addChild(tmp);
    m_sectionList->append(sec);
    foreach(CharacterSheet* sheet,*m_characterList)
    {
        sheet->appendSection(sec);
    }
    endInsertRows();

}

void CharacterSheetModel::addLine(const QModelIndex & index /*Section* index*/)
{

    TreeItem* parentItem = static_cast<TreeItem*>(index.internalPointer());
    beginInsertRows(index,parentItem->childrenCount(),parentItem->childrenCount());
    TreeItem* tmp = new TreeItem(parentItem->getSection(),true);
    tmp->setParent(parentItem);
    parentItem->addChild( tmp);
    QString* tmpstr = new QString(tr("Field %1").arg(parentItem->childrenCount()));
    parentItem->getSection()->append(*tmpstr);
    endInsertRows();
        /*Section* sec = indexToSection(index);

        if(tmp == NULL)
            qDebug() << "section is null 8";
        else
             qDebug() <<"taille=" <<tmp->size() << index << tmp;

        beginInsertRows(indexToSectionIndex(index),tmp->size(),tmp->size());
        QString* tmpstr = new QString(tr("Field %1").arg(tmp->size()));
        TreeItem* tmp = new TreeItem(sec,false);
        sec->insert(*tmpstr,*tmpstr);

        endInsertRows();*/
}
bool CharacterSheetModel::hasChildren ( const QModelIndex & parent  ) const
{

    if(!parent.isValid())//root
        return  m_sectionList->size()>0?true:false;
    else
    {
        TreeItem* childItem = static_cast<TreeItem*>(parent.internalPointer());
        if(childItem->getSection() == NULL)
            qDebug() << "section is null 5";
        if(childItem->isLeaf())
            return  false;
        else
        {
            return childItem->getSection()->size()?true:false;
        }
    }
}
Section* CharacterSheetModel::indexToSection(const QModelIndex & index)
{
    TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
    if(childItem)
    {
        if(childItem->getSection()==NULL)
            qDebug() << " getsection return NULL dans indexToSection";
        return childItem->getSection();
    }

    qDebug() << "return NULL dans indexToSection";
    return  NULL;
}
QModelIndex CharacterSheetModel::indexToSectionIndex(const QModelIndex & index)
{
    if(index.parent().isValid())//if parent is valid it's a field, return its parent (the section).
        return index.parent();
    else
        return index;
}
