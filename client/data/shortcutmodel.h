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
class ShortCutItem
{
public:
    virtual ~ShortCutItem();
    virtual bool isLeaf() const = 0;
};


class ShortCut : public ShortCutItem
{
public:
    ShortCut(const QString& name, const QString& seq);
    QKeySequence getSequence() const;
    void setSequence(const QKeySequence &seq);

    bool isLeaf() const override {return true;}

    QString getName() const;
    void setName(const QString &name);

private:
    QString m_name;
    QKeySequence m_seq;
};

class Category : public ShortCutItem
{
public:
    Category(const QString& name);

    QString name() const;
    void setName(const QString &name);

    ShortCut* getShortCut(int i) const;

    bool isLeaf() const override {return false;}

    int size() const;

    bool hasShortCut(ShortCut* cut) const;
    int indexOf(ShortCut* cut)const;

    void insertShortcut(const QString& name, const QString& key);

private:
    QList<ShortCut*> m_shortcuts;
    QString m_name;
};
/**
 * @brief The ShortCutModel class
 */
class ShortCutModel : public QAbstractItemModel
{
public:
    ShortCutModel();

    virtual QModelIndex index(int,int,const QModelIndex& parent) const;
    virtual QModelIndex parent(const QModelIndex& parent) const;

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;


    void insertShortCut(const QString& category, const QString& name,const QString& key);
    void addCategory(const QString& category);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
    QList<Category*> m_root;
};

#endif
