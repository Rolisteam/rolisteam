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

#include "data/person.h"
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
 * @brief chat window display on screen the chat.
 */    
class ChatWindow : public QWidget
{
Q_OBJECT

public :
    enum CHAT_OPERATOR {DICEROLL,SECRET_DICEROLL,COMMAND,TO_GM_DICEROLL};
    /**
     * @brief ChatWindow
     * @param chat
     * @param parent
     */
	ChatWindow(AbstractChat * chat,QWidget* parent = nullptr);
    /**
     * @brief ~ChatWindow
     */
    virtual ~ChatWindow();
    /**
     * @brief chat
     * @return
     */
    AbstractChat * chat() const;
    /**
     * @brief diceToText
     * @param dice
     * @return
     */
    QString diceToText(QList<ExportedDiceResult>& dice);
    /**
     * @brief toggleViewAction
     * @return
     */
    QAction* toggleViewAction() const;
    /**
     * @brief showMessage
     * @param utilisateur
     * @param couleur
     * @param message
     * @param msgtype
     */
    void showMessage(const QString &utilisateur, const QColor &couleur,const QString &message, NetMsg::Action msgtype = NetMsg::ChatMessageAction);
    /**
     * @brief hasUnseenMessage
     * @return
     */
    bool hasUnseenMessage() const;
    /**
     * @brief setSubWindow
     * @param subWindow
     */
    void setSubWindow(QMdiSubWindow* subWindow);
    /**
     * @brief getSubWindow
     * @return
     */
    QMdiSubWindow* getSubWindow();
    /**
     * @brief setLocalPlayer
     */
    void setLocalPlayer(Person*);

    /**
     * @brief updateDiceAliases
     * @param map
     */
    static void updateDiceAliases(QList<DiceAlias*>* map);

    ImprovedTextEdit *getEditionZone() const;

    void setEditionZone(ImprovedTextEdit *editionZone);

    bool isTimeShown() const;

signals:
    /**
     * @brief ChatWindowHasChanged
     * @param what
     */
    void ChatWindowHasChanged(ChatWindow * what);


public slots:
    /**
     * @brief isVisible
     * @return
     */
    virtual bool isVisible();
    /**
     * @brief save
     */
    void save();
    /**
     * @brief getTitleFromChat
     * @return
     */
    QString getTitleFromChat();
    /**
     * @brief editionGetFocus
     */
    void editionGetFocus();
    /**
     * @brief detachView
     * @param b
     */
    void detachView(bool b);

    void rollDiceCmd(QString cmd,QString owner);

    void showTime(bool showTime);

protected :
    /**
     * @brief init
     * @param parent
     */
    void init(MainWindow * parent);
    /**
     * @brief showEvent
     * @param event
     */
    void showEvent(QShowEvent *event);
    /**
     * @brief hideEvent
     * @param event
     */
    void hideEvent(QHideEvent *event);
    /**
     * @brief updateListAlias
     */
    void updateListAlias();

    void setProperDictionnary(QString idOwner);
private slots :
    /**
     * @brief sendOffTextMessage
     * @param hasHtml true if the given message has html
     * @param message to send.
     */
    void sendOffTextMessage(bool hasHtml,QString message);
    /**
     * @brief upSelectPerson
     */
    void upSelectPerson();
    /**
     * @brief downSelectPerson
     */
    void downSelectPerson();
    /**
     * @brief scheduleUpdateChatMembers
     */
    void scheduleUpdateChatMembers();
    /**
     * @brief updateChatMembers
     */
    void updateChatMembers();
    /**
     * @brief setupUi
     */
    void setupUi();
    /**
     * @brief getMessageResult
     */
    bool getMessageResult(QString& value, QString& command, QString& list);
    /**
     * @brief manageDiceRoll
     * @param str
     * @param secret
     */
    void manageDiceRoll(QString str,QString& messageTitle,QString& message, bool showResult = true);
private :
    static QStringList m_keyWordList;
    QMdiSubWindow* m_window;
    AbstractChat* m_chat;
    QString m_filename;
    bool m_warnedEmoteUnavailable;
    bool m_hasUnseenMessage;
    QPushButton* m_save;
    PreferencesManager* m_preferences;
    ChatBrowser * m_displayZone;
	QComboBox* m_selectPersonComboBox;
    ImprovedTextEdit * m_editionZone;
    QAction * m_toggleViewAction;
    QSplitter* m_splitter;
    QWidget * m_bottomWidget;
    DiceParser* m_diceParser;
    QMap<QString,CHAT_OPERATOR>* m_operatorMap;
    Person* m_localPerson;
    static QList<DiceAlias*>* m_receivedAlias;
    QHash<QString,QHash<QString,QString>*> m_dicoByCharacter;
    bool m_showTime;
};

#endif
