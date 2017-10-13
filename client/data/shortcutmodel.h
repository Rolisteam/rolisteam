/***************************************************************************
    *   Copyright (C) 2017 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef SHORTCUTMODEL_H
#define SHORTCUTMODEL_H

#include <QAbstractItemModel>
#include <QKeySequence>



class ShortCut
{
public:

private:
    QKeySequence m_seq;
	QString m_name;
};

class Category
{
public:

private:
 QList<ShortCut> m_shortcuts;
};

class ShortCutModel : public QAbstractItemModel
{
public:
	ShortCutModel();

    virtual QModelIndex index(int,int,const QModelIndex& parent) const;
    virtual QModelIndex parent(const QModelIndex& parent) const;

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;


    void insertShortCut(QString category, QString key);
    void addCategory(QString category);

private:
	QList<Category> m_root;
};

#endif
