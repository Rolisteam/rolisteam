/***************************************************************************
 *	Copyright (C) 2011 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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
#include "themelistmodel.h"
#include "theme.h"

#include <QSettings>
#include <QDebug>

ThemeListModel::ThemeListModel(QObject *parent) :
    QAbstractListModel(parent)
{
        m_themeList=new ThemeList();
}
void ThemeListModel::addTheme(Theme tmp)
{
    beginInsertRows (QModelIndex(),  m_themeList->size(), m_themeList->size()+1 );

    m_themeList->append(tmp);
    endInsertRows ();
}
int ThemeListModel::rowCount ( const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_themeList->size();
}
Qt::ItemFlags ThemeListModel::flags ( const QModelIndex & index ) const
{
    return Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled;
}
bool ThemeListModel::setData ( const QModelIndex & index, const QVariant & value, int role)
{
    if(!index.isValid())
        return false;
    if(role==Qt::EditRole)
    {
        (*m_themeList)[index.row()].setName(value.toString());
        return true;
    }

    return false;
}

QVariant ThemeListModel::data ( const QModelIndex & index, int role ) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row()>=m_themeList->size())
        return  QVariant();

    if((Qt::DisplayRole == role)||(role==Qt::EditRole))
    {
        return m_themeList->at(index.row()).name();
    }
    return QVariant();
}
void ThemeListModel::removeSelectedTheme(int row)
{
    beginRemoveRows(QModelIndex(),row,row+1);
    m_themeList->removeAt(row);
    endRemoveRows();
}
Theme ThemeListModel::getTheme(int row)
{
    return m_themeList->at(row);
}
void ThemeListModel::readSettings()
{
    qDebug() << "read setting in theme model";

    qRegisterMetaTypeStreamOperators<Theme>("Theme");
    qRegisterMetaTypeStreamOperators<ThemeList>("ThemeList");

    QSettings settings("RolisteamTeam", "Rolisteam/preferencesDialog");
    QVariant variantmp;
    variantmp.setValue(*m_themeList);
    QVariant variant = settings.value("themeslist",variantmp);
    if(variant.canConvert<ThemeList>())
        *m_themeList = variant.value<ThemeList>();

    qDebug() << "end read setting in theme model" << m_themeList->size();

}

void ThemeListModel::writeSettings()
{
    qDebug() << "write setting in theme model" << m_themeList->size();
    QSettings settings("RolisteamTeam", "Rolisteam/preferencesDialog");
    QVariant variantmp;
    variantmp.setValue(*m_themeList);
    settings.setValue("themeslist",variantmp);

}
