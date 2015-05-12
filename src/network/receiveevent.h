/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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


#ifndef RECEIVE_EVENT_H
#define RECEIVE_EVENT_H

#include <QEvent>
#include <QString>
#include <QMap>

#include "network/networkmessagereader.h"
#include "network/networkreceiver.h"

class NetworkLink;
/**
 * @brief The ReceiveEvent class is an QEvent posted to the appplication to notify network message arrival.
 */
class ReceiveEvent : public QEvent
{
    public:
		ReceiveEvent(const NetworkMessageHeader & header, const char * buffer, NetworkLink * link);
        ReceiveEvent(const ReceiveEvent & other);
        ~ReceiveEvent();

        static const int Type;
        /**
         * @brief postToReceiver
         */
        void postToReceiver();

        /**
         * @brief Post again this same event after a delay.
         */
        void repostLater() const;
        /**
         * @brief link
         * @return
         */
		NetworkLink * link() const;
        /**
         * @brief data
         * @return
         */
        NetworkMessageReader & data();

        /**
         * @brief hasReceiverFor
         * @param categorie
         * @param action
         * @return
         */
        static bool hasReceiverFor(quint8 categorie, quint8 action);
        /**
         * @brief registerReceiver
         * @param categorie
         * @param action
         * @param receiver
         */
        static void registerReceiver(NetMsg::Category categorie, NetMsg::Action action, QObject * receiver);

        /**
         * @brief hasNetworkReceiverFor
         * @param categorie
         * @return
         */
        static bool hasNetworkReceiverFor(NetMsg::Category categorie);
        /**
         * @brief getNetWorkReceiverFor
         * @param categorie
         * @return
         */
        static NetWorkReceiver* getNetWorkReceiverFor(NetMsg::Category categorie);
        /**
         * @brief registerNetworkReceiver
         * @param categorie
         * @param receiver
         */
        static void registerNetworkReceiver(NetMsg::Category categorie, NetWorkReceiver* receiver);

    private:
        NetworkMessageReader m_data;
		NetworkLink * m_link;
        quint8 m_repost;

        static QMap<quint16, QObject *> s_receiverMap;
        static QMap<NetMsg::Category, NetWorkReceiver *> ms_netWorkReceiverMap;
};
/**
 * @brief The DelayReceiveEvent class
 */
class DelayReceiveEvent : public QObject
{
    Q_OBJECT

    public:
        DelayReceiveEvent(const ReceiveEvent & event);
        ~DelayReceiveEvent();

    private:
        ReceiveEvent * m_event;

    private slots:
        void postEvent();
};

#endif
