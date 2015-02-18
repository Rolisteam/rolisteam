/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
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
#include "userlistmodel.h"

#include <QDebug>
#include <QStandardItem>

PersonItem::PersonItem(Person* p,bool isLeaf)
    : m_data(p),m_isLeaf(isLeaf)
{
    m_children = new  QList<PersonItem*>;
}

void PersonItem::setPerson(Person* p)
{
    m_data= p;
}

Person* PersonItem::getPerson()
{
    return m_data;
}

bool PersonItem::isLeaf()
{
    return m_isLeaf;
}

void PersonItem::setLeaf(bool leaf)
{
    m_isLeaf = leaf;
}

PersonItem* PersonItem::getParent()
{
    return m_parent;
}
void PersonItem::removeChild(int row)
{
    if((m_children->size()>row)&&(row>=0))
        m_children->removeAt(row);
}

void PersonItem::setParent(PersonItem* p)
{
    m_parent = p;
}
int PersonItem::indexOfPerson(Person* itm)
{
    int i = 0;
    foreach(PersonItem* p, *m_children)
    {

        if(itm == p->getPerson())
            return i;
        i++;
    }
    return -1;
}

void PersonItem::addChild(PersonItem* child)
{
    m_children->append(child);
    child->setParent(this);
}

PersonItem* PersonItem::child(int row)
{
    if((row <m_children->size())&&(row>=0))
        return m_children->at(row);
    else
        return NULL;
}

int PersonItem::row()
{
    return m_parent->indexOfChild(this);
}

int PersonItem::indexOfChild(PersonItem* itm)
{
    return m_children->indexOf(itm);
}
int PersonItem::childrenCount()
{
    return m_children->size();
}

UserListModel::UserListModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_root = new PersonItem(NULL,false);
}
QModelIndex UserListModel::index ( int row, int column, const QModelIndex & parent  ) const
{
    if(row<0)
        return QModelIndex();

    PersonItem* parentItem = NULL;

    if (!parent.isValid())
        parentItem = m_root;
    else
    {
        parentItem = static_cast<PersonItem*>(parent.internalPointer());
    }

    PersonItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
bool UserListModel::isPlayer(const QModelIndex & index)
{
    if(!index.isValid())
        return false;
    if(!index.parent().isValid())
        return true;

    return false;

}

QModelIndex UserListModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return QModelIndex();

    PersonItem *childItem = static_cast<PersonItem*>(index.internalPointer());
    PersonItem *parentItem = childItem->getParent();

    if (parentItem == m_root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
int UserListModel::rowCount ( const QModelIndex & parent  ) const
{
        PersonItem* tmp = static_cast<PersonItem*>(parent.internalPointer());
        if(tmp!=NULL)
            return tmp->childrenCount();
        else
            return m_root->childrenCount();
}
int UserListModel::columnCount ( const QModelIndex & parent  ) const
{
    return 1;
}
QVariant UserListModel::data ( const QModelIndex & index, int role  ) const
{
    if(!index.isValid())
        return QVariant();

   /* if((role == Qt::DisplayRole)||(role == Qt::DecorationRole)||(Qt::CheckStateRole == role))
    {*/
        if(index.column()==0)
        {
            PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
            if(childItem)
            {

                switch(role)
                {
                    case Qt::EditRole:
                    case Qt::DisplayRole:
                        return childItem->getPerson()->getName();

                    case Qt::DecorationRole:
                        return childItem->getPerson()->getColor();

                    case Qt::CheckStateRole:
                        return childItem->getPerson()->checkedState();
                    default:
                        return QVariant();

                }
            }
        }

    return QVariant();
}
void UserListModel::addPlayer(Person* p)
{
    beginInsertRows(QModelIndex(),m_root->childrenCount(),m_root->childrenCount());
    m_root->addChild(new PersonItem(p,false));
    endInsertRows();
}
void UserListModel::removeCharacter(QModelIndex& a,Person* p)
{
        PersonItem* parentItem = static_cast<PersonItem*>(a.parent().internalPointer());
        if(p==parentItem->getPerson())
        {
            beginRemoveRows(a,a.row(),a.row());
            parentItem->removeChild(a.row());
            endRemoveRows();
        }
}

void UserListModel::addCharacter(Person* p,Person* parent)
{
    int row= m_root->indexOfPerson(parent);
    if(row>-1)
    {
        QModelIndex parentIndex = index(row,0);
        PersonItem* childItem = static_cast<PersonItem*>(parentIndex.internalPointer());

        beginInsertRows(parentIndex,childItem->childrenCount(),childItem->childrenCount());
        childItem->addChild(new PersonItem(p,true));
        endInsertRows();
    }
}
void UserListModel::setLocalPlayer(Person* p)
{
    m_player = p;
}

bool UserListModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if(Qt::EditRole==role)
    {
            PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
            QString st = value.toString();
            childItem->getPerson()->setName(st);
            return true;
    }
    else if(Qt::CheckStateRole == role)
    {
        PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
        childItem->getPerson()->setState((Qt::CheckState)value.toInt());
        return true;
    }
    return false;
}
Qt::ItemFlags UserListModel::flags ( const QModelIndex & index )  const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
    if((childItem->getPerson()==m_player)||(childItem->getParent()->getPerson() ==m_player))
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable ;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}
QList<Person*>* UserListModel::getSelectedPerson()
{
    QList<Person*>* tmp = new QList<Person*>();
    addCheckedItem(tmp,m_root);
    return tmp;
}
void UserListModel::addCheckedItem(QList<Person*>* tmp,PersonItem* itm)
{
    for(int i = 0;i<itm->childrenCount();i++)
    {
        Person* tmpPerson = itm->child(i)->getPerson();
        if(tmpPerson->checkedState()==Qt::Checked)
            tmp->append(tmpPerson);
        addCheckedItem(tmp,itm->child(i));
    }
}
bool UserListModel::isLocalPlayer(QModelIndex& index)
{
    if(!index.isValid())
    {
        qDebug()<< "Index invalide" ;
        return false;

     }

    //PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
    return (Qt::ItemIsEditable | flags(index));

    //return (p==m_player);
}
