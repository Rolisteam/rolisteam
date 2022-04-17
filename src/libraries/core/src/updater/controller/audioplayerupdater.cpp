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
#include "updater/controller/audioplayerupdater.h"

#include <array>

#include "controller/audiocontroller.h"
#include "controller/audioplayercontroller.h"

#include "data/campaign.h"
#include "model/musicmodel.h"
#include "network/receiveevent.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

NetMsg::Action state2Action(AudioPlayerController::State state)
{
    NetMsg::Action netAction= NetMsg::ChangePositionSong;
    switch(state)
    {
    case AudioPlayerController::PlayingState:
        netAction= NetMsg::PlaySong;
        break;
    case AudioPlayerController::StoppedState:
        netAction= NetMsg::StopSong;
        break;
    case AudioPlayerController::PausedState:
        netAction= NetMsg::PauseSong;
        break;
    default:
        break;
    }
    return netAction;
}

AudioPlayerUpdater::AudioPlayerUpdater(campaign::CampaignManager* campaign, AudioController* controller,
                                       QObject* parent)
    : QObject(parent), m_ctrl(controller), m_campaign(campaign)
{
}

void AudioPlayerUpdater::setLocalIsGM(bool m)
{
    m_localIsGM= m;

    if(m_localIsGM)
    {
        initSignalForGM();
        ReceiveEvent::removeNetworkReceiver(NetMsg::MusicCategory, this);
    }
    else
        ReceiveEvent::registerNetworkReceiver(NetMsg::MusicCategory, this);
}

