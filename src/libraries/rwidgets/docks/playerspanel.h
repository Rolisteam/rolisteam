/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
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

#include "rwidgets_global.h"
#include <QItemSelectionModel>
#include <QPointer>
#include <QPushButton>
#include <QSet>
#include <QTreeView>
#include <memory>

class UserListView;
class PersonDialog;
class Person;
class PlayerOnMapModel;
class PlayerController;
class PreferencesManager;

namespace Ui
{
class PlayersPanel;
}

/**
 * @brief The PlayersListWidget class is the QDockWidget which display the PlayersListView. It is part of the MVC
 * pattern as the Controler.
 *
 */
class RWIDGET_EXPORT PlayersPanel : public QWidget
{
    Q_OBJECT
public:
    PlayersPanel(PlayerController* ctrl, PreferencesManager* pref, QWidget* parent= nullptr);
    ~PlayersPanel();

private:
    void setConnection();

signals:
    void runDiceForCharacter(const QString& dice, const QString& uuid);

private:
    Ui::PlayersPanel* m_ui;
    QPointer<PlayerController> m_ctrl;
    QPointer<QItemSelectionModel> m_selectionModel;
};

#endif
