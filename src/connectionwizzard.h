/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef CONNECTIONWIZZARD_H
#define CONNECTIONWIZZARD_H

#include <QDialog>
#include "connection.h"
namespace Ui {
    class ConnectionWizzard;
}
class QListWidgetItem;
class PreferencesManager;
class QAbstractButton;

class ConnectionWizzard : public QDialog {
    Q_OBJECT
public:
    ConnectionWizzard(QWidget *parent = 0);
    ~ConnectionWizzard();



public slots:
    void addNewConnection();
    void removeConnection();

protected slots:
    void selectionChanged();
    void editionFinished();
    void onApply(QAbstractButton*);
    void updateList();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConnectionWizzard *ui;
    int m_currentRow;
    ConnectionList m_connectionList;

    Connection* m_currentConnection;
    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
};

#endif // CONNECTIONWIZZARD_H
