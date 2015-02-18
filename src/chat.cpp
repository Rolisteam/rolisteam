/*************************************************************************
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

#include "chat.h"

#include <QUuid>

#include "networkmessagereader.h"
#include "networkmessagewriter.h"
#include "persons.h"
#include "playersList.h"
#include "receiveevent.h"

extern bool G_client;

/****************
 * AbstractChat *
 ****************/


bool AbstractChat::belongsToLocalPlayer() const
{
    return false;
}


/**************
 * PublicChat *
 **************/

PublicChat::PublicChat()
{}

PublicChat::~PublicChat()
{}

QString PublicChat::identifier() const
{ 
    return QString("");
}

QString PublicChat::name() const
{ 
    return tr("Commun");
}

bool PublicChat::belongsTo(Player * player) const
{
    Q_UNUSED(player);
    return false;
}

void PublicChat::sendThem(NetworkMessage & message, Liaison * but) const
{
    message.sendAll(but);
}

bool PublicChat::everyPlayerHasFeature(const QString & feature, quint8 version) const
{
    return PlayersList::instance().everyPlayerHasFeature(feature, version);
}


/**************
 * PlayerChat *
 **************/

PlayerChat::PlayerChat(Player * player)
    : m_player(player)
{
    if (m_player == NULL)
        qFatal("PlayerChat with NULL player");
}

PlayerChat::~PlayerChat()
{}

QString PlayerChat::identifier() const
{
    return m_player->uuid();
}

QString PlayerChat::name() const
{
    return m_player->name();
}

bool PlayerChat::belongsTo(Player * player) const
{
    return (m_player == player);
}

void PlayerChat::sendThem(NetworkMessage & message, Liaison * but) const
{
    if (G_client)
        message.sendAll(but);
    else
    {
        Liaison * to = m_player->link();
        if (to != but)
            message.sendTo(to);
    }
}

bool PlayerChat::everyPlayerHasFeature(const QString & feature, quint8 version) const
{
    return m_player->hasFeature(feature, version);
}


/***************
 * PrivateChat *
 ***************/

/***
 * Network messages :
 ***
 * We use only two messages for private chats as UpdateChat is used to create a chat too.
 *
 * Tchat : UpdateChat
 * String8  uuid of the chat
 * String8  uuid of the owner
 * String16 name of the chat
 * uint8    number of players in chat
 * String8* uuid of each chat
 *
 * Tchat : DelChat
 * String8 uuid of the chat
 */

PrivateChat::PrivateChat(const QString & name)
    : m_name(name)
{
    m_uuid   = QUuid::createUuid().toString();
    m_owner = PlayersList::instance().localPlayer();
    m_set.insert(m_owner);
}

PrivateChat::PrivateChat(ReceiveEvent & event)
{
    NetworkMessageReader & data = event.data();
    PlayersList & g_playersList = PlayersList::instance();

    QString chatUuid  = data.string8();

    m_owner = g_playersList.getPlayer(data.string8());
    if (m_owner == NULL)
    {
        qWarning("New chat from an unknown player.");
        return;
    }

    if (!G_client and !sameLink(event.link()))
    {
        qWarning("%s is usurpating chat %s", qPrintable(m_owner->name()), qPrintable(chatUuid));
        return;
    }

    m_name = data.string16();

    for (quint8 i = data.uint8() ; i > 0 ; i--)
    {
        QString uuid = data.string8();
        m_set.insert(g_playersList.getPlayer(uuid));
    }

    m_uuid = chatUuid;
}

PrivateChat::~PrivateChat()
{}


QString PrivateChat::identifier() const
{
    return m_uuid;
}


QString PrivateChat::name() const
{
    return m_name;
}

bool PrivateChat::belongsToLocalPlayer() const
{
    return (m_owner == PlayersList::instance().localPlayer());
}

bool PrivateChat::belongsTo(Player * player) const
{
    return (m_owner == player);
}


void PrivateChat::sendThem(NetworkMessage & message, Liaison * but) const
{
    if (G_client)
    {
        message.sendAll(but);
        return;
    }

    Player * localPlayer = PlayersList::instance().localPlayer();
    foreach (Player * player, m_set)
    {
        if (player != localPlayer && player != NULL && player->link() != but)
            message.sendTo(player->link());
    }
}


bool PrivateChat::everyPlayerHasFeature(const QString & feature, quint8 version) const
{
    foreach (Player * player, m_set)
    {
        if (! player->hasFeature(feature, version))
            return false;
    }
    return true;
}

Player * PrivateChat::owner() const
{
    return m_owner;
}

bool PrivateChat::sameLink(Liaison * link)
{
    return (link == m_owner->link());
}

bool PrivateChat::includeLocalPlayer() const
{
    return m_set.contains(PlayersList::instance().localPlayer());
}

bool PrivateChat::removePlayer(Player * player)
{
    if (G_client)
    {
        qWarning("You're not allowed to delete a player from %s", qPrintable(m_uuid));
        return false;
    }

    return m_set.remove(player);
}

QSet<Player *> PrivateChat::players() const
{
    return m_set;
}


void PrivateChat::setPlayers(const QSet<Player *> & set)
{
    if (m_owner == PlayersList::instance().localPlayer())
    {
        m_set = set;
    }
    else
        qWarning("Can't modify directly a private chat not owned by the local player.");
}

void PrivateChat::sendUpdate() const
{
    NetworkMessageWriter message(NetMsg::ChatCategory, NetMsg::UpdateChatAction);
    message.string8(m_uuid);
    message.string8(m_owner->uuid());
    message.string16(m_name);

    quint8 size = m_set.size();
    message.uint8(size);

    quint8 i = 0;
    foreach (Player * player, m_set)
    {
        if (player != NULL)
        {
            message.string8(player->uuid());
            i++;
        }
    }
    if (i > size)
        qFatal("Too many players in the set for private chat %s", qPrintable(m_uuid));
    while (i < size)
    {
        qWarning("One player missing in the set for private chat %s. We add an invalid uuid.", qPrintable(m_uuid));
        message.string8(QString());
        i++;
    }

    sendThem(message, m_owner->link());
}


void PrivateChat::sendDel() const
{
    NetworkMessageWriter message(NetMsg::ChatCategory, NetMsg::DelChatAction);
    message.string8(m_uuid);
    sendThem(message);
}


void PrivateChat::set(const PrivateChat & other, bool thenUpdate)
{
    if (other.m_owner != m_owner)
    {
        qWarning("Can't set this private chat to one with a different owner");
        return;
    }
    p_set(other.m_name, other.m_set, thenUpdate);
}


void PrivateChat::set(const QString & name, const QSet<Player *> & set)
{
    if (m_owner != PlayersList::instance().localPlayer())
    {
        qWarning("Can't change this private chat locally.");
        return;
    }
    p_set(name, set);
}

void PrivateChat::p_set(const QString & name, QSet<Player *> set, bool thenUpdate)
{
    set.insert(m_owner);

    if (thenUpdate && !G_client)
    {
        m_set.subtract(set);
        if (m_set.size() > 0)
            sendDel();
    }

    m_name = name;
    emit changedName();
    m_set  = set;
    if (thenUpdate)
        sendUpdate();
}
