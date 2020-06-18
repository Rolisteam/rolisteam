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
#ifndef CHATROOM_H
#define CHATROOM_H

#include <QObject>
#include <QUuid>

#include <memory>

namespace InstantMessaging
{
class FilteredPlayerModel;
class MessageModel;
class MessageInterface;
class ChatRoom : public QObject
{
    Q_OBJECT
    Q_PROPERTY(FilteredPlayerModel* recipiants READ recipiants CONSTANT)
    Q_PROPERTY(MessageModel* messageModel READ messageModel CONSTANT)
    Q_PROPERTY(bool unreadMessage READ unreadMessage WRITE setUnreadMessage NOTIFY unreadMessageChanged)
    Q_PROPERTY(ChatRoomType type READ type CONSTANT)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int recipiantCount READ recipiantCount NOTIFY recipiantCountChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString localId READ localId WRITE setLocalId NOTIFY localIdChanged)
public:
    enum ChatRoomType
    {
        GLOBAL,
        SINGLEPLAYER,
        EXTRA
    };
    explicit ChatRoom(ChatRoomType type, const QString& id= QUuid::createUuid().toString(QUuid::WithoutBraces),
                      QObject* parent= nullptr);
    virtual ~ChatRoom();

    MessageModel* messageModel() const;
    FilteredPlayerModel* recipiants() const;

    QString title() const;
    ChatRoomType type() const;
    int recipiantCount() const;
    bool unreadMessage() const;
    QString uuid() const;
    QString localId() const;

    bool hasRecipiant(const QString& id);

public slots:
    void setUuid(const QString& id);
    void setTitle(const QString& title);
    void setUnreadMessage(bool b);
    void addMessage(const QString& text);
    void addMessageInterface(MessageInterface* message);
    void setLocalId(const QString& id);

signals:
    void unreadMessageChanged(bool);
    void titleChanged(QString title);
    void recipiantCountChanged(int);
    void uuidChanged(QString);
    void localIdChanged(QString);

private:
    std::unique_ptr<FilteredPlayerModel> m_recipiants;
    std::unique_ptr<MessageModel> m_messageModel;
    QString m_title;
    ChatRoomType m_type;
    QString m_uuid;
    bool m_unreadMessage= false;
};
} // namespace InstantMessaging
#endif // CHATROOM_H
