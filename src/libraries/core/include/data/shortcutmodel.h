/***************************************************************************
 *   Copyright (C) 2017 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#include <core_global.h>
#include <vector>
#include <memory>

class CORE_EXPORT ShortCutItem
{
public:
    virtual ~ShortCutItem();
    virtual bool isLeaf() const= 0;
};

class CORE_EXPORT ShortCut : public ShortCutItem
{
public:
    ShortCut(const QString& name, const QString& seq);

    QKeySequence getSequence() const;
    void setSequence(const QKeySequence& seq);

    bool isLeaf() const override { return true; }

    QString getName() const;
    void setName(const QString& name);

private:
    QString m_name;
    QKeySequence m_seq;
};

class CORE_EXPORT Category : public ShortCutItem
{
public:
    Category(const QString& name);
    Category(Category&& other);

    QString name() const;
    void setName(const QString& name);

    ShortCut* getShortCut(int i) const;

    bool isLeaf() const override { return false; }

    int size() const;

    bool hasShortCut(ShortCut* cut) const;
    int indexOf(ShortCut* cut) const;

    void insertShortcut(const QString& name, const QString& key);

private:
    std::vector<std::unique_ptr<ShortCut>> m_shortcuts;
    QString m_name;
};
/**
 * @brief The ShortCutModel class
 */
class CORE_EXPORT ShortCutModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    ShortCutModel();

    virtual QModelIndex index(int, int, const QModelIndex& parent) const override;
    virtual QModelIndex parent(const QModelIndex& parent) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void insertShortCut(const QString& category, const QString& name, const QString& key);
    void addCategory(const QString& category);
    void removeCategory(const QString& category, bool isDestoyed = false);


private:
    std::vector<std::unique_ptr<Category>> m_root;
};

#endif
