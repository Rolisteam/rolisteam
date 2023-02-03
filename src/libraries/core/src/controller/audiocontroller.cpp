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
#include "controller/audiocontroller.h"

AudioController::AudioController(campaign::CampaignManager* campaign, PreferencesManager* pref, QObject* parent)
    : QObject(parent)
    , m_firstCtrl(new AudioPlayerController(First, Core::preferences::KEY_DIRECTORY_AP1, pref))
    , m_secondCtrl(new AudioPlayerController(Second, Core::preferences::KEY_DIRECTORY_AP2, pref))
    , m_thirdCtrl(new AudioPlayerController(Third, Core::preferences::KEY_DIRECTORY_AP3, pref))
    , m_updater(new AudioPlayerUpdater(campaign, this))
{
    connect(m_firstCtrl.get(), &AudioPlayerController::localIsGmChanged, this, &AudioController::localisGMChanged);
}

AudioPlayerController* AudioController::firstController() const
{
    return m_firstCtrl.get();
}
AudioPlayerController* AudioController::secondController() const
{
    return m_secondCtrl.get();
}
AudioPlayerController* AudioController::thirdController() const
{
    return m_thirdCtrl.get();
}

bool AudioController::localIsGM() const
{
    return m_firstCtrl->localIsGm();
}

void AudioController::setLocalIsGM(bool m)
{
    m_firstCtrl->setLocalIsGm(m);
    m_secondCtrl->setLocalIsGm(m);
    m_thirdCtrl->setLocalIsGm(m);
    m_updater->setLocalIsGM(m);
}
