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

class NetworkMessageReader;

namespace InstantMessaging
{
class ChatRoom;
class InstantMessagingModel;
class InstantMessagingUpdater : public QObject
{
    Q_OBJECT
public:
    explicit InstantMessagingUpdater(QObject* parent= nullptr);

    void addChatRoom(InstantMessaging::ChatRoom* chat, bool remote= false);
    static void sendMessage();
    static void openChat(InstantMessaging::ChatRoom* chat);
    static void closeChat();
    static void readChatroomToModel(InstantMessaging::InstantMessagingModel* model, NetworkMessageReader* msg);

    template <typename T>
    void sendOffChatRoomChanges(InstantMessaging::ChatRoom* chatRoom, const QString& property);
    void addMessageToModel(InstantMessaging::InstantMessagingModel* model, NetworkMessageReader* msg);
};

} // namespace InstantMessaging
#endif // INSTANTMESSAGINGUPDATER_H
