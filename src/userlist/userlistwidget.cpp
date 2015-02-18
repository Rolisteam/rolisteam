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
#include <QPushButton>
#include <QSlider>
#include <QLCDNumber>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QCloseEvent>

#include "userlistwidget.h"
#include "userlistview.h"
#include "userlistmodel.h"

#include "person.h"
#include "player.h"

UserListWidget::UserListWidget(QWidget *parent) :
    QDockWidget(parent)
{
    setObjectName("UserListWidget");
    setupUI();
    setAction();

    m_model = new UserListModel;
    m_view->setModel(m_model);

}
UserListWidget::~UserListWidget()
{
    delete m_tchatButton;
    delete m_addPC;
    delete m_delPC;
    delete m_sizePC;
    delete m_sizePCNumber;
    delete m_view;
    delete m_model;
    delete m_centralWidget;
}
void UserListWidget::setupUI()
{

    m_tchatButton = new QPushButton(tr("Start Tchat"),this);
    m_addPC = new QPushButton(tr("Add PC"),this);
    m_delPC = new QPushButton(tr("delete selected PC"),this);
    setWindowTitle(tr("Characters and Players"));
    m_sizePC = new QSlider(Qt::Horizontal,this);
    m_sizePC->setMaximum(30);
    m_sizePC->setMinimum(0);
    m_sizePCNumber = new QLCDNumber(2,this);

    m_verticalLayout = new QVBoxLayout;
    m_horizontalLayoutSlider = new QHBoxLayout;
    m_horizontalLayoutButton= new QHBoxLayout;

    m_view = new UserListView(this);

    m_horizontalLayoutSlider->addWidget(m_sizePC);
    m_horizontalLayoutSlider->addWidget(m_sizePCNumber);
    m_horizontalLayoutButton->addWidget(m_addPC);
    m_horizontalLayoutButton->addWidget(m_delPC);


    m_verticalLayout->addWidget(m_tchatButton);
    m_verticalLayout->addWidget(m_view);
    m_verticalLayout->addLayout(m_horizontalLayoutSlider);
    m_verticalLayout->addLayout(m_horizontalLayoutButton);

    m_centralWidget = new QWidget();
    m_centralWidget->setLayout(m_verticalLayout);




    setWidget(m_centralWidget);
}
void UserListWidget::setAction()
{
    connect(m_sizePC,SIGNAL(valueChanged(int)),m_sizePCNumber,SLOT(display(int)));
    connect(m_addPC,SIGNAL(clicked()),this,SLOT(addPC()));
    connect(m_delPC,SIGNAL(clicked()),this,SLOT(delSelectedPC()));
    connect(m_tchatButton,SIGNAL(clicked()),this,SIGNAL(opentchat()));
    connect(m_view,SIGNAL(currentItemChanged(QModelIndex)),this,SLOT(currentChanged(QModelIndex)));
}
void UserListWidget::addUser(Person* p)
{
    m_model->addPlayer(p);
}
void UserListWidget::addPC()
{
    /**
     * @todo Add features to amend the avatar.
     */
    m_model->addCharacter(new Character(tr("New Character"),Qt::white,""),m_local);
}
void UserListWidget::delSelectedPC()
{
    QModelIndex p = m_view->currentIndex();
    m_model->removeCharacter(p,m_local);
}
QList<Person*>* UserListWidget::getSelectedPerson()
{
   return m_model->getSelectedPerson();
}

void UserListWidget::currentChanged(const QModelIndex& p)
{
    if(m_model->isPlayer(p))
    {
        m_addPC->setEnabled(true);
        m_delPC->setEnabled(false);
    }
    else
    {
        m_addPC->setEnabled(false);
        m_delPC->setEnabled(true);
    }
}
void UserListWidget::setLocalPlayer(Player* p)
{
    m_local = p;
    addUser(m_local);
    m_model->setLocalPlayer(p);
}
void UserListWidget::closeEvent ( QCloseEvent * event )
{
    QDockWidget::closeEvent(event);
    if(event->isAccepted())
    {
        emit changeVisibility(false);
    }
}
