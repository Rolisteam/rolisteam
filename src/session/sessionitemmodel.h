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
#ifndef SESSIONITEMMODEL_H
#define SESSIONITEMMODEL_H

#include <QAbstractItemModel>
#include "data/cleveruri.h"
class RessourcesNode;
/**
    * @brief stored a ressource node and the position into the tree model
    */
class ResourcesItem
{
public:
    /**
    * @brief constructor with parameter
    * @param Section pointer
    * @param isLeaf either the item is a leaf or not.
    */
    ResourcesItem(RessourcesNode* p,bool isLeaf);
    
    /**
    * @brief default destructor
    */
    ~ResourcesItem();
    
    /**
    * @brief defines the stored section
    */
    void setData(RessourcesNode* p);
    /**
    * @brief accessor to the stored section
    */
    RessourcesNode* getData();
    
    
    
    /**
    * @brief return either the item is a leaf or note.
    */
    bool isLeaf();
    /**
    * @brief defines if the item is a leaf or not
    */
    void setLeaf(bool leaf);
    /**
    * @brief get the item's parent
    * @return pointer to its parent
    */
    ResourcesItem* getParent();
    /**
    * @brief defines the item's parent
    * @param new parent
    */
    void setParent(ResourcesItem* p);
    /**
    * @brief adds child in this item.
    * @param new child
    */
    void addChild(ResourcesItem* child);
    /**
    * @brief gives the child at the given position
    * @param the position (row) of the wanted item.
    * @return TreeItem's pointer or NULL if row is out of range.
    */
    ResourcesItem* child(int row);
    /**
    * @brief gives the position of the current item into is father.
    */
    int row();
    /**
    * @brief gives the position of the given child in the children list.
    * @param the child
    * @return position of child, -1 if not matched.
    */
    int indexOfChild(ResourcesItem* itm);
    /**
    * @brief accessor to the children count.
    */
    int childrenCount();
    
    void clean();
    
    QList<ResourcesItem*>* getChildren();
private:
    RessourcesNode* m_data;
    bool m_isLeaf; /// @brief true when no child and has valid parent
    bool m_directory; // organisation helper.
    ResourcesItem* m_parent;
    QList<ResourcesItem*>* m_children;
    
};

class Session;
class Chapter;
/**
    * @brief subclassed model to fit the management of session (ressources)
    */
class SessionItemModel : public QAbstractItemModel
{
public:
    /**
    * @brief default construtor
    */
    SessionItemModel();
    /**
    * @brief pure virtual method to create index according the given position and parent
    * @param row : row of the index (define position of the index into the parent's children list)
    * @param column : column of the index (define position of the index into the parent's children list)
    * @param parent : define the parent, if null parent is root item
    * @return the created index
    */
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    /**
    * @brief get the parent of the given index
    * @param child item
    * @return parent
    */
    virtual QModelIndex parent( const QModelIndex & index ) const;
    /**
    * @brief accessor to the rowcount
    * @param index
    * @return the number of children
    */
    virtual int rowCount(const QModelIndex&) const;
    /**
    * @brief accessor to the column count
    * @param ignored
    * @return always return 1
    */
    virtual int columnCount( const QModelIndex&) const;
    /**
    * @brief main method to provide data to the view.
    * @param index to display
    * @param role of data
    * @return appropriate data
    */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /**
    * @brief provides data to header section
    */
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    /**
    * @brief remove item
    */
    void remove(QModelIndex& index);
    /**
    * @brief add chapter with the given name into the given parent
    * @param name of the chapter
    * @param index of parent
    * @return pointer to the added chapter
    */
    Chapter* addChapter(QString& name,QModelIndex parent);
    /**
    * @brief set the Session
    */
    void setSession(Session* s);
    /**
    * @brief add chapter
    * @param chapter address
    */
    void addChapter(Chapter* c );
    /**
    * @brief add ressources into the session and display it into the session manager widget
    * @param uri of the resource
    * @param type of resource
    * @param parent index
    * @return created cleverURI address
    */
    CleverURI* addRessources(CleverURI* uri,QModelIndex& index);
    /**
    * @brief fills up the model with the given chapter and its children
    */
    void populateChapter(Chapter* t,ResourcesItem* parentIte);
    /**
    * @brief accessor to the chapter at the given position
    * @param index in the model
    * @return address of the chapter or NULL otherwise.
    */
    Chapter* getChapter(QModelIndex& index);
    /**
    * @brief Allows editing directly in the view
    * @param index position
    * @param new value
    * @param role
    */
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    /**
    * @brief otheridden method to define flag of items, chapter are selectable and enabled
    * resource are also checkable
    * @param position of the item
    */
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
private:
    //Section* m_rootSection;
    
    ResourcesItem* m_rootItem;/// root item address
    Session* m_session;/// address to the session
};

#endif // SESSIONITEMMODEL_H
