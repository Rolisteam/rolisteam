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
#include "model/messagemodel.h"

#include <QDebug>
#include <set>

#include "data/person.h"
#include "instantmessaging/messagefactory.h"
#include "model/playermodel.h"

namespace InstantMessaging
{

MessageModel::MessageModel(PlayerModel* playerModel, QObject* parent)
    : QAbstractListModel(parent), m_personModel(playerModel)
{
}

MessageModel::~MessageModel()= default;

int MessageModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_messages.size());
}

QVariant MessageModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    int item= role;
    if(role == Qt::DisplayRole)
        item= TextRole;

    std::set<int> map({MessageTypeRole, TextRole, TimeRole, OwnerRole, LocalRole, MessageRole, WriterRole,
                       OwnerNameRole, OwnerColorRole, WriterNameRole, WriterColorRole});

    if(map.find(item) == map.end())
        return {};

    QVariant var;
    auto message= m_messages[static_cast<std::size_t>(index.row())].get();

    if(!message)
        return var;

    auto writer= m_personModel->personById(message->writer());
    auto owner= m_personModel->personById(message->owner());

    switch(item)
    {
    case MessageTypeRole:
        var= message->type();
        break;
    case TextRole:
        var= message->text();
        break;
    case MessageRole:
        var= QVariant::fromValue(message);
        break;
    case TimeRole:
        var= message->dateTime().toString("hh:mm");
        break;
    case LocalRole:
        var= (m_localId == message->owner());
        break;
    case WriterRole:
        var= message->writer();
        break;
    case OwnerRole:
        var= message->owner();
        break;
    case OwnerNameRole:
        var= owner ? owner->name() : tr("Invalid Person");
        break;
    case OwnerColorRole:
        var= owner ? owner->getColor() : Qt::gray;
        break;
    case WriterNameRole:
        var= writer ? writer->name() : tr("Invalid Person");
        break;
    case WriterColorRole:
        var= writer ? writer->getColor() : Qt::gray;
        break;
    }

    return var;
}

QHash<int, QByteArray> MessageModel::roleNames() const
{
    return QHash<int, QByteArray>({{MessageTypeRole, "type"},
                                   {TextRole, "text"},
                                   {TimeRole, "time"},
                                   {OwnerRole, "ownerId"},
                                   {OwnerNameRole, "ownerName"},
                                   {OwnerColorRole, "ownerColor"},
                                   {MessageRole, "message"},
                                   {WriterRole, "writerId"},
                                   {WriterNameRole, "writerName"},
                                   {WriterColorRole, "writerColor"},
                                   {LocalRole, "local"}});
}

QString MessageModel::localId() const
{
    return m_localId;
}

void MessageModel::setLocalId(const QString& localid)
{
    if(m_localId == localid)
        return;
    m_localId= localid;
    emit localIdChanged(m_localId);
}

void MessageModel::addMessage(const QString& text, const QDateTime& time, const QString& owner, const QString& writerId,
                              InstantMessaging::MessageInterface::MessageType type)
{
    auto msg= InstantMessaging::MessageFactory::createMessage(owner, writerId, time, type, text);
    if(!msg)
        return;
    addMessageInterface(msg);
    emit messageAdded(msg);
}

void MessageModel::addMessageInterface(MessageInterface* msg)
{
    std::unique_ptr<MessageInterface> interface(msg);

    beginInsertRows(QModelIndex(), 0, 0);
    m_messages.insert(m_messages.begin(), std::move(interface));
    endInsertRows();

    if(msg->owner() != localId())
        emit unreadMessageChanged();
}
} // namespace InstantMessaging
