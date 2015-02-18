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
class ChatListModel;
class DicePlugInManager;
class QComboBox;
/**
  * @brief Chat window is the widget used for talking with other players in the game
  */
class Tchat : public SubMdiWindows
{
Q_OBJECT

public :
    /**
      * @brief default constructor
      */
    Tchat(CleverURI* uri,QWidget *parent = NULL);
    /**
      * @brief destructor
      */
    ~Tchat();

    /**
      * @brief called when message must be shown.
      */
    void showMessage(QString user, QColor color, QString msg);
    /**
      * @brief called when result from dice roll must be shown.
      */
    void showDiceRoll(QString user, QColor color, QString msg);

    /**
      * @brief must save conversation into the given file.
      * @param file's uri
      */
    virtual void saveFile(const QString& file);
    /**
      * @brief read conversation from the given file
      * @param file's uri
      */
    virtual void openFile(const QString& file);
    /**
     * @brief define menu for this window.
     */
    bool defineMenu(QMenu* /*menu*/);

    /**
      * @brief define all people involve in this conversation.
      * @param tmp : new client list
      */
    void setClients(QList<Person*>* tmp);
    virtual bool hasDockWidget() const;
    virtual QDockWidget* getDockWidget();

public slots:
    /**
      * @brief add person to the client list
      * @param person : pointer to the person
      */
    void addPerson(Person* );
    /**
      * @brief remove person from the client list
      * @param person : pointer to the person
      */
    void removePerson(Person*);


protected :
        /**
          * @brief catches the close event to hide the windows instead of close it.
          */
    void closeEvent(QCloseEvent *event);
    /**
      *  @brief catches show event to change the focus to the typing zone.
      */
    void showEvent(QShowEvent *event);

private :
    /**
      * @brief creates the graphical interface
      */
    void setupUi();

    QTextEdit* m_meetingRoom;
    TchatEditor* m_tchatEditor;
    QStringList m_messageHistoric;
    int m_historicNumber;
    QSplitter* m_splitter;
    QSplitter* m_HorizonSplitter;
    QList<Person*>* m_clientList;
    QListView* m_listView;
    ChatListModel* m_listModel;
    QComboBox* m_combobox;
    DicePlugInManager* m_diceManager;


private slots :
    /**
      * @brief called when user needs to go backward in his historic
      */
    void getUpHistoric();
    /**
      * @brief called when user needs to go forward in his historic
      */
    void getDownHistoric();
    /**
      * @brief called when user presses entry key, call the dice rolling system and send off the message to other clients.
      */
    void onEntry();

};

#endif
