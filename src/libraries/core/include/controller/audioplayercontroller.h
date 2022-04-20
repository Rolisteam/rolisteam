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
#ifndef AUDIOPLAYERCONTROLLER_H
#define AUDIOPLAYERCONTROLLER_H

#include "model/musicmodel.h"
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QModelIndexList>
#include <QObject>
#include <QPointer>
#include <core_global.h>
#include <memory>

class PreferencesManager;
class CORE_EXPORT AudioPlayerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(PlayingMode mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(MusicModel* model READ model CONSTANT)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool localIsGm READ localIsGm WRITE setLocalIsGm NOTIFY localIsGmChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool muted READ muted NOTIFY mutedChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(qint64 time READ time WRITE setTime NOTIFY timeChanged)
public:
    enum PlayingMode
    {
        LOOP,
        UNIQUE,
        NEXT,
        SHUFFLE
    };
    Q_ENUM(PlayingMode)
    enum State
    {
        StoppedState,
        PlayingState,
        PausedState,
        ErrorState
    };
    Q_ENUM(State)
    explicit AudioPlayerController(int id, const QString& key, PreferencesManager* pref, QObject* parent= nullptr);
    virtual ~AudioPlayerController();

    int id() const;
    PlayingMode mode() const;
    MusicModel* model() const;
    QString error() const;
    bool visible() const;
    bool muted() const;
    qint64 time() const;
    State state() const;
    QString text() const;
    int volume() const;
    bool localIsGm() const;
    QStringList directoriesList() const;

public slots:
    void setMedia(const QModelIndex& index);
    void play();
    void stop();
    void pause();
    void next();
    void setPlayingMode(AudioPlayerController::PlayingMode mode);
    void loadPlayList(const QString& path);
    void setVolume(int volume);
    void exportList(const QString& path);
    void mute();
    void clear();
    void removeSong(const QModelIndexList& index);
    void addSong(const QList<QUrl>& path);
    void setLocalIsGm(bool b);
    void setVisible(bool b);
    void setTime(int time);
    void addMusicModel();

    // networkfunction
    void nwNewSong(const QString& string, qint64 time);

signals:
    void modeChanged();
    void volumeChanged(int);
    void localIsGmChanged();
    void errorChanged();
    void visibleChanged(bool b);
    void mutedChanged();
    void textChanged();
    void stateChanged(AudioPlayerController::State state);
    void timeChanged(qint64);
    void durationChanged(qint64 dura);
    void startPlayingSong(QString name, qint64 time);

private:
    int m_id;
    PlayingMode m_mode= NEXT;
    std::unique_ptr<MusicModel> m_model;
    PreferencesManager* m_pref= nullptr;
    QMediaPlayer m_player;
    QAudioOutput m_audioOutput;
    QString m_text;
    bool m_localIsGM= false;
    bool m_visible= true;
    QString m_prefkey;
    // int m_volume{0};
};

#endif // AUDIOPLAYERCONTROLLER_H
