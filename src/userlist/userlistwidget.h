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

class QPushButton;
class QSlider;
class QLCDNumber;
class UserListView;
class UserListModel;
class QVBoxLayout;
class QHBoxLayout;
class Person;
class Player;
class UserListWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit UserListWidget(QWidget *parent = 0);
    ~UserListWidget();

    QList<Person*>* getSelectedPerson();
protected:
    void setupUI();
    void setAction();
signals:
    void sizePcChanged(int);
    void opentchat();

public slots:
    void addUser(Person* p);
    void setLocalPlayer(Player* p);


protected slots:
    void addPC();
    void delSelectedPC();
    void currentChanged(const QModelIndex&);


private:
    QPushButton* m_tchatButton;
    QPushButton* m_addPC;
    QPushButton* m_delPC;
    QSlider* m_sizePC;
    QLCDNumber* m_sizePCNumber;
    UserListView* m_view;
    UserListModel* m_model;

    QVBoxLayout* m_verticalLayout;
    QHBoxLayout* m_horizontalLayoutSlider;
    QHBoxLayout* m_horizontalLayoutButton;
    QWidget* m_centralWidget;
    Player* m_local;
    Player* m_gameMaster;
};

#endif // USERLISTWIDGET_H
