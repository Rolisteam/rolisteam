/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

/********************************************************************/
/*                                                                  */
/* Fenetre permettant aux utilisateurs de communiquer entre eux par */
/* ecrit. Il existe un tchat par utilisateur distant, et un tchat   */
/* commun permettant de discuter avec tous les utilisateurs.        */
/*                                                                  */
/********************************************************************/	


#ifndef TCHAT_H
#define TCHAT_H

#include <QMdiSubWindow>

#include <QSplitter>
#include <QFile>
#include <QList>
#include "tchateditor.h"
#include "submdiwindows.h"

class Person;
class QListView;
class TchatListModel;
class DicePlugInManager;
class QComboBox;
class Tchat : public SubMdiWindows
{
Q_OBJECT

public :
    Tchat(QWidget *parent);
    ~Tchat();

    void showMessage(QString user, QColor color, QString msg);
    void showDiceRoll(QString user, QColor color, QString msg);
    void updateActions();
    virtual void saveFile(QString& file);
    virtual void openFile(QString& file);
    bool defineMenu(QMenu* /*menu*/);

    void setClients(QList<Person*>* tmp);

public slots:
    void addPerson(Person* );
    void removePerson(Person*);


protected :
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);

private :

    void setupUi();

    QTextEdit* m_meetingRoom;
    TchatEditor* m_tchatEditor;
    QStringList m_messageHistoric;
    int m_historicNumber;
    QSplitter* m_splitter;
    QSplitter* m_HorizonSplitter;
    QList<Person*>* m_clientList;
    QListView* m_listView;
    TchatListModel* m_listModel;
    QComboBox* m_combobox;
    DicePlugInManager* m_diceManager;


private slots :
    void getUpHistoric();
    void getDownHistoric();
    void onEntry();

};

#endif
