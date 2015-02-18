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

/**
* @brief is a dialog box display to define all parameter required for establishing a TCP/IP connection.
* @author Renaud Guezennec
*
*
*/
class ConnectionWizzard : public QDialog {
    Q_OBJECT
public:
	/**
	* @brief default constructor 
	*/
    ConnectionWizzard(QWidget *parent = 0);
	/**
	* @brief destructor 
	*/
    ~ConnectionWizzard();


signals:
    void connectionApply();
public slots:
	/**
	* @brief adds a new connection into the model 
	*/
    void addNewConnection();
	/**
	* @brief removes connection from the model. 
	*/
    void removeConnection();

protected slots:
	/**
	* @brief is called when the selection has changed.
	*/
    void selectionChanged();
	/**
	* @brief is called when user finished to edit the connection name.
	*/
    void editionFinished();
	/**
	* @brief is called when user click on any bottom buttom [Apply, OK,Cancel], only the apply is handled here. Other buttons are linked to dialog signals. 
	*/
    void onApply(QAbstractButton*);
	/**
	* @brief is called when some actions required to refresh the list.
	*/
    void updateList();
protected:
	/**
	* @brief handle for event, not used anymore.
	*/
    void changeEvent(QEvent *e);

private:
	/**
	* @brief pointeur to the GUI from Qtdesigner
	*/
    Ui::ConnectionWizzard *ui;
	/**
	* @brief current row
	*/
    int m_currentRow;
	/**
	* @brief storage structure which manages all connections.
	*/
    ConnectionList m_connectionList;

	/**
	* @brief current connection
	*/
    Connection* m_currentConnection;
    /**
      * @brief pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
};

#endif // CONNECTIONWIZZARD_H
