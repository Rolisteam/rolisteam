/***************************************************************************
 *     Copyright (C) 2010 by Joseph Boudou                                 *
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


#include "Features.h"

#include "datareader.h"
#include "persons.h"
#include "playersList.h"
#include "types.h"


/******************
 * Local Features *
 ******************/

#define LENGTH(X) (sizeof X / sizeof X[0])

static struct {
    QString name;
    quint8  version;
} localFeatures[] = {
    {QString("Emote"), 0}
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

void addFeature(DataReader & data)
{
    QString uuid   = data.string8();
    QString name   = data.string8();
    quint8 version = data.uint8();

    Player * player = PlayersList::instance().getPlayer(uuid);
    if (player == NULL)
    {
        qWarning("Feature %s for unknown player %s", qPrintable(name), qPrintable(uuid));
        return;
    }

    player->setFeature(name, version);
}

/************************
 * SendFeaturesIterator *
 ************************/

SendFeaturesIterator::SendFeaturesIterator()
    : QMapIterator<QString, quint8>(QMap<QString, quint8>()), m_player(NULL), m_message(parametres, AddFeatureAction)
{
}

SendFeaturesIterator::SendFeaturesIterator(const Player & player)
    : QMapIterator<QString, quint8>(player.m_features), m_player(&player), m_message(parametres, AddFeatureAction)
{
}

SendFeaturesIterator & SendFeaturesIterator::operator=(const Player * player)
{
    if (player != NULL)
        QMapIterator<QString, quint8>::operator=(player->m_features);

    m_player = player;
    return *this;
}

SendFeaturesIterator::~SendFeaturesIterator()
{
}

DataWriter & SendFeaturesIterator::message()
{
    m_message.reset();
    qDebug("zorglub");
    if (m_player != NULL)
    {
        qDebug("Prepared feature %s -> %s (%d)", qPrintable(m_player->uuid()), qPrintable(key()), value());
        m_message.string8(m_player->uuid());
        m_message.string8(key());
        m_message.uint8(value());
    }
    return m_message;
}
