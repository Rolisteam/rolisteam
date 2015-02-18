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


#include <QListView>
#include <QEvent>

#include "chatlist.h"
#include "MainWindow.h"

#include "chatlistwidget.h"


ChatListWidget::ChatListWidget(MainWindow * parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Tchats"));
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    m_chatList = new ChatList(parent);

    QListView * listView = new QListView(this);
    listView->setModel(m_chatList);
    setWidget(listView);

    m_selectionModel = listView->selectionModel();
    listView->installEventFilter(this);
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
