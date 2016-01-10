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
#include "data/session.h"
#include <QDebug>
#include <QIcon>
ResourcesItem::ResourcesItem(RessourcesNode* p,bool leaf)
    : m_data(p),m_isLeaf(leaf)
{
    m_children =new QList<ResourcesItem*>;
    
}
ResourcesItem* ResourcesItem::getParent()
{
    return m_parent;
}
ResourcesItem::~ResourcesItem()
{
    delete m_data;
    // delete m_parent;
    delete m_children;
}

void ResourcesItem::setParent(ResourcesItem* p)
{
    m_parent = p;
}

void ResourcesItem::setData(RessourcesNode* p)
{
    m_data = p;
}
void ResourcesItem::clean()
{
    m_children->clear();
}

RessourcesNode* ResourcesItem::getData()
{
    return m_data;
}

bool ResourcesItem::isLeaf()
{
    return m_isLeaf;
}

void ResourcesItem::setLeaf(bool leaf)
{
    m_isLeaf=leaf;
    
}
int ResourcesItem::childrenCount()
{
    return m_children->size();
}

void ResourcesItem::addChild(ResourcesItem* child)
{
    child->setParent(this);
    m_children->append(child);
}


ResourcesItem* ResourcesItem::child(int row)
{
    // qDebug() << "child row="<< row << m_children->size() << m_children->at(row);
    if(row < m_children->size())
        return m_children->at(row);
    
    return NULL;
}
int ResourcesItem::row()
{
    return m_parent->indexOfChild(this);
}
int ResourcesItem::indexOfChild(ResourcesItem* itm)
{
    return m_children->indexOf(itm);
}
QList<ResourcesItem*>* ResourcesItem::getChildren()
{
    return m_children;
}

//////////////////////////////////////
/// SessionItemModel
/////////////////////////////////////

SessionItemModel::SessionItemModel()
    : m_session(NULL)
{
    m_rootItem = new ResourcesItem(NULL,false);
    m_session = new Session();
}
QModelIndex SessionItemModel::index( int row, int column, const QModelIndex & parent ) const
{
    
    if(row<0)
        return QModelIndex();
    
    ResourcesItem* parentItem = NULL;
    
    // qDebug()<< "Index session " <<row << column << parent;
    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<ResourcesItem*>(parent.internalPointer());
    
    ResourcesItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
    
}
bool  SessionItemModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if(Qt::EditRole==role)
    {
        ResourcesItem* childItem = static_cast<ResourcesItem*>(index.internalPointer());
        QString st = value.toString();
        childItem->getData()->setShortName(st);
        return true;
    }
    return false;
}
Qt::ItemFlags SessionItemModel::flags ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    ResourcesItem* childItem = static_cast<ResourcesItem*>(index.internalPointer());
    if(!childItem->isLeaf())
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;
    
}
QModelIndex SessionItemModel::parent( const QModelIndex & index ) const
{
    
    if (!index.isValid())
        return QModelIndex();
    
    ResourcesItem *childItem = static_cast<ResourcesItem*>(index.internalPointer());
    ResourcesItem *parentItem = childItem->getParent();
    
    if (parentItem == m_rootItem)
        return QModelIndex();
    
    return createIndex(parentItem->row(), 0, parentItem);
    
    
    
}
int SessionItemModel::rowCount(const QModelIndex& index) const
{
    // qDebug() << index;
    if(index.isValid())
    {
        ResourcesItem* tmp = static_cast<ResourcesItem*>(index.internalPointer());
        //    qDebug() << "rowCount tmp"<< tmp->childrenCount();
        return tmp->childrenCount();
    }
    else
    {
        //  qDebug() << "rowCount root"<<  m_rootItem->childrenCount() << m_session->chapterList().size() << m_session->childrenCount();
        //return m_rootItem->getChildren()->size();
        return (m_session->childrenCount());//->childrenCount();
    }
}
int SessionItemModel::columnCount(const QModelIndex&) const
{
    return 1;
}
QVariant SessionItemModel::data(const QModelIndex &index, int role ) const
{
    if((role == Qt::DisplayRole)||(Qt::EditRole==role))
    {
        if(!index.isValid())
            return QVariant();
        if(index.column()==0)// filename
        {
            ResourcesItem* tmp = static_cast<ResourcesItem*>(index.internalPointer());
            if(tmp)
            {
                RessourcesNode* t =  tmp->getData();
                //qDebug() << tmp;
                return t->getShortName();
            }
        }
    }
    if(role == Qt::DecorationRole)
    {
        if(index.column()==0)// type of file
        {
            ResourcesItem* tmp = static_cast<ResourcesItem*>(index.internalPointer());
            if(tmp)
            {
                CleverURI* t =  dynamic_cast<CleverURI*>(tmp->getData());
                if(t==NULL)
                    return QVariant();

                //   return CleverURI::getIcon((CleverURI::ContentType)t->getType());
                return QIcon(t->getIcon());

                // return t->getShortName();
            }
        }
    }
    return QVariant();
}
void SessionItemModel::setSession(Session* s)
{
    /// @todo parse the session and build appropriate ressources items.
    m_session = s;
    m_rootItem->clean();
    for(int i =0;i<m_session->chapterList().size();i++)
    {
        Chapter* tmp = m_session->chapterList()[i];
        ResourcesItem* rt = new ResourcesItem(tmp,false);
        m_rootItem->addChild(rt);
        populateChapter(tmp,rt);
    }
    for(int i = 0;i<m_session->getUnclassedList().size();i++)
    {
        CleverURI* tmp2 = m_session->getUnclassedList()[i];
        ResourcesItem* rt = new ResourcesItem(tmp2,true);
        m_rootItem->addChild(rt);
    }
}

