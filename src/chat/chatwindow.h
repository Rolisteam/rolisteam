/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
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


#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QComboBox>
#include <QFile>
#include <QSplitter>
#include <QTextBrowser>
#include <QMdiSubWindow>

#include "network/networkmessage.h"
#include "preferences/preferencesmanager.h"
#include "widgets/colorbutton.h"
#include "chatbrowser.h"

#include "diceparser.h"

//#include "ui_chatwindow.h"

class AbstractChat;
class MainWindow;
class Player;
class ImprovedTextEdit;

namespace Ui {
class ChatWindow;
}
/**
 * @brief chat window.
 */    
class ChatWindow : public QWidget
{
Q_OBJECT

public :
    ChatWindow(AbstractChat * chat, MainWindow * parent);
    ~ChatWindow();
    AbstractChat * chat() const;
    QAction * toggleViewAction() const;
    void showMessage(const QString &utilisateur, const QColor &couleur,const QString &message, NetMsg::Action msgtype = NetMsg::ChatMessageAction);
    bool hasUnseenMessage() const;
    void setSubWindow(QMdiSubWindow* subWindow);
    QMdiSubWindow* getSubWindow();

signals:
    void ChatWindowHasChanged(ChatWindow * what);

public slots:
    virtual bool isVisible();
    void save();
    QString getTitleFromChat();
    void editionGetFocus();

protected :
    void init(MainWindow * parent);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private slots :
    void emettreTexte(QString messagehtml,QString message);
    void upSelectPerson();
    void downSelectPerson();
    void scheduleUpdateChatMembers();
    void updateChatMembers();
    void setupUi();
    void getMessageResult(QString&);

private :
    static QStringList m_keyWordList;
    QMdiSubWindow* m_window;
    AbstractChat * m_chat;
    QString m_filename;
    bool m_warnedEmoteUnavailable;
    bool m_hasUnseenMessage;
    MainWindow* m_mainWindow;
    QPushButton* m_save;
    PreferencesManager* m_preferences;
    ChatBrowser * m_displayZone;
    QComboBox * m_selectPersonComboBox;
    ImprovedTextEdit * m_editionZone;
    QAction * m_toggleViewAction;
    QSplitter* m_splitter;
    QWidget * m_bottomWidget;
    DiceParser* m_diceParser;
};

#endif
