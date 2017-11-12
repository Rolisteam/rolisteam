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

#include "chat/chat.h"

#include <QUuid>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "data/person.h"
#include "userlist/playersList.h"
#include "network/receiveevent.h"
#include "preferences/preferencesmanager.h"

#include "data/player.h"

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
{
    PlayersList* g_playersList = PlayersList::instance();
    connect(g_playersList, SIGNAL(playerAdded(Player *)), this, SIGNAL(changedMembers()));
    connect(g_playersList, SIGNAL(playerDeleted(Player *)), this, SIGNAL(changedMembers()));
}

PublicChat::~PublicChat()
{}

QString PublicChat::identifier() const
{ 
    return QString("");
}

QString PublicChat::name() const
{ 
    return tr("Global");
}

bool PublicChat::belongsTo(Player * player) const
{
    Q_UNUSED(player);
    return false;
}

void PublicChat::sendThem(NetworkMessage & message, NetworkLink * but) const
{
    message.sendAll(but);
}

bool PublicChat::everyPlayerHasFeature(const QString & feature, quint8 version) const
{
    return PlayersList::instance()->everyPlayerHasFeature(feature, version);
}


/**************
 * PlayerChat *
 **************/

PlayerChat::PlayerChat(Player * player)
    : m_player(player)
{
    if (m_player == nullptr)
        qFatal("PlayerChat with nullptr player");
    connect(PlayersList::instance(), SIGNAL(playerChanged(Player *)),
            this, SLOT(verifyName(Player *)));
}

PlayerChat::~PlayerChat()
{}

QString PlayerChat::identifier() const
{
    return m_player->getUuid();
}

QString PlayerChat::name() const
{
    return m_player->getName();
}

bool PlayerChat::belongsTo(Player * player) const
{
    return (m_player == player);
}

void PlayerChat::sendThem(NetworkMessage & message, NetworkLink * but) const
{
    if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        message.sendAll(but);
    }
    else
    {
        NetworkLink* to = m_player->link();
        if (to != but)
        {
            message.sendTo(to);
        }
    }
}

bool PlayerChat::everyPlayerHasFeature(const QString & feature, quint8 version) const
{
    return m_player->hasFeature(feature, version);
}

void PlayerChat::verifyName(Player * player)
{
    if (player == m_player)
        emit changedName(m_player->getName());
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
    m_owner = PlayersList::instance()->getLocalPlayer();
    m_set.insert(m_owner);
}

PrivateChat::PrivateChat(ReceiveEvent & event)
{
    NetworkMessageReader & data = event.data();
    PlayersList* g_playersList = PlayersList::instance();

    QString chatUuid  = data.string8();

    m_owner = g_playersList->getPlayer(data.string8());
    if (m_owner == nullptr)
    {
        qWarning("New chat from an unknown player.");
        return;
    }

    if ((!PreferencesManager::getInstance()->value("isClient",true).toBool()) && (!sameLink(event.link())))
    {
        qWarning("%s is usurpating chat %s", qPrintable(m_owner->getName()), qPrintable(chatUuid));
        return;
    }

    m_name = data.string16();

    for (quint8 i = data.uint8() ; i > 0 ; i--)
    {
        QString uuid = data.string8();
        m_set.insert(g_playersList->getPlayer(uuid));
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
    return (m_owner == PlayersList::instance()->getLocalPlayer());
}

bool PrivateChat::belongsTo(Player * player) const
{
    return (m_owner == player);
}


void PrivateChat::sendThem(NetworkMessage & message, NetworkLink * but) const
{
    p_sendThem(message, but, false);
}

void PrivateChat::p_sendThem(NetworkMessage & message, NetworkLink * but, bool force) const
{
    if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        if (force || m_set.size() > 1)
            message.sendAll(but);
        return;
    }

    Player * localPlayer = PlayersList::instance()->getLocalPlayer();
    foreach (Player * player, m_set)
    {
        if (player != localPlayer && player != nullptr && player->link() != but)
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

bool PrivateChat::sameLink(NetworkLink * link)
{
    return (link == m_owner->link());
}

bool PrivateChat::includeLocalPlayer() const
{
    return m_set.contains(PlayersList::instance()->getLocalPlayer());
}

bool PrivateChat::removePlayer(Player * player)
{
    if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        qWarning("You're not allowed to delete a player from %s", qPrintable(m_uuid));
        return false;
    }

    bool ret = m_set.remove(player);
    if (ret)
        emit changedMembers();
    return ret;
}

QSet<Player *> PrivateChat::players() const
{
    return m_set;
}


void PrivateChat::sendUpdate() const
{
    NetworkMessageWriter message(NetMsg::ChatCategory, NetMsg::UpdateChatAction);
    message.string8(m_uuid);
    message.string8(m_owner->getUuid());
    message.string16(m_name);

    quint8 size = m_set.size();
    message.uint8(size);

    quint8 i = 0;
    foreach (Player * player, m_set)
    {
        if (player != nullptr)
        {
            message.string8(player->getUuid());
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

    p_sendThem(message, m_owner->link(), true);
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
    if (m_owner != PlayersList::instance()->getLocalPlayer())
    {
        qWarning("Can't change this private chat locally.");
        return;
    }
    p_set(name, set, true);
}

void PrivateChat::p_set(const QString & name, QSet<Player *> set, bool thenUpdate)
{
    set.insert(m_owner);

    if (thenUpdate && !PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        m_set.subtract(set);
        if (m_set.size() > 0)
            sendDel();
    }

    m_name = name;
    emit changedName(m_name);
    m_set  = set;
    if (thenUpdate)
        sendUpdate();
    emit changedMembers();
}
