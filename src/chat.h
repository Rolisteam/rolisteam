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


#ifndef CHAT_H
#define CHAT_H

#include <QSet>
#include <QString>

class Liaison;
class NetworkMessage;
class Player;
class ReceiveEvent;

class AbstractChat : public QObject
{
    Q_OBJECT

    public:
        virtual QString identifier() const =0;
        virtual QString name() const =0;
        virtual bool belongsToLocalPlayer() const;
        virtual bool belongsTo(Player * player) const =0;
        virtual void sendThem(NetworkMessage & message, Liaison * but = NULL) const =0;
        virtual bool everyPlayerHasFeature(const QString & feature, quint8 version = 0) const =0;

    signals:
        void changedName();
};

class PublicChat : public AbstractChat
{
    Q_OBJECT

    public:
        PublicChat();
        ~PublicChat();

        QString identifier() const;
        QString name() const;
        bool belongsTo(Player * player) const;
        void sendThem(NetworkMessage & message, Liaison * but = NULL) const;
        bool everyPlayerHasFeature(const QString & feature, quint8 version = 0) const;
};

class PlayerChat : public AbstractChat
{
    Q_OBJECT

    public:
        PlayerChat(Player * player);
        ~PlayerChat();

        QString identifier() const;
        QString name() const;
        bool belongsTo(Player * player) const;
        void sendThem(NetworkMessage & message, Liaison * but = NULL) const;
        bool everyPlayerHasFeature(const QString & feature, quint8 version = 0) const;

    private:
        Player * m_player;
};

class PrivateChat : public AbstractChat
{
    Q_OBJECT

    public:
        /**
         * @brief Create a chat owned by the local player
         */
        PrivateChat(const QString & name);

        /**
         * @brief Create a chat from the network.
         * If something went wrong, the identifier will be invalid.
         */
        PrivateChat(ReceiveEvent & event);

        ~PrivateChat();

        QString identifier() const;
        QString name() const;
        bool belongsToLocalPlayer() const;
        bool belongsTo(Player * player) const;
        void sendThem(NetworkMessage & message, Liaison * but = NULL) const;
        bool everyPlayerHasFeature(const QString & feature, quint8 version = 0) const;

        Player * owner() const;

        bool sameLink(Liaison * link);

        bool includeLocalPlayer() const;
        bool removePlayer(Player * player);
        QSet<Player *> players() const;

        /**
         * @brief Change players set silently.
         * Only works on chat owned by local player.
         * Does NOT send anything on net.
         */
        void setPlayers(const QSet<Player *> & set);

        void sendUpdate() const;
        void sendDel() const;

        /**
         * @brief Update the current chat to the other chat if they have the same owner.
         * Then send update.
         */
        void set(const PrivateChat & data, bool update = true);

        /**
         * @brief Update chat owned by local player.
         * Then send update.
         */
        void set(const QString & name, const QSet<Player *> & set);

    private:
        QString  m_uuid;
        QString  m_name;
        Player * m_owner;
        QSet<Player *> m_set;

        void p_set(const QString & name, QSet<Player *> set, bool update = true);
};

#endif
