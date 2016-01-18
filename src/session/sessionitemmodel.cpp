/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include "sessionitemmodel.h"
#include "data/cleveruri.h"

#include <QDebug>
#include <QIcon>

//////////////////////////////////////
/// SessionItemModel
/////////////////////////////////////

SessionItemModel::SessionItemModel()
{
    m_rootItem = new Chapter();
}
QModelIndex SessionItemModel::index( int row, int column, const QModelIndex & parent ) const
{
    if(row<0)
        return QModelIndex();
    
    ResourcesNode* parentItem = NULL;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<ResourcesNode*>(parent.internalPointer());
    
    ResourcesNode* childItem = parentItem->getChildAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
bool  SessionItemModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if(!index.isValid())
        return false;

    if(Qt::EditRole==role)
    {
        ResourcesNode* childItem = static_cast<ResourcesNode*>(index.internalPointer());
        childItem->setName( value.toString());
        return true;
    }
    return false;
}
Qt::ItemFlags SessionItemModel::flags ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    ResourcesNode* childItem = static_cast<ResourcesNode*>(index.internalPointer());
    if(childItem->mayHaveChildren())
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;
    
}
QModelIndex SessionItemModel::parent( const QModelIndex & index ) const
{
    
    if (!index.isValid())
        return QModelIndex();
    
    ResourcesNode *childItem = static_cast<ResourcesNode*>(index.internalPointer());
    ResourcesNode *parentItem = childItem->getParent();
    
    if (parentItem == m_rootItem)
        return QModelIndex();
    
    return createIndex(parentItem->rowInParent(), 0, parentItem);
}
int SessionItemModel::rowCount(const QModelIndex& index) const
{
    if(index.isValid())
    {
        ResourcesNode* tmp = static_cast<ResourcesNode*>(index.internalPointer());
        return tmp->getChildrenCount();
    }
    else
    {
        return m_rootItem->getChildrenCount();
    }
}
int SessionItemModel::columnCount(const QModelIndex&) const
{
    return 1;
}
QVariant SessionItemModel::data(const QModelIndex &index, int role ) const
{
    if(!index.isValid())
        return QVariant();

    if(index.column()==0)// filename
    {
        ResourcesNode* tmp = static_cast<ResourcesNode*>(index.internalPointer());
        if(tmp)
        {
            if((role == Qt::DisplayRole)||(Qt::EditRole==role))
            {
                return tmp->name();
            }
            else if(role == Qt::DecorationRole)
            {
                return QIcon(tmp->getIcon());
            }
        }
    }
    return QVariant();
}

void SessionItemModel::addResource(ResourcesNode* node,QModelIndex& parent)
{
    if(m_rootItem->contains(node))
        return;

    Chapter* parentItem=NULL;
    if(!parent.isValid())
    {
        parentItem=m_rootItem;
    }
    else
    {
        ResourcesNode* node = static_cast<ResourcesNode*>(parent.internalPointer());
        if(!node->mayHaveChildren())
        {
            node=node->getParent();//leaf's parent is not a leaf indeed
        }
        parentItem=dynamic_cast<Chapter*>(node);// NULL when it is not a chapter.
    }

    beginInsertRows(QModelIndex(),parentItem->getChildrenCount(),parentItem->getChildrenCount());
    parentItem->addResource(node);
    endInsertRows();
}

void SessionItemModel::remove(QModelIndex& index)
{
    if(!index.isValid())
        return;
    ResourcesNode* indexItem = static_cast<ResourcesNode*>(index.internalPointer());
    QModelIndex parent = index.parent();
    ResourcesNode* parentItem=NULL;
    
    if(!parent.isValid())
        parentItem=m_rootItem;
    else
        parentItem= static_cast<ResourcesNode*>(parent.internalPointer());

    
    beginRemoveRows(index.parent(),index.row(),index.row());
    parentItem->removeChild(indexItem);
    endRemoveRows();
}

QVariant SessionItemModel::headerData ( int section, Qt::Orientation orientation, int role  ) const
{
    if((role==Qt::DisplayRole)&&(orientation==Qt::Horizontal))
    {
        switch(section)
        {
        case 0:
            return tr("Name");
            break;
        case 1:
            return tr("Type");
            break;
        case 2:
            return tr("Other");
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}
void SessionItemModel::saveModel(QDataStream& out)
{
    m_rootItem->write(out);
}

void SessionItemModel::loadModel(QDataStream& in)
{
    m_rootItem->read(in);
}
