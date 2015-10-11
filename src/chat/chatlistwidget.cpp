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


#include "chat/chatlistwidget.h"

#include <QEvent>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "chat/chat.h"
#include "chatlist.h"
#include "mainwindow.h"
#include "privatechatdialog.h"
#include "dicealias.h"

ChatListWidget::ChatListWidget(MainWindow * parent)
    : QDockWidget(/*parent*/),m_diceAliasMapFromGM(new QList<DiceAlias*>())
{
    Q_UNUSED(parent);
    setWindowTitle(tr("Chat messaging"));
    setObjectName("ChatListWidget");
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    m_chatList = new ChatList(parent);

    // UI

    m_privateChatDialog = new PrivateChatDialog(this);

    m_listView = new QListView(this);
    BlinkingDecorationDelegate* blinkingDelegate = new BlinkingDecorationDelegate();
    connect(blinkingDelegate,SIGNAL(refresh()),this,SLOT(updateAllUnreadChat()));

    m_listView->setItemDelegateForColumn(0,blinkingDelegate);
    m_listView->setModel(m_chatList);
    m_listView->setIconSize(QSize(28,20));
    connect(m_listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(editChat(const QModelIndex &)));

    m_selectionModel = m_listView->selectionModel();
    connect(m_selectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(selectAnotherChat(const QModelIndex &)));
//    listView->installEventFilter(this);

    QPushButton * addChatButton = new QPushButton(tr("Add a chat"));
    connect(addChatButton, SIGNAL(clicked()), this, SLOT(createPrivateChat()));

    m_deleteButton = new QPushButton(tr("Remove a chat"));
    connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedChat()));
    m_deleteButton->setEnabled(false);

    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addChatButton);
    buttonLayout->addWidget(m_deleteButton);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 3, 3);
    layout->addWidget(m_listView);
    layout->addLayout(buttonLayout);

    QWidget * mainWidget = new QWidget(this);
    mainWidget->setLayout(layout);
    setWidget(mainWidget);
}

ChatListWidget::~ChatListWidget()
{
}
void ChatListWidget::updateAllUnreadChat()
{
    for(int i=0;i<m_chatList->rowCount();++i)
    {
        QModelIndex index = m_chatList->index(i,0);
        if(index.data(Qt::DecorationRole).value<QColor>().red()==255)
        {
            m_listView->update(index);
        }
    }
}
QMenu* ChatListWidget::chatMenu() const
{
    return m_chatList->chatMenu();
}

QObject* ChatListWidget::chatList() const
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
    AbstractChat* chat = m_chatList->chat(index);
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
NetWorkReceiver::SendType ChatListWidget::processMessage(NetworkMessageReader* msg, NetworkLink* link)
{
    switch (msg->action())
    {
    case NetMsg::addDiceAlias:
        processAddDiceAlias(msg);
        break;
    case NetMsg::removeDiceAlias:
        processRemoveDiceALias(msg);
        break;
    case NetMsg::moveDiceAlias:
        processMoveDiceALias(msg);
        break;
    default:
        break;
    }
}
void ChatListWidget::processAddDiceAlias(NetworkMessageReader* msg)
{
  int pos = msg->int64();
  QString regex = msg->string32();
  QString value = msg->string32();
  bool isReplace = (bool)msg->int8();
  bool isEnable = (bool)msg->int8();
  DiceAlias* tmp = new DiceAlias(regex,value,isReplace,isEnable);

  m_diceAliasMapFromGM->insert(pos,tmp);

  m_chatList->updateDiceAliases(m_diceAliasMapFromGM);
}
void ChatListWidget::processRemoveDiceALias(NetworkMessageReader* msg)
{
    int pos = msg->int64();
    if(m_diceAliasMapFromGM->size()>pos)
    {
        m_diceAliasMapFromGM->removeAt(pos);
    }
}
void ChatListWidget::processMoveDiceALias(NetworkMessageReader* msg)
{
    int from = msg->int64();
    int to   = msg->int64();
    if((from>=0)&&(from<m_diceAliasMapFromGM->size()))
    {
        DiceAlias* tpm = m_diceAliasMapFromGM->takeAt(from);
        m_diceAliasMapFromGM->insert(to,tpm);

    }

}
