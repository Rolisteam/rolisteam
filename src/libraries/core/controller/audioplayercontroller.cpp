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
#include "audioplayercontroller.h"

#include "core/model/musicmodel.h"
#include "core/worker/iohelper.h"
#include "core/worker/modelhelper.h"
#include "preferences/preferencesmanager.h"

#include <chrono>
#include <random>

int randomSong(int current, int count)
{
    static auto seed= std::chrono::high_resolution_clock::now().time_since_epoch().count();
    static std::mt19937 rng= std::mt19937(static_cast<unsigned long long>(seed));
    std::uniform_int_distribution<qint64> dist(0, count - 1);

    if(count < 0)
        return -1;
    if(count < 1)
        return 0;
    if(count < 2)
        return current == 1 ? 0 : 1;

    int res= current;
    while(res == current)
    {
        res= dist(rng);
    }
    qDebug() << res;
    return res;
}

AudioPlayerController::AudioPlayerController(int id, const QString& key, PreferencesManager* pref, QObject* parent)
    : QObject(parent), m_id(id), m_model(new MusicModel), m_pref(pref), m_prefkey(key)
{
    m_player.setAudioRole(QAudio::MusicRole);
    connect(&m_player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this,
            &AudioPlayerController::errorChanged);
    connect(&m_player, &QMediaPlayer::stateChanged, this, [this]() { emit stateChanged(state()); });
    connect(&m_player, &QMediaPlayer::mutedChanged, this, &AudioPlayerController::mutedChanged);
    connect(&m_player, &QMediaPlayer::durationChanged, this, &AudioPlayerController::durationChanged);
    connect(&m_player, &QMediaPlayer::volumeChanged, this, &AudioPlayerController::volumeChanged);
    connect(&m_player, &QMediaPlayer::positionChanged, this, &AudioPlayerController::timeChanged);

    connect(&m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        switch(status)
        {
        case QMediaPlayer::EndOfMedia:
            next();
            break;
        default:
            break;
        }
    });
}

AudioPlayerController::~AudioPlayerController()= default;

int AudioPlayerController::id() const
{
    return m_id;
}

AudioPlayerController::PlayingMode AudioPlayerController::mode() const
{
    return m_mode;
}

MusicModel* AudioPlayerController::model() const
{
    return m_model.get();
}

QString AudioPlayerController::error() const
{
    return m_player.errorString();
}

bool AudioPlayerController::visible() const
{
    return m_visible;
}

bool AudioPlayerController::muted() const
{
    return m_player.isMuted();
}

int AudioPlayerController::time() const
{
    return m_player.position();
}

AudioPlayerController::State AudioPlayerController::state() const
{
    AudioPlayerController::State state;
    switch(m_player.state())
    {
    case QMediaPlayer::StoppedState:
        state= AudioPlayerController::StoppedState;
        break;
    case QMediaPlayer::PausedState:
        state= AudioPlayerController::PausedState;
        break;
    case QMediaPlayer::PlayingState:
        state= AudioPlayerController::PlayingState;
        break;
    }

    if(m_player.error() != QMediaPlayer::NoError)
        state= AudioPlayerController::ErrorState;

    return state;
}

QString AudioPlayerController::text() const
{
    return m_text;
}

int AudioPlayerController::volume() const
{
    return m_player.volume();
}

bool AudioPlayerController::localIsGm() const
{
    return m_localIsGM;
}

void AudioPlayerController::setMedia(const QModelIndex& index)
{
    m_text= m_model->data(index, MusicModel::TITLE).toString();
    m_player.setMedia(m_model->data(index, MusicModel::URL).toUrl());
    m_model->setCurrentSong(index);
    emit textChanged();
}

void AudioPlayerController::play()
{
    m_player.play();
    auto url= m_player.media().request().url();
    emit startPlayingSong(url.isLocalFile() ? url.fileName() : url.toString(), m_player.position());
}

void AudioPlayerController::stop()
{
    m_player.stop();
}

void AudioPlayerController::pause()
{
    m_player.pause();
}

void AudioPlayerController::next()
{
    auto p= m_model->indexOfCurrent();
    auto start= true;
    switch(m_mode)
    {
    case LOOP:
        play();
        break;
    case UNIQUE:
        start= false;
        break;
    case NEXT:
        setMedia(m_model->index(p + 1, 0));
        play();
        break;
    case SHUFFLE:
        setMedia(m_model->index(randomSong(p, m_model->rowCount()), 0));
        play();
        break;
    }
    if(start)
    {
        auto url= m_player.media().request().url();
        emit startPlayingSong(url.isLocalFile() ? url.fileName() : url.toString(), m_player.position());
    }
}

void AudioPlayerController::setPlayingMode(PlayingMode mode)
{
    if(m_mode == mode)
        return;
    m_mode= mode;
    emit modeChanged();
}

void AudioPlayerController::loadPlayList(const QString& path)
{
    auto urls= IOHelper::readM3uPlayList(path);
    m_model->addSong(urls);
}

void AudioPlayerController::setVolume(int volume)
{
    m_player.setVolume(volume);
}

void AudioPlayerController::exportList(const QString& path)
{
    IOHelper::writePlaylist(path, m_model->urls());
}

void AudioPlayerController::mute()
{
    m_player.setMuted(!m_player.isMuted());
}

void AudioPlayerController::clear()
{
    m_model->removeAll();
}

void AudioPlayerController::removeSong(const QModelIndexList& index)
{
    m_model->removeSong(index);
}

void AudioPlayerController::addSong(const QList<QUrl>& path)
{
    m_model->addSong(path);
}

void AudioPlayerController::setLocalIsGm(bool b)
{
    if(b == m_localIsGM)
        return;
    m_localIsGM= b;
    emit localIsGmChanged();
}

void AudioPlayerController::setVisible(bool b)
{
    if(m_visible == b)
        return;
    m_visible= b;
    emit visibleChanged(b);
}

void AudioPlayerController::setTime(int time)
{
    m_player.setPosition(time);
}

void AudioPlayerController::addMusicModel()
{
    ModelHelper::fetchMusicModelWithTableTop(m_model.get());
}

QStringList AudioPlayerController::directoriesList() const
{
    if(!m_pref)
        return {};
    return m_pref->value(m_prefkey, {}).toStringList();
}

void AudioPlayerController::nwNewSong(const QString& name, qint64 time)
{
    if(localIsGm())
        return;
    QStringList list= directoriesList();

    auto url= IOHelper::findSong(name, list);
    qDebug() << "read from network" << url << name << time;

    m_text= url.fileName();
    m_player.setMedia(url);
    m_player.setPosition(time);
    m_player.play();
    emit textChanged();
}
