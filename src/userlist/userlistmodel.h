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
#ifndef USERLISTMODEL_H
#define USERLISTMODEL_H

#include <QAbstractItemModel>
#include "person.h"
/**
  * @brief node class for UserList three model, stores a pointer to person instance and Children item, it makes a composite pattern
  * @attention Please pay attention to the the different terms: Person and PersonItem. @see Person documentation
  */
class PersonItem
{
public:
    /**
      * @brief constructor with argument
      * @param person is the data field
      * @param no deeper children
      */
    PersonItem(Person* p,bool isLeaf);
    /**
      * @brief  defines the data field
      */
    void setPerson(Person* p);
    /**
      * @brief accessor to get the person pointer
      */
    Person* getPerson();

    /**
      * @brief true : is the last depth, false otherwise
      */
    bool isLeaf();
    /**
      * @brief defines
      */
    void setLeaf(bool leaf);

    /**
      * @brief accessor to the parent item
      */
    PersonItem* getParent();
    /**
      * @brief defines the parent pointer
      */
    void setParent(PersonItem* p);
    /**
      * @brief adds child to his current
      */
    void addChild(PersonItem* child);
    /**
      * @brief removes child at the given row
      */
    void removeChild(int row);
    /**
      * @brief accessor to get corresponding child
      * @return child or null if there is no child at this row
      */
    PersonItem* child(int row);

    int row();
    int indexOfChild(PersonItem* itm);
    int indexOfPerson(Person* itm);

    int childrenCount();
private:
    Person* m_data;
    bool m_isLeaf;
    PersonItem* m_parent;
    QList<PersonItem*>* m_children;
};
/**
  * @brief homemade tree model to display users/character
  */
class UserListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    /**
      * @brief default constructor
      */
    explicit UserListModel(QObject *parent = 0);

    /**
      * @brief builds QModeIndex at the given position, must be implemented when subclassing QAbstractItemModel
      */
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    /**
      * @brief find out and return the parent of the given index, must be implemented when subclassing QAbstractItemModel
      */
    virtual QModelIndex parent ( const QModelIndex & index ) const;

    /**
      * @brief accessor to the row count, it is the number of child of the parent
      */
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    /**
      * @brief count of column always 1
      */
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    /**
      * @brief gets the appropriate data corresponding to the index and the role, must be implemented when subclassing QAbstractItemModel
      */
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    /**
      * @brief , must be implemented to make an editing model
      */
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    /**
      * @brief add player as child of the root item
      */
    virtual void addPlayer(Person* p);
    /**
      * @brief either the index represents a player or a character
      */
    bool isPlayer(const QModelIndex& );
    /**
      * @brief adds character as child of the given parent
      */
    virtual void addCharacter(Person* p,Person* parent);
    /**
      * @brief return selection flags for the given index, making them editable, selectable ...
      */
    Qt::ItemFlags flags ( const QModelIndex & index )  const;
    /**
      * @brief defines the given Person as the local player
      */
    virtual void setLocalPlayer(Person* p);
    /**
      * @brief accessor to the selection
      */
    QList<Person*>* getSelectedPerson();
    /**
      * @brief remove the Person of
      */
    void removeCharacter(QModelIndex& l,Person* p);

    virtual bool isLocalPlayer(QModelIndex& p);
private:
    /**
      * @brief put all checked item into the list
      */
    void addCheckedItem(QList<Person*>* tmp,PersonItem* itm);

    /**
      * @brief root item
      */
    PersonItem* m_root;
    /**
      * @brief current player
      */
    Person* m_player;
};

#endif // USERLISTMODEL_H
