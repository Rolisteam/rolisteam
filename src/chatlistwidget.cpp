/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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


#include "chatlistwidget.h"

#include <QEvent>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>

#include "chat.h"
#include "chatlist.h"
#include "MainWindow.h"
#include "privatechatdialog.h"


ChatListWidget::ChatListWidget(MainWindow * parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Tchats"));
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    m_chatList = new ChatList(parent);

    // UI

    m_privateChatDialog = new PrivateChatDialog(this);

    QListView * listView = new QListView(this);
    listView->setModel(m_chatList);
    listView->setIconSize(QSize(28,20));
    m_selectionModel = listView->selectionModel();
    listView->installEventFilter(this);

    QPushButton * addChatButton = new QPushButton(tr("Ajouter un tchat"));
    connect(addChatButton, SIGNAL(pressed()), this, SLOT(createPrivateChat()));

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 3, 3);
    layout->addWidget(listView);
    layout->addWidget(addChatButton);

    QWidget * mainWidget = new QWidget(this);
    mainWidget->setLayout(layout);
    setWidget(mainWidget);
}

ChatListWidget::~ChatListWidget()
{
}

QMenu * ChatListWidget::chatMenu() const
{
    return m_chatList->chatMenu();
}

QObject * ChatListWidget::chatList() const
{
    return m_chatList;
}

bool ChatListWidget::eventFilter(QObject * object, QEvent * event)
{
    Q_UNUSED(object);

    if (event->type() == QEvent::FocusOut)
        m_selectionModel->reset();
    return false;
}

void ChatListWidget::createPrivateChat()
{
    PrivateChat * newChat = new PrivateChat(tr("Nouveau Tchat"));
    if (m_privateChatDialog->edit(newChat) == QDialog::Accepted)
    {
        m_chatList->addLocalChat(newChat);
    }
    else
        delete newChat;
}
