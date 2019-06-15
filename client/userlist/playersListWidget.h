/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
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
#include <QPointer>
#include <QPushButton>
#include <QSet>
#include <QTreeView>
#include <memory>

#include "userlist/playerslistproxy.h"
class UserListView;
class Map;
class PersonDialog;
class Person;
/**
 * @brief The PlayersListWidgetModel class
 */
class PlayersListWidgetModel : public PlayersListProxyModel
{
    Q_OBJECT

public:
    PlayersListWidgetModel(QObject* parent= nullptr);

    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);

public slots:
    void setCurrentMap(Map* map);

private:
    QPointer<Map> m_map;
    bool isCheckable(const QModelIndex& index) const;
};

/**
 * @brief The PlayersListWidget class is the QDockWidget which display the PlayersListView. It is part of the MVC
 * pattern as the Controler.
 *
 */
class PlayersListWidget : public QDockWidget
{
    Q_OBJECT
public:
    PlayersListWidget(QWidget* parent= nullptr);
    ~PlayersListWidget();

    PlayersListWidgetModel* model() const;

public slots:
    void updateUi(bool isGM);

private:
    void setUI();

private slots:
    void editIndex(const QModelIndex& index);
    void createLocalCharacter();
    void selectAnotherPerson(const QModelIndex& current);
    void deleteSelected();
signals:
    void runDiceForCharacter(const QString& dice, const QString& uuid);

private:
    PersonDialog* m_personDialog;
    QItemSelectionModel* m_selectionModel;
    std::unique_ptr<PlayersListWidgetModel> m_model;
    UserListView* m_playersListView;
    QPushButton* m_delButton;
    QPushButton* m_addPlayerButton;
};

#endif
