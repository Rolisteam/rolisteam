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

#include <QObject>
#include <QPointer>
#include <memory>
#include <vector>

#include "controllerinterface.h"
#include "network/networkreceiver.h"

class PlayerModel;
class LocalPersonModel;
namespace InstantMessaging
{
class InstantMessagingModel;
class InstantMessagingUpdater;
class FilterInstantMessagingModel;
class ChatRoom;
class ChatroomSplitterModel;
} // namespace InstantMessaging

class InstantMessagingController : public AbstractControllerInterface, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(InstantMessaging::ChatroomSplitterModel* mainModel READ mainModel CONSTANT)
    Q_PROPERTY(InstantMessaging::ChatRoom* globalChatroom READ globalChatroom CONSTANT)
    Q_PROPERTY(PlayerModel* playerModel READ playerModel CONSTANT)
    Q_PROPERTY(LocalPersonModel* localPersonModel READ localPersonModel CONSTANT)
    Q_PROPERTY(QString localId READ localId WRITE setLocalId NOTIFY localIdChanged)
    Q_PROPERTY(bool nightMode READ nightMode WRITE setNightMode NOTIFY nightModeChanged)
public:
    explicit InstantMessagingController(PlayerModel* player, QObject* parent= nullptr);
    virtual ~InstantMessagingController();
    InstantMessaging::ChatroomSplitterModel* mainModel() const;
    InstantMessaging::ChatRoom* globalChatroom() const;
    PlayerModel* playerModel() const;
    LocalPersonModel* localPersonModel() const;
    QString localId() const;
    bool nightMode() const;

    void setGameController(GameController*) override;
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

public slots:
    void addChatroomSplitterModel();
    void detach(const QString& id, int index);
    void reattach(const QString& id, int index);
    void splitScreen(const QString& id, int index);
    void setLocalId(const QString& id);
    void addExtraChatroom(const QString& title, bool everyone, const QVariantList& recipiant);
    void setNightMode(bool mode);
    void openLink(const QString& link);

signals:
    void createdChatWindow(InstantMessaging::FilterInstantMessagingModel* model);
    void localIdChanged(QString);
    void nightModeChanged(bool);
    void openWebPage(QString);

private:
    std::unique_ptr<LocalPersonModel> m_localPersonModel;
    std::unique_ptr<InstantMessaging::InstantMessagingUpdater> m_updater;
    std::vector<std::unique_ptr<InstantMessaging::ChatroomSplitterModel>> m_splitterModels;
    std::unique_ptr<InstantMessaging::InstantMessagingModel> m_model;
    QPointer<PlayerModel> m_players;
    bool m_nightMode= false;
    int m_fontSizeFactor= false;
};

#endif // TEXTMESSAGINGCONTROLLER_H
