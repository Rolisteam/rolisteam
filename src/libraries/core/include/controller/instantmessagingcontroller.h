/***************************************************************************
 *	 Copyright (C) 2020 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef TEXTMESSAGINGCONTROLLER_H
#define TEXTMESSAGINGCONTROLLER_H

#include <QFont>
#include <QObject>
#include <QPointer>
#include <memory>
#include <vector>

#include "controllerinterface.h"
#include "data/chatroom.h"
#include "data/localpersonmodel.h"
#include "model/chatroomsplittermodel.h"
#include "model/filterinstantmessagingmodel.h"
#include "model/instantmessagingmodel.h"
#include "model/playermodel.h"

#include <core_global.h>

class DiceRoller;

namespace InstantMessaging
{
class InstantMessagingUpdater;
} // namespace InstantMessaging

class CORE_EXPORT InstantMessagingController : public AbstractControllerInterface
{
    Q_OBJECT
    Q_PROPERTY(InstantMessaging::ChatroomSplitterModel* mainModel READ mainModel CONSTANT)
    Q_PROPERTY(InstantMessaging::ChatRoom* globalChatroom READ globalChatroom CONSTANT)
    Q_PROPERTY(PlayerModel* playerModel READ playerModel CONSTANT)
    Q_PROPERTY(LocalPersonModel* localPersonModel READ localPersonModel CONSTANT)
    Q_PROPERTY(QString localId READ localId WRITE setLocalId NOTIFY localIdChanged)
    Q_PROPERTY(bool nightMode READ nightMode WRITE setNightMode NOTIFY nightModeChanged)
    Q_PROPERTY(bool sound READ sound WRITE setSound NOTIFY soundChanged FINAL)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool unread READ unread NOTIFY unreadChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged FINAL)
    Q_PROPERTY(InstantMessaging::InstantMessagingModel* model READ model CONSTANT)
public:
    explicit InstantMessagingController(DiceRoller* diceRoller, PlayerModel* player, QObject* parent= nullptr);
    virtual ~InstantMessagingController();
    InstantMessaging::ChatroomSplitterModel* mainModel() const;
    InstantMessaging::ChatRoom* globalChatroom() const;
    InstantMessaging::ChatRoom* gmChatroom() const;
    PlayerModel* playerModel() const;
    LocalPersonModel* localPersonModel() const;
    QString localId() const;
    bool nightMode() const;
    bool visible() const;
    bool unread() const;

    void setGameController(GameController*) override;

    InstantMessaging::InstantMessagingModel* model() const;

    bool sound() const;
    void setSound(bool newSound);

    QFont font() const;
    void setFont(const QFont& newFont);

public slots:
    void addChatroomSplitterModel();
    void closeChatroom(const QString& id, bool network);
    void detach(const QString& id, int index);
    void reattach(const QString& id, int index);
    void splitScreen(const QString& id, int index);
    void setLocalId(const QString& id);
    void addExtraChatroom(const QString& title, bool everyone, const QVariantList& recipiant);
    void setNightMode(bool mode);
    void openLink(const QString& link);
    void setDiceParser(DiceRoller* diceParser);
    void setVisible(bool b);

    void translateDiceResult(const QHash<int, QList<int>>& rollResult, const QString& rest);
    void rollDiceCommand(const QString& cmd, bool gmOnly, const QString& characterId);
    void sendMessageToGlobal(const QString& msg, const QString& characterId);
    void sendMessageToGM(const QString& msg, const QString& characterId);

signals:
    void createdChatWindow(InstantMessaging::FilterInstantMessagingModel* model);
    void localIdChanged(QString);
    void nightModeChanged(bool);
    void openWebPage(QString);
    void visibleChanged(bool);
    void unreadChanged();
    void chatRoomCreated(InstantMessaging::ChatRoom* room, bool remote);
    void chatRoomRemoved(const QString& id, bool remote);
    void soundChanged();
    void fontChanged();

private:
    std::unique_ptr<LocalPersonModel> m_localPersonModel;
    std::vector<std::unique_ptr<InstantMessaging::ChatroomSplitterModel>> m_splitterModels;
    std::unique_ptr<InstantMessaging::InstantMessagingModel> m_model;
    QPointer<PlayerModel> m_players;
    bool m_nightMode= false;
    int m_fontSizeFactor= false;
    QPointer<DiceRoller> m_diceParser;
    bool m_visible= false;
    bool m_sound{true};
    QFont m_font;
};

#endif // TEXTMESSAGINGCONTROLLER_H
