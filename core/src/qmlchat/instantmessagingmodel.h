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
#ifndef INSTANTMESSAGINGMODEL_H
#define INSTANTMESSAGINGMODEL_H

#include "qmlchat/chatroom.h"
#include <QAbstractListModel>
#include <memory>

namespace InstantMessaging
{
class MessageInterface;
class InstantMessagingModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString localId READ localId WRITE setLocalId NOTIFY localIdChanged)
public:
    enum Role
    {
        TitleRole= Qt::UserRole + 1,
        IdRole,
        TypeRole,
        ChatRole,
        HasUnreadMessageRole,
        ClosableRole,
        RecipiantCountRole
    };

    explicit InstantMessagingModel(QObject* parent= nullptr);
    virtual ~InstantMessagingModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    ChatRoom* globalChatRoom() const;
    QString localId() const;

public slots:
    void insertGlobalChatroom(const QString& title, const QString& uuid= QString());
    void insertIndividualChatroom(const QString& playerId, const QString& playerName);
    void insertExtraChatroom(const QString& title, const QStringList& playerIds, bool remote,
                             const QString& uuid= QString());
    void setLocalId(const QString& id);

    void addMessageIntoChatroom(MessageInterface*, ChatRoom::ChatRoomType type, const QString& uuid);
    void removePlayer(const QString& id);

signals:
    void chatRoomCreated(InstantMessaging::ChatRoom*, bool remote= false);
    void localIdChanged(QString);

private:
    void addChatRoom(ChatRoom* room, bool remote= false);

private:
    std::vector<std::unique_ptr<ChatRoom>> m_chats;
    QString m_localId;
};
} // namespace InstantMessaging
Q_DECLARE_METATYPE(InstantMessaging::InstantMessagingModel*)
#endif // INSTANTMESSAGINGMODEL_H
