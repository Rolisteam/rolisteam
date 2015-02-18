/***************************************************************************
    *     Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify     *
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
#ifndef USERLISTWIDGET_H
#define USERLISTWIDGET_H

#include <QDockWidget>
#include <QModelIndex>
#include <QSettings>


#include "messagemanager.h"

class QPushButton;
class QSlider;
class QLCDNumber;
class UserListView;
class UserListModel;
class QVBoxLayout;
class QHBoxLayout;
class Person;
class Player;
/**
    * @brief UserListWidget is dedicated to show informations about connected player and their character.
    */
class UserListWidget : public QDockWidget, public MessageManager
{
    Q_OBJECT
public:
    /**
    * @brief default constructor
    */
    explicit UserListWidget(QWidget *parent = 0);
    /**
    * @brief destructor
    */
    ~UserListWidget();
    /**
    * @brief transforms the selection into list of Person
    */
    QList<Person*>* getSelectedPerson();
    
    /**
    * @brief performs the message management
    */
    void processed(Message *);
    
    /**
    * @brief Load informations from the previous rolisteam's execution
    */
    void readSettings(QSettings &m);
    
    /**
    * @brief Save parameters for next executions.
    */
    void writeSettings(QSettings &m);
    
signals:
    /**
    * @brief emitted when Playable character has changed
    */
    void sizePcChanged(int);
    /**
    * @brief emitted when user required opening Instant messaging with someone
    */
    void opentchat();
    /**
    * @brief emitted when the widget is shown or hidden
    */
    void changeVisibility(bool);
    
public slots:
    /**
    * @brief adding Player and his characters into the model
    */
    void addPlayerToModel(Player* p);
    /**
    * @brief defines the given Player as the local one
    */
    void setLocalPlayer(Player* p);
    
    
protected:
    /**
    * @brief performs some task about user interface
    */
    void setupUI();
    /**
    * @brief initialisation of QAction
    */
    void setAction();
    /**
    * @brief catches closeevent to hide the widget instead of destroy it
    */
    void closeEvent ( QCloseEvent * event );
    
protected slots:
    /**
    * @brief add blank character to the local player
    */
    void addPC();
    /**
    * @brief deletes selected character
    */
    void delSelectedPC();
    /**
    * @brief update the current item (we must know the current item all the time)
    */
    void currentChanged(const QModelIndex&);
    
    
private:
    QPushButton* m_tchatButton; /// start chat
    QPushButton* m_addPC; /// add character to the local player
    QPushButton* m_delPC; /// delete caracter
    QSlider* m_sizePC; /// change size of character
    QLCDNumber* m_sizePCNumber; /// Diplays the size
    UserListView* m_view; ///  View of player/character tree
    UserListModel* m_model; /// Model of player/character tree
    
    QVBoxLayout* m_verticalLayout; /// layout
    QHBoxLayout* m_horizontalLayoutSlider;
    QHBoxLayout* m_horizontalLayoutButton;
    QWidget* m_centralWidget;
    Player* m_local;
    Player* m_gameMaster;
};

#endif // USERLISTWIDGET_H
