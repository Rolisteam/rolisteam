/***************************************************************************
 *     Copyright (C) 2010 by Joseph Boudou                                 *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *     http://www.rolisteam.org/                                           *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include <QDebug>

#include "Features.h"

#include "network/networkmessagereader.h"
#include "data/person.h"
#include "data/player.h"
#include "userlist/playersList.h"
#include "network/receiveevent.h"


/******************
 * Local Features *
 ******************/
// clazy:skip
#define LENGTH(X) (sizeof X / sizeof X[0])

static struct {
    QString name;
    quint8  version;
} localFeatures[] = {
    {QString("Emote"), 0},
    {QString("MultiChat"), 0},
    {QString("RichTextChat"), 0},
    {QString("MapPermission"), 0}
};


/*************
 * Functions *
 *************/

void setLocalFeatures(Player & player)
{
    int nbFeatures = LENGTH(localFeatures);
    for (int i = 0; i < nbFeatures; i++)
    {
        player.setFeature(localFeatures[i].name, localFeatures[i].version);
    }
}

void addFeature(ReceiveEvent & event)
{
    NetworkMessageReader & data = event.data();
    QString uuid   = data.string8();
    QString name   = data.string8();
    quint8 version = data.uint8();

    Player * player = PlayersList::instance()->getPlayer(uuid);
    if (player == nullptr)
    {
        qWarning()<< QString("Feature %1 for unknown player %2").arg(name).arg(uuid);
        event.repostLater();
        return;
    }

    player->setFeature(name, version);
}

/************************
 * SendFeaturesIterator *
 ************************/

SendFeaturesIterator::SendFeaturesIterator()
    : QMapIterator<QString, quint8>(QMap<QString, quint8>()), m_player(nullptr), m_message(NetMsg::SetupCategory, NetMsg::AddFeatureAction)
{
}

SendFeaturesIterator::SendFeaturesIterator(const Player & player)
    : QMapIterator<QString, quint8>(player.m_features), m_player(&player), m_message(NetMsg::SetupCategory, NetMsg::AddFeatureAction)
{
}

SendFeaturesIterator & SendFeaturesIterator::operator=(const Player * player)
{
    if (player != nullptr)
        QMapIterator<QString, quint8>::operator=(player->m_features);

    m_player = player;
    return *this;
}

SendFeaturesIterator::~SendFeaturesIterator()
{
}

NetworkMessageWriter & SendFeaturesIterator::message()
{
    m_message.reset();
    if (m_player != nullptr)
    {
        m_message.string8(m_player->getUuid());
        m_message.string8(key());
        m_message.uint8(value());
    }
    return m_message;
}
