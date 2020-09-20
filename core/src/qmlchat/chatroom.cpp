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
#include "chatroom.h"

#include <QtConcurrent>
#include <set>

#include "diceparser.h"
#include "filteredplayermodel.h"
#include "messagemodel.h"

namespace InstantMessaging
{
namespace
{
InstantMessaging::MessageInterface::MessageType textToType(const QString& text)
{
    auto type= InstantMessaging::MessageInterface::Text;
    if(text.startsWith("!"))
        type= InstantMessaging::MessageInterface::Dice;
    else if(text.startsWith("/"))
        type= InstantMessaging::MessageInterface::Command;

    return type;
}
} // namespace
ChatRoom::ChatRoom(ChatRoomType type, const QStringList& recipiants, const QString& id, QObject* parent)
    : QObject(parent)
    , m_recipiants(new FilteredPlayerModel(recipiants))
    , m_messageModel(new MessageModel)
    , m_type(type)
    , m_uuid(id)
{
    connect(m_messageModel.get(), &MessageModel::localIdChanged, this, &ChatRoom::localIdChanged);
    connect(m_messageModel.get(), &MessageModel::unreadMessageChanged, this, [this]() { setUnreadMessage(true); });
}

ChatRoom::~ChatRoom()= default;

MessageModel* ChatRoom::messageModel() const
{
    return m_messageModel.get();
}

bool ChatRoom::unreadMessage() const
{
    return m_unreadMessage;
}

QString ChatRoom::uuid() const
{
    return m_uuid;
}

QString ChatRoom::localId() const
{
    return m_messageModel->localId();
}

FilteredPlayerModel* ChatRoom::recipiants() const
{
    return m_recipiants.get();
}

QString ChatRoom::title() const
{
    return m_title;
}

ChatRoom::ChatRoomType ChatRoom::type() const
{
    return m_type;
}

int ChatRoom::recipiantCount() const
{
    return m_recipiants->rowCount();
}

void ChatRoom::setUuid(const QString& id)
{
    if(id == m_uuid)
        return;
    m_uuid= id;
    emit uuidChanged(m_uuid);
}

void ChatRoom::setTitle(const QString& title)
{
    if(m_title == title)
        return;
    m_title= title;
    emit titleChanged(m_title);
}

void ChatRoom::setUnreadMessage(bool b)
{
    if(b == m_unreadMessage)
        return;
    m_unreadMessage= b;
    emit unreadMessageChanged(m_unreadMessage);
}

void ChatRoom::addMessage(const QString& text, const QString& personId, const QString& personName)
{
    auto type= textToType(text);
    using IM= InstantMessaging::MessageInterface;
    bool valid= false;
    if(type == IM::Dice)
    { // dice
        auto command= text;
        valid= rollDice(command.remove(0, 1), personId);
    }
    else if(type == IM::Command)
    {
        auto command= text;
        valid= runCommand(command.remove(0, 1), personId, personName);
    }

    if(type == IM::Text || !valid)
    {
        m_messageModel->addMessage(text, QDateTime::currentDateTime(), localId(),
                                   personId.isEmpty() ? localId() : personId, type);
    }
}

bool ChatRoom::rollDice(const QString& command, const QString& personId)
{
    using IM= InstantMessaging::MessageInterface;
    auto id= personId.isEmpty() ? localId() : personId;

    QFutureWatcher<std::pair<bool, QString>>* watcher= new QFutureWatcher<std::pair<bool, QString>>();

    connect(watcher, &QFutureWatcher<std::pair<bool, QString>>::finished, this, [this, watcher, id]() {
        auto result= watcher->result();
        m_messageModel->addMessage(result.second, QDateTime::currentDateTime(), localId(), id,
                                   result.first ? IM::Dice : IM::Error);
        delete watcher;
    });

    QFuture<std::pair<bool, QString>> future= QtConcurrent::run([this, command, id]() -> std::pair<bool, QString> {
        if(!m_diceParser->parseLine(command))
            return {};

        m_diceParser->start();
        auto error= m_diceParser->errorMap();
        if(!error.isEmpty())
            return {false, error.first()};

        auto result= m_diceParser->resultAsJSon([](const QString& value, const QString& color, bool highlight) {
            QString result= value;
            bool hasColor= !color.isEmpty();
            QString style;
            if(hasColor)
            {
                style+= QStringLiteral("color:%1;").arg(color);
            }
            if(highlight)
            {
                if(style.isEmpty())
                    style+= QStringLiteral("color:%1;")
                                .arg("red"); // default color must get the value from the setting object
                style+= QStringLiteral("font-weight:bold;");
            }
            if(!style.isEmpty())
                result= QString("<span style=\"%2\">%1</span>").arg(value).arg(style);
            return result;
        });

        return {true, result};
    });
    watcher->setFuture(future);
    return true;
}

bool ChatRoom::runCommand(const QString& command, const QString& personId, const QString& personName)
{
    QStringList meCommand({"emote ", "me ", "em ", "e "});
    auto text= command;
    for(auto e : meCommand)
    {
        if(text.startsWith(e))
        {
            text.remove(0, e.size());
            text= QString("<i>%2 %1<\\i>").arg(text).arg(personName);
            m_messageModel->addMessage(text, QDateTime::currentDateTime(), localId(),
                                       personId.isEmpty() ? localId() : personId,
                                       InstantMessaging::MessageInterface::Command);
            return true;
        }
    }

    return false;
}

void ChatRoom::addMessageInterface(MessageInterface* message)
{
    m_messageModel->addMessageInterface(message);
}

void ChatRoom::setLocalId(const QString& id)
{
    m_messageModel->setLocalId(id);
}
bool ChatRoom::hasRecipiant(const QString& id)
{
    return m_recipiants->hasRecipiant(id);
}

void ChatRoom::setDiceParser(DiceParser* diceParser)
{
    m_diceParser= diceParser;
}
} // namespace InstantMessaging
