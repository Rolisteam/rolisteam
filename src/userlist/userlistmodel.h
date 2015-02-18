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

class PersonItem
{
public:
    PersonItem(Person* p,bool isLeaf);
    void setPerson(Person* p);
    Person* getPerson();

    bool isLeaf();
    void setLeaf(bool leaf);

    PersonItem* getParent();
    void setParent(PersonItem* p);

    void addChild(PersonItem* child);
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

class UserListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit UserListModel(QObject *parent = 0);


    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    virtual void addPlayer(Person* p);
    bool isPlayer(const QModelIndex& );
    virtual void addCharacter(Person* p,Person* parent);
    Qt::ItemFlags flags ( const QModelIndex & index )  const;
    virtual void setLocalPlayer(Person* p);
    QList<Person*>* getSelectedPerson();


private:
    void addCheckedItem(QList<Person*>* tmp,PersonItem* itm);


    PersonItem* m_root;
    Person* m_player;
};

#endif // USERLISTMODEL_H
