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
#include "shortcutmodel.h"
#include <QtDebug>

ShortCutItem::~ShortCutItem()
{

}

ShortCut::ShortCut(const QString &name, const QString &seq)
    : m_name(name),m_seq(seq)
{

}
QKeySequence ShortCut::getSequence() const
{
    return m_seq;
}

void ShortCut::setSequence(const QKeySequence &seq)
{
    m_seq = seq;
}

QString ShortCut::getName() const
{
    return m_name;
}

void ShortCut::setName(const QString &name)
{
    m_name = name;
}


/////////////////////////////////////////
Category::Category(const QString& name)
    : m_name(name)
{

}

QString Category::name() const
{
    return m_name;
}

void Category::setName(const QString &name)
{
    m_name = name;
}

ShortCut* Category::getShortCut(int i) const
{
    return m_shortcuts.at(i);
}
int Category::size() const
{
    return m_shortcuts.size();
}

bool Category::hasShortCut(ShortCut* cut) const
{
    return m_shortcuts.contains(cut);
}

int Category::indexOf(ShortCut *cut) const
{
    return m_shortcuts.indexOf(cut);
}

void Category::insertShortcut(const QString& name, const QString& key)
{
    m_shortcuts.append(new ShortCut(name,key));
}
/////////////////////////////////////////
ShortCutModel::ShortCutModel()
{

}

QModelIndex ShortCutModel::index(int r, int c, const QModelIndex &parent) const
{
    if(r<0)
        return QModelIndex();

    if (!parent.isValid())
    {
        return createIndex(r,c,m_root[r]);
    }
    else
    {
        Category* parentItem = static_cast<Category*>(parent.internalPointer());
        return createIndex(r,c,parentItem->getShortCut(r));
    }
}

QModelIndex ShortCutModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ShortCutItem* childItem = static_cast<ShortCutItem*>(index.internalPointer());

    if(!childItem->isLeaf())
    {
        return QModelIndex();
    }
    else
    {
        ShortCut* shortCut = dynamic_cast<ShortCut*>(childItem);
        if(nullptr != shortCut)
        {
            auto cat = std::find_if(m_root.begin(),m_root.end(),[=](Category* cat){
                   return cat->hasShortCut(shortCut);
            });
            return createIndex((*cat)->indexOf(shortCut), 0, *cat);
        }
    }
    return QModelIndex();
}

void ShortCutModel::addCategory(const QString& category)
{
    auto cat = std::find_if(m_root.begin(),m_root.end(),[=](Category* cat){
            return (cat->name() == category);
    });
    if(cat == m_root.end())
    {
        beginInsertRows(QModelIndex(),m_root.size(),m_root.size());
        m_root.append(new Category(category));
        endInsertRows();
    }
}

void ShortCutModel::insertShortCut(const QString& category,const QString& name,const QString& key)
{
    auto cat = std::find_if(m_root.begin(),m_root.end(),[=](Category* cat){
        if(cat->name() == category)
        {
            return true;
        }
        else
        {
            return false;
        }
    });

    QModelIndex idx = index(m_root.indexOf(*cat),0,QModelIndex());

    beginInsertRows(idx,(*cat)->size(),(*cat)->size());
    (*cat)->insertShortcut(name,key);
    endInsertRows();
}

int ShortCutModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
    {
        return m_root.size();
    }
    else
    {
        ShortCutItem* childItem = static_cast<ShortCutItem*>(parent.internalPointer());
        if(childItem->isLeaf())
        {
            return 0;
        }
        else
        {
            Category* cat = dynamic_cast<Category*>(childItem);
            if(nullptr != cat)
            {
                return cat->size();
            }
        }
    }
    return 0;
}

int ShortCutModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant ShortCutModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(Qt::DisplayRole == role)
    {

        ShortCutItem* childItem = static_cast<ShortCutItem*>(index.internalPointer());
        if(childItem->isLeaf())
        {
            ShortCut* shortCut = dynamic_cast<ShortCut*>(childItem);
            if(index.column() == 0)
            {
                return shortCut->getName();
            }
            else
            {
                return shortCut->getSequence().toString(QKeySequence::NativeText);
            }
        }
        else
        {
            Category* cat = dynamic_cast<Category*>(childItem);
            if(index.column() == 0)
            {
                 return cat->name();
            }
        }
    }
    return QVariant();
}






