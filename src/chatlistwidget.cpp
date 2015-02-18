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
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "chat.h"
#include "chatlist.h"
#include "MainWindow.h"
#include "privatechatdialog.h"


ChatListWidget::ChatListWidget(MainWindow * parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Chat messaging"));
    setObjectName("ChatListWidget");
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    m_chatList = new ChatList(parent);

    // UI

    m_privateChatDialog = new PrivateChatDialog(this);

    QListView * listView = new QListView(this);
    listView->setModel(m_chatList);
    listView->setIconSize(QSize(28,20));
    connect(listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(editChat(const QModelIndex &)));

    m_selectionModel = listView->selectionModel();
    connect(m_selectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(selectAnotherChat(const QModelIndex &)));
//    listView->installEventFilter(this);

    QPushButton * addChatButton = new QPushButton(tr("Add a chat"));
    connect(addChatButton, SIGNAL(pressed()), this, SLOT(createPrivateChat()));

    m_deleteButton = new QPushButton(tr("Remove a chat"));
    connect(m_deleteButton, SIGNAL(pressed()), this, SLOT(deleteSelectedChat()));
    m_deleteButton->setEnabled(false);

    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addChatButton);
    buttonLayout->addWidget(m_deleteButton);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 3, 3);
    layout->addWidget(listView);
    layout->addLayout(buttonLayout);

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

// This function is never called now.
bool ChatListWidget::eventFilter(QObject * object, QEvent * event)
{
    Q_UNUSED(object);

    if (event->type() == QEvent::FocusOut)
    {
        m_selectionModel->reset();
        m_deleteButton->setEnabled(false);
    }
    return false;
}

void ChatListWidget::createPrivateChat()
{
    PrivateChat * newChat = new PrivateChat(tr("New chat"));
    if (m_privateChatDialog->edit(newChat) == QDialog::Accepted)
    {
        m_chatList->addLocalChat(newChat);
    }
    else
        delete newChat;
}

void ChatListWidget::selectAnotherChat(const QModelIndex & index)
{
    AbstractChat * chat = m_chatList->chat(index);
    m_deleteButton->setEnabled(
            chat != NULL &&
            chat->inherits("PrivateChat") &&
            chat->belongsToLocalPlayer()
            );
}

void ChatListWidget::editChat(const QModelIndex & index)
{
    PrivateChat * chat = qobject_cast<PrivateChat *>(m_chatList->chat(index));
    if (chat != NULL)
        m_privateChatDialog->edit(chat);
}

void ChatListWidget::deleteSelectedChat()
{
    m_chatList->delLocalChat(m_selectionModel->currentIndex());
}
