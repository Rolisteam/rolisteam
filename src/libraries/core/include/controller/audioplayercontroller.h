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
#include <QStringListModel>
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
    Q_PROPERTY(uint volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(bool localIsGm READ localIsGm WRITE setLocalIsGm NOTIFY localIsGmChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool muted READ muted NOTIFY mutedChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(quint64 time READ time NOTIFY timeChanged)
    Q_PROPERTY(QStringListModel* directories READ directories CONSTANT)
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
    quint64 time() const;
    State state() const;
    QString text() const;
    uint volume() const;
    bool localIsGm() const;

    QStringListModel* directories() const;

public slots:
    void setMedia(const QModelIndex& index);
    void play();
    void stop();
    void pause();
    void next();
    void setPlayingMode(AudioPlayerController::PlayingMode mode);
    void loadPlayList(const QString& path);
    void setVolume(uint volume);
    void exportList(const QString& path);
    void mute();
    void clear();
    void removeSong(const QModelIndexList& index);
    void addSong(const QList<QUrl>& path);
    void setLocalIsGm(bool b);
    void setVisible(bool b);
    void setTime(quint64 time);
    void addMusicModel();

    void addDirectory(const QString& dirPath);
    void removeDirectory(int index);
    void moveDirectory(int index, bool up);

    // networkfunction
    void nwNewSong(const QString& string, qint64 time);

private:
    void updatePref();
signals:
    void modeChanged();
    void volumeChanged(int);
    void localIsGmChanged();
    void errorChanged(const QString& error);
    void visibleChanged(bool b);
    void mutedChanged();
    void textChanged();
    void stateChanged(AudioPlayerController::State state);
    void timeChanged(qint64);
    void positionChanged(qint64);
    void durationChanged(qint64 dura);
    void startPlayingSong(QString name, qint64 time);
    void stopPlaying();

private:
    std::unique_ptr<MusicModel> m_model;
    std::unique_ptr<QStringListModel> m_directories;
    int m_id;
    PlayingMode m_mode= NEXT;
    QPointer<PreferencesManager> m_pref;
    QMediaPlayer m_player;
    QAudioOutput m_audioOutput;
    QString m_text;
    bool m_localIsGM= false;
    bool m_visible= true;
    QString m_prefkey;
};

#endif // AUDIOPLAYERCONTROLLER_H
