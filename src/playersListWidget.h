/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#ifndef PLAYERS_LIST_WIDGET_H
#define PLAYERS_LIST_WIDGET_H

#include <QAbstractProxyModel>
#include <QDockWidget>
#include <QItemSelectionModel>
#include <QPushButton>
#include <QSet>
#include <QTreeView>

#include "playerslistproxy.h"

class Carte;
class PersonDialog;
class Person;
/**
 * @brief The PlayersListWidgetModel class
 */
class PlayersListWidgetModel : public PlayersListProxyModel
{
    Q_OBJECT

    public:
        PlayersListWidgetModel(QObject * parent = 0);

        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);

    public slots:
        void changeMap(Carte * map);

    private:
       Carte * m_map;

       bool isCheckable(const QModelIndex &index) const;
};

/**
 * @brief A QTreeView with editable DecorationRole if it's a QColor.
 * @todo The code is really generic. The class might be put somewhere else to be reused.
 */
class PlayersListView : public QTreeView
{
    Q_OBJECT

    public:
        PlayersListView(QWidget * parent = NULL);
        ~PlayersListView();

    protected:
        void mouseDoubleClickEvent(QMouseEvent * event);
};

class PlayersListWidget : public QDockWidget
{
    Q_OBJECT

    public:
        PlayersListWidget(QWidget * parent = NULL);
        ~PlayersListWidget();

        PlayersListWidgetModel * model() const;

    private slots:
        void editIndex(const QModelIndex & index);
        void createLocalCharacter();
        void selectAnotherPerson(const QModelIndex & current);
        void deleteSelected();

    private:
        PersonDialog           * m_personDialog;
        QItemSelectionModel    * m_selectionModel;
        QPushButton            * m_delButton;
        PlayersListWidgetModel * m_model;

        void setUI();
};

#endif
