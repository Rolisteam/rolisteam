/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                   *
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
#include "audioPlayer.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QFileDialog>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>

#include "network/networklink.h"
#include "network/networkmessagewriter.h"

AudioPlayer::AudioPlayer(QWidget* parent) : QDockWidget(parent) //,m_currentSource(nullptr)
{
    m_isGM= false;
    m_preferences= PreferencesManager::getInstance();

    setObjectName("MusicPlayer");
    setupUi();
    setWidget(m_mainWidget);
}

void AudioPlayer::setPlayerName(const QString& name)
{
    m_playerName= name;
}

void AudioPlayer::contextMenuEvent(QContextMenuEvent* ev)
{
    QMenu menu;
    if(m_isGM)
    {
        for(auto& tmp : m_players)
        {
            if((tmp->geometry().contains(ev->pos(), true)) && (tmp->isVisible()))
            {
                tmp->addActionsIntoMenu(&menu);
                menu.addSeparator();
            }
        }
    }
    for(auto& act : m_playerActionsList)
    {
        menu.addAction(act);
    }

    menu.exec(ev->globalPos());
    ev->accept();
}
AudioPlayer::~AudioPlayer()
{
    delete m_mainWidget;
}

void AudioPlayer::setupUi()
{
    setWindowTitle(tr("Background Music"));
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::AllDockWidgetFeatures);
    setMinimumWidth(255);
    m_mainWidget= new QWidget(this);

    m_mainLayout= new QVBoxLayout();
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    m_gmControlVolume= new QCheckBox(tr("Control player's volumes"));

    connect(m_gmControlVolume, &QCheckBox::clicked, this, [this](bool status) {
        m_preferences->registerValue(QStringLiteral("audio_player_gm_control_volume"), status);

        if(status)
            askForControl();
    });

    m_mainLayout->addWidget(m_gmControlVolume);

    m_volumeControlled= new QCheckBox(tr("Allow remote volume control"), this);

    connect(m_volumeControlled, &QCheckBox::toggled, this, [this](bool status) {
        m_preferences->registerValue(QStringLiteral("audio_player_remote_volume_control"), status);
    });

    m_mainLayout->addWidget(m_volumeControlled);
    m_globalVolume= new QSlider(this);
    m_globalVolume->setOrientation(Qt::Horizontal);
    m_globalVolume->setRange(0, 200);
    m_globalVolume->setValue(100);
    m_mainLayout->addWidget(m_globalVolume);

    m_volumeControlled->setVisible(false);
    m_volumeControlled->setChecked(false);
    m_globalVolume->setVisible(false);
    m_globalVolume->setEnabled(false);

    for(int i= 0; i < 3; ++i)
    {
        PlayerWidget* playerWidget= new PlayerWidget(i, this);
        connect(playerWidget, &PlayerWidget::newSongPlayed, this, &AudioPlayer::onePlayerHasNewSong);
        connect(playerWidget, &PlayerWidget::playerIsPaused, this, &AudioPlayer::onePlayerIsPaused);
        connect(playerWidget, &PlayerWidget::playerStopped, this, &AudioPlayer::onePlayerHasStopped);
        connect(playerWidget, &PlayerWidget::volumeChanged, this, &AudioPlayer::volumeChangedOnOneAudioPlayer);
        connect(playerWidget, &PlayerWidget::playerIsPlaying, this, &AudioPlayer::onePlayerPlays);
        connect(playerWidget, &PlayerWidget::playerPositionChanged, this, &AudioPlayer::onePlayerHasChangedPosition);

        m_players.append(playerWidget);
        QAction* act= new QAction(tr("Show/hide Player %1").arg(i + 1), this);
        act->setCheckable(true);
        connect(act, &QAction::triggered, this, &AudioPlayer::showMusicPlayer);
        m_mainLayout->addWidget(m_players[i]);
        m_playerActionsList.append(act);
    }

    connect(m_volumeControlled, &QCheckBox::toggled, m_globalVolume, &QSlider::setEnabled);
    connect(m_volumeControlled, &QCheckBox::toggled, this, [this](bool checked) {
        m_volumeControlable= checked;
        std::for_each(m_players.begin(), m_players.end(),
                      [checked](PlayerWidget* player) { player->setControlledVolume(checked); });
    });
    connect(m_globalVolume, &QSlider::valueChanged, this, [this](int value) {
        auto f= static_cast<qreal>(value) / 100.0;
        std::for_each(m_players.begin(), m_players.end(), [f](PlayerWidget* player) { player->setVolumeFactor(f); });
    });
    m_mainWidget->setLayout(m_mainLayout);
}

void AudioPlayer::readSettings()
{
    int i= 0;
    for(auto& action : m_playerActionsList)
    {
        action->setChecked(m_preferences->value(QString("music_player_%1_status").arg(i), true).toBool());
        m_players[i]->setVisible(action->isChecked());
        ++i;
    }
    m_volumeControlled->setChecked(
        m_preferences->value(QStringLiteral("audio_player_remote_volume_control"), false).toBool());
    m_gmControlVolume->setChecked(
        m_preferences->value(QStringLiteral("audio_player_gm_control_volume"), false).toBool());
}

