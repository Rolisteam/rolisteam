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
#include "cleveruri.h"
class RessourcesNode;
class ResourcesItem
{
public:
    /**
      * @brief constructor with parameter
      * @param Section pointer
      * @param isLeaf either the item is a leaf or not.
      */
    ResourcesItem(RessourcesNode* p,bool isLeaf);
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
class SessionItemModel : public QAbstractItemModel
{
public:
    SessionItemModel();

   virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   virtual QModelIndex parent( const QModelIndex & index ) const;
   virtual int rowCount(const QModelIndex&) const;
   virtual int columnCount( const QModelIndex&) const;
   virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
   QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    void remove(QModelIndex& index);
    Chapter* addChapter(QString& name,QModelIndex parent);
    void setSession(Session* s);
    void addChapter(Chapter* c );
    CleverURI* addRessources(QString& urifile, CleverURI::ContentType& type,QModelIndex& index);

    void populateChapter(Chapter* t,ResourcesItem* parentIte);

    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
private:
    //Section* m_rootSection;

    ResourcesItem* m_rootItem;
    Session* m_session;
};

#endif // SESSIONITEMMODEL_H
