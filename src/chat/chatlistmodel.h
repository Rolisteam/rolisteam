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
#ifndef TCHATLISTMODEL_H
#define TCHATLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

class Person;

/**
  *  @brief model for listing all opened chat
  */
class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
      *  @brief default constructor
      */
    explicit ChatListModel(QObject *parent = 0);
    /**
      *  @brief overridden function to define number of row
      */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /**
      *  @brief overridden function to get data from the model
      */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
      *  @brief accessor to define data set.
      */
    void setClients(QList<Person*>* tmp);
signals:


public slots:



private:
    QList<Person*>* m_dataList; /// data list

};

#endif // TCHATLISTMODEL_H
