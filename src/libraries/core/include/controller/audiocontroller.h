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
#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QObject>
#include <memory>

#include "preferences/preferencesmanager.h"
#include "audioplayercontroller.h"
#include "data/campaignmanager.h"
#include "updater/controller/audioplayerupdater.h"
#include <core_global.h>

class CORE_EXPORT AudioController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AudioPlayerController* firstController READ firstController CONSTANT)
    Q_PROPERTY(AudioPlayerController* secondController READ secondController CONSTANT)
    Q_PROPERTY(AudioPlayerController* thirdController READ thirdController CONSTANT)
    Q_PROPERTY(bool localIsGM READ localIsGM WRITE setLocalIsGM NOTIFY localisGMChanged)
public:
    enum AudioPlayerId
    {
        First,
        Second,
        Third
    };
    explicit AudioController(campaign::CampaignManager* manager, PreferencesManager* pref, QObject* parent= nullptr);

    AudioPlayerController* firstController() const;
    AudioPlayerController* secondController() const;
    AudioPlayerController* thirdController() const;

    bool localIsGM() const;

signals:
    void localisGMChanged();

public slots:
    void setLocalIsGM(bool m);

private:
    std::unique_ptr<AudioPlayerController> m_firstCtrl;
    std::unique_ptr<AudioPlayerController> m_secondCtrl;
    std::unique_ptr<AudioPlayerController> m_thirdCtrl;
    std::unique_ptr<AudioPlayerUpdater> m_updater;
};

#endif // AUDIOCONTROLLER_H
