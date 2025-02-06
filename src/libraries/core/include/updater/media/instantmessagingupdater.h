/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#ifndef INSTANTMESSAGINGUPDATER_H
#define INSTANTMESSAGINGUPDATER_H

#include <QObject>
#include <QPointer>

#include "controller/instantmessagingcontroller.h"
#include "network/networkreceiver.h"
#include <core_global.h>

class NetworkMessageReader;
namespace InstantMessaging
{
class ChatRoom;
class InstantMessagingModel;
class CORE_EXPORT InstantMessagingUpdater : public QObject, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(bool saveChatrooms READ saveChatrooms WRITE setSaveChatrooms NOTIFY saveChatroomsChanged FINAL)
public:
    explicit InstantMessagingUpdater(InstantMessagingController* ctrl, QObject* parent= nullptr);

    void addChatRoom(InstantMessaging::ChatRoom* chat, bool remote= false);
    static void sendMessage();
    static void openChat(InstantMessaging::ChatRoom* chat);
    static void closeChat();
    static void readChatroomToModel(InstantMessaging::InstantMessagingModel* model, NetworkMessageReader* msg);

    template <typename T>
    void sendOffChatRoomChanges(InstantMessaging::ChatRoom* chatRoom, const QString& property);
    void addMessageToModel(InstantMessaging::InstantMessagingModel* model, NetworkMessageReader* msg);
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

    void save(const QString& path);
    void load(const QString& path);

    bool saveChatrooms() const;
    void setSaveChatrooms(bool newSaveChatrooms);

public slots:
    void removeChatRoom(const QString &id, bool remote);
signals:
    void saveChatroomsChanged();

private:
    QPointer<InstantMessagingController> m_imCtrl;
    bool m_saveChatrooms{false};
};

} // namespace InstantMessaging
#endif // INSTANTMESSAGINGUPDATER_H
