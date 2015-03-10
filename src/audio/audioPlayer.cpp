/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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


#include <QToolButton>
#include <QFileDialog>
#include <QPushButton>
#include <QMenu>
#include <QContextMenuEvent>
#include <QListView>
#include <QDebug>



#include "ClientServeur.h"
#include "Liaison.h"
#include "variablesGlobales.h"
#include "types.h"
#include "network/networkmessagewriter.h"




AudioPlayer * AudioPlayer::m_singleton = NULL;


AudioPlayer::AudioPlayer(QWidget *parent)
    : QDockWidget(parent)//,m_currentSource(NULL)
{
    m_preferences = PreferencesManager::getInstance();
    setObjectName("MusicPlayer");
    setupUi();
    setWidget(m_mainWidget);
}

AudioPlayer*  AudioPlayer::getInstance(QWidget *parent)
{
    if(m_singleton==NULL)
    {
        m_singleton = new AudioPlayer(parent);
    }
    return m_singleton;
}
void AudioPlayer::contextMenuEvent(QContextMenuEvent* ev)
{
    QMenu menu;
    if(!PreferencesManager::getInstance()->value("isPlayer",true).toBool())
    {
        foreach(PlayerWidget* tmp,m_players)
        {
            if((tmp->geometry().contains(ev->pos(),true))&&(tmp->isVisible()))
            {
                tmp->addActionsIntoMenu(&menu);
                menu.addSeparator();
            }
        }
    }
    foreach(QAction* act,m_playerActionsList)
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
    m_mainWidget = new QWidget();

    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    for(int i = 0;i < 3; ++i)
    {
        PlayerWidget*  playerWidget = new PlayerWidget(i,this);
        connect(playerWidget,SIGNAL(newSongPlayed(int,QString)),this,SLOT(onePlayerHasNewSong(int,QString)));
        connect(playerWidget,SIGNAL(playerIsPaused(int)),this,SLOT(onePlayerIsPaused(int)));
        connect(playerWidget,SIGNAL(playerStopped(int)),this,SLOT(onePlayerHasStopped(int)));
        connect(playerWidget,SIGNAL(playerIsPlaying(int,quint64)),this,SLOT(onePlayerPlays(int,quint64)));
        connect(playerWidget,SIGNAL(playerPositionChanged(int,quint64)),this,SLOT(onePlayerHasChangedPosition(int,quint64)));

        m_players.append(playerWidget);
        QAction* act = new QAction(tr("Show/hide Player %1").arg(i),this);
        act->setCheckable(true);
        act->setChecked(m_preferences->value(QString("music_player_%1_status").arg(i),true).toBool());
        connect(act,SIGNAL(toggled(bool)),this,SLOT(showMusicPlayer(bool)));
        m_playerActionsList.append(act);
        m_mainLayout->addWidget(m_players[i]);
    }

    m_mainWidget->setLayout(m_mainLayout);

}
void AudioPlayer::showMusicPlayer(bool status)
{
    QAction* act = qobject_cast<QAction*>(sender());

    if(NULL!=act)
    {
        int i = m_playerActionsList.indexOf(act);

        if(-1!=1)
        {
            PlayerWidget* tmp = m_players[i];
            tmp->setVisible(status);
            m_preferences->registerValue(QString("music_player_%1_status").arg(i),status);
        }
    }
}
void AudioPlayer::updateUi()
{
    foreach(PlayerWidget* tmp,m_players)
    {
        tmp->updateUi();
    }
    for(int i = 0; i< m_players.size(); ++i)
    {
        m_playerActionsList[i]->setChecked(m_preferences->value(QString("music_player_%1_status").arg(i),true).toBool());
    }
    if(m_preferences->value("isPlayer",false).toBool())
    {
        m_mainLayout->addStretch(1);
    }
//    else
//    {

//    }
}
void AudioPlayer::onePlayerHasStopped(int id)
{
    if(!m_preferences->value("isPlayer",false).toBool())
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::StopSong);
        message.uint8(id);
        message.sendAll();
    }
}

void AudioPlayer::onePlayerIsPaused(int id)
{
    qDebug() << "audio player onePlayerIsPaused"<< id;
    if(!m_preferences->value("isPlayer",false).toBool())
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::PauseSong);
        message.uint8(id);
        message.sendAll();
    }
}

void AudioPlayer::onePlayerPlays(int id,quint64 pos)
{
    qDebug() << "audio player onePlayerPlays"<< id;
    if(!m_preferences->value("isPlayer",false).toBool())
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::PlaySong);
        message.uint8(id);
        message.uint64(pos);
        message.sendAll();
    }
}


void AudioPlayer::onePlayerHasNewSong(int id,QString str)
{
    qDebug() << "audio player onePlayerHasNewSong"<< id;
    if(!m_preferences->value("isPlayer",false).toBool())
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::NewSong);
        message.uint8(id);
        message.string32(str);
        message.sendAll();
    }
}

void AudioPlayer::onePlayerHasChangedPosition(int id,quint64 pos)
{
    qDebug() << "audio player onePlayerHasChangedPosition"<< id;
    if(!m_preferences->value("isPlayer",false).toBool())
    {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::ChangePositionSong);
        message.uint8(id);
        message.uint64(pos);
        message.sendAll();
    }
}




QString AudioPlayer::getDirectoryKey()
{
    QString key;
    if (PreferencesManager::getInstance()->value("isPlayer",true).toBool())
    {
        key="MusicDirectoryPlayer";
    }
    else
    {
        key="MusicDirectoryGM";
    }
    return key;
}

void AudioPlayer::pChangeDirectory()
{

    QString key = getDirectoryKey();


    QString tmp = QFileDialog::getExistingDirectory(0 , tr("Select the songs directory"), m_preferences->value(key,QDir::homePath()).toString(),
                                                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    m_preferences->registerValue(key,tmp);

}

void AudioPlayer::processMessage(NetworkMessageReader* msg)
{
    qDebug() << "network debug";
    int id = msg->uint8();
    NetMsg::Action action = msg->action();
    switch (action) {
    case NetMsg::PlaySong:
        qDebug() << "playSong";
        m_players[id]->playSong(msg->uint64());
        break;
    case NetMsg::PauseSong:
        qDebug() << "pauseSong";
        m_players[id]->pause();
        break;
    case NetMsg::ChangePositionSong:
    {
        qDebug() << "change position";
        int pos= msg->uint64();
        m_players[id]->setPositionAt(pos);
    }
        break;
    case NetMsg::StopSong:
        qDebug() << "stop";
        m_players[id]->stop();
        break;
    case NetMsg::NewSong:
    {
        qDebug() << "new";
        QString file = msg->string32();
        m_players[id]->setSourceSong(file);
    }
        break;
    default:
        break;
    }
}
