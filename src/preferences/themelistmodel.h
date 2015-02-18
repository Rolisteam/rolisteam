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
#ifndef THEMELISTMODEL_H
#define THEMELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "theme.h"
/**
  * @brief themelistModel is part of MVC system which displays list of available theme.
  */
class ThemeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
      * @brief default constructor
      */
    explicit ThemeListModel(QObject *parent = 0);
    /**
      * @brief overridden function
      * @param parent
      * @return the rowCount of the given parent.
      */
    int rowCount ( const QModelIndex & parent) const;
    /**
      * @brief convert QModelIndex to data given the role.
      * @param index element to display
      * @param role: required data type
      * @return data in the QVariant type.
      */
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    /**
      * @brief defines flag for the given index.
      * @param index
      * @return composition of flags for index.
      */
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    /**
      * @brief Allows editing widget, useful for changing the theme name.
      */
    bool setData ( const QModelIndex & index, const QVariant & value, int role);

    /**
      * @brief accessor to theme, no verification about the out of range error.
      * @param row
      * @return theme stores at row
      */
    Theme getTheme(int row);
    /**
     * @brief Load informations from the previous rolisteam's execution
     */
    void readSettings();

    /**
     * @brief Save parameters for next executions.
     */
    void writeSettings();
public slots:
    /**
      * @brief remove the selected index
      * @param selected row
      */
    void removeSelectedTheme(int row);
    /**
      * @brief append theme into the list.
      */
    void addTheme(Theme tmp);
private:
    /**
      * @brief internal storage structure (typedef of QList<theme>)
      */
        ThemeList* m_themeList;
};

#endif // THEMELISTMODEL_H