void AudioPlayer::showMusicPlayer(bool status)
{
    QAction* act= qobject_cast<QAction*>(sender());

    if(nullptr == act)
        return;

    int i= m_playerActionsList.indexOf(act);

    if(-1 != i)
    {
        PlayerWidget* tmp= m_players[i];
        tmp->setVisible(status);
        m_preferences->registerValue(QString("music_player_%1_status").arg(i), status);
    }
}
void AudioPlayer::updateUi(bool isGM)
{
    m_isGM= isGM;
    for(auto& tmp : m_players)
    {
        tmp->updateUi(isGM);
    }
    int i= 0;
    for(auto& action : m_playerActionsList)
    {
        action->setChecked(m_preferences->value(QString("music_player_%1_status").arg(i), true).toBool());
        m_players[i]->setVisible(action->isChecked());
        ++i;
    }
    if(!isGM)
    {
        m_mainLayout->addStretch(1);
        m_volumeControlled->setVisible(true);
        m_globalVolume->setVisible(true);
        m_gmControlVolume->setVisible(false);
    }
    else
    {
        m_volumeControlled->setVisible(false);
        m_globalVolume->setVisible(false);
        m_gmControlVolume->setVisible(true);
    }
}
void AudioPlayer::onePlayerHasStopped(int id)
{
    if(m_isGM)
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::StopSong);
        message.uint8(static_cast<quint8>(id));
        message.sendToServer();
    }
}

void AudioPlayer::onePlayerIsPaused(int id)
{
    if(m_isGM)
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::PauseSong);
        message.uint8(static_cast<quint8>(id));
        message.sendToServer();
    }
}

void AudioPlayer::askForControl()
{
    if(!m_isGM)
        return;

    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::AskForControl);
    message.uint8(static_cast<quint8>(0));
    for(const auto& player : m_players)
    {
        message.int8(static_cast<qint8>(player->volume()));
    }
    message.sendToServer();
}

void AudioPlayer::volumeChangedOnOneAudioPlayer(int id, int volume)
{
    if(!m_isGM || !m_gmControlVolume->isChecked())
        return;

    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::VolumeChanged);
    message.uint8(static_cast<quint8>(id));
    message.uint8(static_cast<quint8>(volume));
    message.sendToServer();
}

void AudioPlayer::volumeControlError()
{
    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::ErrorVolumeControl);
    message.uint8(static_cast<quint8>(0));
    message.string32(m_playerName);
    message.sendToServer();
}

void AudioPlayer::onePlayerPlays(int id, quint64 pos)
{
    if(m_isGM)
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::PlaySong);
        message.uint8(static_cast<quint8>(id));
        message.uint64(pos);
        message.sendToServer();
    }
}

void AudioPlayer::onePlayerHasNewSong(int id, QString str)
{
    if(m_isGM)
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::NewSong);
        message.uint8(static_cast<quint8>(id));
        message.string32(str);
        message.sendToServer();
    }
}

void AudioPlayer::onePlayerHasChangedPosition(int id, quint64 pos)
{
    if(m_isGM)
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::ChangePositionSong);
        message.uint8(static_cast<quint8>(id));
        message.uint64(pos);
        message.sendToServer();
    }
}

NetWorkReceiver::SendType AudioPlayer::processMessage(NetworkMessageReader* msg)
{
    int id= msg->uint8();
    if(id >= m_players.size() && id < 0)
        return NetWorkReceiver::NONE;

    NetMsg::Action action= msg->action();
    switch(action)
    {
    case NetMsg::PlaySong:
        m_players[id]->playSong(msg->uint64());
        break;
    case NetMsg::PauseSong:
        m_players[id]->pause();
        break;
    case NetMsg::ChangePositionSong:
    {
        quint64 pos= msg->uint64();
        m_players[id]->setPositionAt(pos);
    }
    break;
    case NetMsg::StopSong:
        m_players[id]->stop();
        break;
    case NetMsg::NewSong:
    {
        QString file= msg->string32();
        m_players[id]->setSourceSong(file);
    }
    break;
    case NetMsg::VolumeChanged:
    {
        auto vol= msg->uint8();
        if(m_volumeControlable)
            m_players[id]->setVolume(vol);
    }
    break;
    case NetMsg::AskForControl:
    {

        if(!m_volumeControlable)
        {
            auto btn= QMessageBox::question(
                this, tr("GM asks for remote volume control"),
                tr("Your GM wants to control volume of each audio player. Do you accept this request ?"),
                QMessageBox::Yes, QMessageBox::No);
            if(btn == QMessageBox::Yes)
            {
                m_volumeControlled->setChecked(true);
                m_volumeControlable= true;
            }
            else
                volumeControlError();
        }
        if(m_volumeControlable)
        {
            for(auto player : m_players)
                player->setVolume(msg->int8());
        }
    }
    break;
    case NetMsg::ErrorVolumeControl:
    {
        auto playerName= msg->string32();
        emit errorMessage(
            tr("%1 did not accept your request for remote volume control.", "%1 is the player name").arg(playerName));
    }
    break;
    default:
        break;
    }
    return NetWorkReceiver::AllExceptSender;
}
void AudioPlayer::openSongList(QString str)
{
    if(!m_players.isEmpty())
    {
        m_players.at(0)->readM3uPlayList(str);
    }
}

void AudioPlayer::openSong(QString str)
{
    if(!m_players.isEmpty())
    {
        m_players.at(0)->addSongIntoModel(str);
    }
}