void AudioPlayerUpdater::initSignalForGM()
{
    auto ctrl1= m_ctrl->firstController();
    auto ctrl2= m_ctrl->secondController();
    auto ctrl3= m_ctrl->thirdController();

    auto model1= ctrl1->model();
    auto model2= ctrl2->model();
    auto model3= ctrl3->model();

    auto func1= [ctrl1, this]()
    {
        if(!m_localIsGM)
            return;
        IOHelper::writeJsonObjectIntoFile(
            m_campaign->placeDirectory(campaign::Campaign::Place::FIRST_AUDIO_PLAYER_FILE),
            IOHelper::saveAudioPlayerController(ctrl1));
    };
    auto func2= [ctrl2, this]()
    {
        if(!m_localIsGM)
            return;
        IOHelper::writeJsonObjectIntoFile(
            m_campaign->placeDirectory(campaign::Campaign::Place::SECOND_AUDIO_PLAYER_FILE),
            IOHelper::saveAudioPlayerController(ctrl2));
    };
    auto func3= [ctrl3, this]()
    {
        if(!m_localIsGM)
            return;
        IOHelper::writeJsonObjectIntoFile(
            m_campaign->placeDirectory(campaign::Campaign::Place::THIRD_AUDIO_PLAYER_FILE),
            IOHelper::saveAudioPlayerController(ctrl3));
    };

    connect(ctrl1, &AudioPlayerController::volumeChanged, this, func1);
    connect(ctrl1, &AudioPlayerController::visibleChanged, this, func1);
    connect(ctrl1, &AudioPlayerController::modeChanged, this, func1);
    connect(model1, &MusicModel::rowsInserted, this, func1);
    connect(model1, &MusicModel::rowsRemoved, this, func1);
    connect(model1, &MusicModel::modelReset, this, func1);

    connect(ctrl2, &AudioPlayerController::volumeChanged, this, func2);
    connect(ctrl2, &AudioPlayerController::visibleChanged, this, func2);
    connect(ctrl2, &AudioPlayerController::modeChanged, this, func2);
    connect(model2, &MusicModel::rowsInserted, this, func2);
    connect(model2, &MusicModel::rowsRemoved, this, func2);
    connect(model2, &MusicModel::modelReset, this, func2);

    connect(ctrl3, &AudioPlayerController::volumeChanged, this, func3);
    connect(ctrl3, &AudioPlayerController::visibleChanged, this, func3);
    connect(ctrl3, &AudioPlayerController::modeChanged, this, func3);
    connect(model3, &MusicModel::rowsInserted, this, func3);
    connect(model3, &MusicModel::rowsRemoved, this, func3);
    connect(model3, &MusicModel::modelReset, this, func3);

    connect(m_campaign, &campaign::CampaignManager::campaignLoaded, this,
            [ctrl1, ctrl2, ctrl3, this]()
            {
                qDebug() << "campaignLOADED" << m_localIsGM;
                if(!m_localIsGM)
                    return;
                bool ok;
                IOHelper::fetchAudioPlayerController(
                    ctrl1, IOHelper::loadJsonFileIntoObject(
                               m_campaign->placeDirectory(campaign::Campaign::Place::FIRST_AUDIO_PLAYER_FILE), ok));

                IOHelper::fetchAudioPlayerController(
                    ctrl2, IOHelper::loadJsonFileIntoObject(
                               m_campaign->placeDirectory(campaign::Campaign::Place::SECOND_AUDIO_PLAYER_FILE), ok));

                IOHelper::fetchAudioPlayerController(
                    ctrl3, IOHelper::loadJsonFileIntoObject(
                               m_campaign->placeDirectory(campaign::Campaign::Place::THIRD_AUDIO_PLAYER_FILE), ok));
            });

    // Network
    // play new song
    connect(ctrl1, &AudioPlayerController::startPlayingSong, this,
            [](const QString& song, qint64 time)
            { MessageHelper::sendOffPlaySong(song, time, AudioController::First); });
    connect(ctrl2, &AudioPlayerController::startPlayingSong, this,
            [](const QString& song, qint64 time)
            { MessageHelper::sendOffPlaySong(song, time, AudioController::Second); });
    connect(ctrl2, &AudioPlayerController::startPlayingSong, this,
            [](const QString& song, qint64 time)
            { MessageHelper::sendOffPlaySong(song, time, AudioController::Third); });

    // play/stop/pause
    connect(ctrl1, &AudioPlayerController::stateChanged, this,
            [](AudioPlayerController::State state)
            {
                auto netAction= state2Action(state);
                qDebug() << "first: state change" << state;
                if(netAction == NetMsg::ChangePositionSong)
                    return;
                MessageHelper::sendOffMusicPlayerOrder(netAction, AudioController::First);
            });
    connect(ctrl2, &AudioPlayerController::stateChanged, this,
            [](AudioPlayerController::State state)
            {
                auto netAction= state2Action(state);
                qDebug() << "second: state change" << state;
                if(netAction == NetMsg::ChangePositionSong)
                    return;
                MessageHelper::sendOffMusicPlayerOrder(netAction, AudioController::Second);
            });
    connect(ctrl2, &AudioPlayerController::stateChanged, this,
            [](AudioPlayerController::State state)
            {
                auto netAction= state2Action(state);
                qDebug() << "third: state change" << state;
                if(netAction == NetMsg::ChangePositionSong)
                    return;
                MessageHelper::sendOffMusicPlayerOrder(netAction, AudioController::Third);
            });
}

NetWorkReceiver::SendType AudioPlayerUpdater::processMessage(NetworkMessageReader* msg)
{
    qDebug() << "network message received" << msg;
    int id= msg->uint8();
    auto res= NetWorkReceiver::NONE;

    std::array<AudioPlayerController*, 3> array(
        {m_ctrl->firstController(), m_ctrl->secondController(), m_ctrl->thirdController()});

    if(id >= 3 || id < 0)
        return res;

    auto ctrl= array[id];

    NetMsg::Action action= msg->action();
    switch(action)
    {
    case NetMsg::PlaySong:
        ctrl->setTime(msg->int64());
        ctrl->play();
        break;
    case NetMsg::PauseSong:
        ctrl->pause();
        break;
    case NetMsg::ChangePositionSong:
        ctrl->setTime(msg->int64());
        break;
    case NetMsg::StopSong:
        ctrl->stop();
        break;
    case NetMsg::NewSong:
    {
        auto title= msg->string32();
        auto time= msg->int64();
        ctrl->nwNewSong(title, time);
    }
    break;
    default:
        break;
    }
    return NetWorkReceiver::AllExceptSender;
}