Chapter* SessionItemModel::addChapter(QString& name,QModelIndex parent)
{
    ResourcesItem* tmp =NULL;
    if(!parent.isValid())
        tmp=m_rootItem;
    else
        tmp = static_cast<ResourcesItem*>(parent.internalPointer());
    
    
    Chapter* t=NULL;
    if(!parent.isValid())
    {
        beginInsertRows(parent,tmp->childrenCount(),tmp->childrenCount());
        t = m_session->addChapter(name);
        tmp->addChild(new ResourcesItem(t,false));
        endInsertRows();


    }
    else
    {
        Chapter* parentChapter = dynamic_cast<Chapter*>(tmp->getData());
        if(parentChapter!=NULL)
        {
            beginInsertRows(parent,tmp->childrenCount(),tmp->childrenCount());
            t = parentChapter->addChapter(name);
            tmp->addChild(new ResourcesItem(t,false));
            endInsertRows();

        }

    }
    
    
    return t;
}

CleverURI* SessionItemModel::addRessources(CleverURI* uri,QModelIndex& parent)
{
    ResourcesItem* parentItem=NULL;
    if(!parent.isValid())
        parentItem=m_rootItem;
    else
    {
        parentItem = static_cast<ResourcesItem*>(parent.internalPointer());
    }
    beginInsertRows(QModelIndex(),parentItem->childrenCount(),parentItem->childrenCount());
    
    if(parentItem->isLeaf())
    {
        parentItem=parentItem->getParent();//leaf's parent is not a leaf indeed
    }
    
    
    Chapter* chap=dynamic_cast<Chapter*>(parentItem->getData());// NULL when it is not a chapter.
    
    m_session->addRessource(uri,chap);
    parentItem->addChild(new ResourcesItem(uri,true));
    endInsertRows();
    return uri;
}
void SessionItemModel::populateChapter(Chapter* t,ResourcesItem* parentItem)
{
    for(int i =0;i<t->getChapterList().size();i++)
    {
        Chapter* tmp = t->getChapterList()[i];
        ResourcesItem* rt = new ResourcesItem(tmp,false);
        parentItem->addChild(rt);
        populateChapter(tmp,rt);
    }
    for(int i = 0;i<t->getResourceList().size();i++)
    {
        CleverURI* tmp2 = t->getResourceList()[i];
        ResourcesItem* rt = new ResourcesItem(tmp2,true);
        parentItem->addChild(rt);
    }
}
void SessionItemModel::remove(QModelIndex& index)
{
    if(!index.isValid())
        return;
    ResourcesItem* indexItem = static_cast<ResourcesItem*>(index.internalPointer());
    QModelIndex parent = index.parent();
    ResourcesItem* parentItem=NULL;
    
    if(!parent.isValid())
        parentItem=m_rootItem;
    else
        parentItem= static_cast<ResourcesItem*>(parent.internalPointer());
    
    if(indexItem->childrenCount()>0)
    {
        beginRemoveRows(index,0,indexItem->childrenCount());
        QList<ResourcesItem*>* child = indexItem->getChildren();



        endRemoveRows();
    }
    
    beginRemoveRows(index.parent(),index.row(),index.row());
    parentItem->getChildren()->removeOne(indexItem);
    //delete indexItem;
    
    m_session->removeRessourcesNode(indexItem->getData());
    
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
Chapter* SessionItemModel::getChapter(QModelIndex& index)
{
    ResourcesItem* childItem = static_cast<ResourcesItem*>(index.internalPointer());
    if(!childItem->isLeaf())
        return NULL;
    else
        return static_cast<Chapter*>(childItem->getData());
}
