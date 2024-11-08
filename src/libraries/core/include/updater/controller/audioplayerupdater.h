/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#ifndef AUDIOPLAYERUPDATER_H
#define AUDIOPLAYERUPDATER_H

#include <QObject>
#include <QPointer>
#include <memory>

#include "network/networkreceiver.h"
#include <core_global.h>
class AudioController;
namespace campaign
{
class CampaignManager;
}

class CORE_EXPORT AudioPlayerUpdater : public QObject, public NetWorkReceiver
{
    Q_OBJECT
public:
    explicit AudioPlayerUpdater(campaign::CampaignManager* campaign, AudioController* controller,
                                QObject* parent= nullptr);

    void setLocalIsGM(bool m);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);

    void saveStates();

signals:
    void savePlayers();

private:
    void initSignalForGM();

private:
    QPointer<AudioController> m_ctrl;
    QPointer<campaign::CampaignManager> m_campaign;
    bool m_localIsGM= true;
};

#endif // AUDIOPLAYERUPDATER_H
