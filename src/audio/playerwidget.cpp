/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

#include "playerwidget.h"

#include "ui_audiowidget.h"
#include "network/networkmessage.h"
#define FACTOR_WAIT 4

PlayerWidget::PlayerWidget(int id, QWidget* parent)
    : QWidget(parent),m_id(id),m_ui(new Ui::AudioWidgetUI)
{
    m_preferences = PreferencesManager::getInstance();
    m_ui->setupUi(this);
    m_playingMode = NEXT;

    setupUi();
    m_model = new MusicModel(this);
    m_ui->m_songList->setDragEnabled(true);
    m_ui->m_songList->setDragDropMode(QAbstractItemView::DropOnly);
    m_ui->m_songList->setModel(m_model);
    m_ui->m_songList->setAcceptDrops(true);
    m_ui->m_songList->setDropIndicatorShown(true);

    updateUi(true);
}

void PlayerWidget::startMedia(QMediaContent* p,QString title,bool play)
{
    m_content = p;
    m_player.setMedia(*m_content);
    m_ui->m_timeSlider->setMinimum(0);
    if(title.isEmpty())
    {
        m_ui->m_label->setText(p->canonicalUrl().fileName());
    }
    else
    {
        m_ui->m_label->setText(title);
    }
    if(play)
    {
        m_player.play();
    }
}
void PlayerWidget::setDuration(qint64 duration)
{
    m_ui->m_timeSlider->setMaximum(duration);
}

void PlayerWidget::positionChanged(qint64 time)
{
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);

     if((!m_preferences->value("isPlayer",false).toBool()) && ((time>m_time+(FACTOR_WAIT*m_player.notifyInterval()))||(time<m_time)))
     {
          emit playerPositionChanged(m_id,time);
     }
     m_time = time;
     m_ui->m_timeSlider->setValue(time);
     m_ui->m_timerDisplay->display(displayTime.toString("mm:ss"));

}
void PlayerWidget::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status)
    {
    case QMediaPlayer::EndOfMedia:
        findNext();
        break;
    default:
        break;
    }
}
void PlayerWidget::findNext()
{
    modeHasBeenChanged();
    switch (m_playingMode)
    {
    case NEXT:
    {
        QModelIndex index = m_model->getCurrentSong();
        int next=index.row()+1;
        QModelIndex newIndex = index.sibling(next,0);
        if(newIndex.isValid())
        {
            startMediaByModelIndex(newIndex);
        }

    }
        break;
    case UNIQUE:
        //nothing
        break;
    case LOOP:
        m_player.play();
        break;
    }
}

void PlayerWidget::setupUi()
{
    m_ui->m_timeSlider->setMinimum(0);
    m_ui->m_volumeSlider->setRange(0,100);
    // **************** CREATE ACTIONS ********************************
    m_playAct = new QAction(style()->standardIcon(QStyle::SP_MediaPlay),tr("Play"),this);
    m_pauseAct = new QAction(style()->standardIcon(QStyle::SP_MediaPause),tr("Pause"),this);
    m_stopAct = new QAction(style()->standardIcon(QStyle::SP_MediaStop),tr("Stop"),this);
    m_uniqueAct = new QAction(QIcon(":/resources/icons/playunique.png"),tr("Next"),this);
    m_uniqueAct->setShortcut(QKeySequence("Ctrl+U"));
    m_uniqueAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_uniqueAct->setCheckable(true);
    m_repeatAct = new QAction(QIcon(":/resources/icons/playloop.png"),tr("Previous"),this);
    m_repeatAct->setCheckable(true);
    m_repeatAct->setShortcut(QKeySequence("Ctrl+R"));
    m_repeatAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_changeDirectoryAct = new QAction(style()->standardIcon(QStyle::SP_DirIcon),tr("Open Directory"),this);
    m_volumeMutedAct = new QAction(this);
    m_volumeMutedAct->setCheckable(true);
    m_volumeMutedAct->setShortcut(QKeySequence("Ctrl+M"));
    m_volumeMutedAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_loadTableTopAudioPlayListAct = new QAction(tr("load TableTopAudio.com playlist"),this);


    m_openPlayList= new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton),tr("Open Playlist"),this);
    m_openPlayList->setShortcut(QKeySequence("Ctrl+J"));
    m_openPlayList->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_savePlayList= new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton),tr("Save Playlist"),this);
    m_savePlayList->setShortcut(QKeySequence("Ctrl+E"));
    m_savePlayList->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_clearList= new QAction(style()->standardIcon(QStyle::SP_DialogResetButton),tr("Clear"),this);
    m_clearList->setShortcut(QKeySequence("Ctrl+Del"));
    m_clearList->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_addAction 	= new QAction(QIcon(":/resources/icons/add.png"),tr("Add Songs"), this);
    m_addAction->setShortcut(QKeySequence("Ctrl+A"));
    m_addAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_addStreamAction = new QAction(tr("Open Stream"),this);

    m_deleteAction	= new QAction(QIcon(":/resources/icons/remove.png"),tr("Remove Song"), this);
    m_deleteAction->setShortcut(QKeySequence("Del"));
    m_deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);


    m_ui->m_volumeSlider->setValue(m_preferences->value(QString("volume_player_%1").arg(m_id),50).toInt());
    m_audioFileFilter = m_preferences->value("AudioFileFilter","*.wav *.mp2 *.mp3 *.ogg *.flac").toString();

    // **************** Add ACTIONS ********************************

    addAction(m_uniqueAct);
    addAction(m_repeatAct);
    addAction(m_volumeMutedAct);
    addAction(m_openPlayList);
    addAction(m_savePlayList);
    addAction(m_clearList);
    addAction(m_deleteAction);
    addAction(m_addAction);
    addAction(m_addStreamAction);

    // **************** TOOLTIP ACTIONS ********************************
    m_addAction->setToolTip(tr("Add song to the list"));
    m_deleteAction->setToolTip(tr("Remove selected file"));



    // **************** Add Action In Buttons ********************************
    m_ui->m_deleteButton->setDefaultAction(m_deleteAction);
    m_ui->m_deleteButton->addAction(m_clearList);
    m_ui->m_savePlaylist->setDefaultAction(m_savePlayList);
    m_ui->m_addButton->setDefaultAction(m_addAction);
    m_ui->m_addButton->addAction(m_openPlayList);
    m_ui->m_addButton->addAction(m_loadTableTopAudioPlayListAct);
    m_ui->m_addButton->addAction(m_addStreamAction);

    m_ui->m_volumeMutedButton->setDefaultAction(m_volumeMutedAct);
    m_ui->m_playButton->setDefaultAction(m_playAct);
    m_ui->m_pauseButton->setDefaultAction(m_pauseAct);
    m_ui->m_stopButton->setDefaultAction(m_stopAct);
    m_ui->m_uniqueMode->setDefaultAction(m_uniqueAct);
    m_ui->m_repeatMode->setDefaultAction(m_repeatAct);
    m_ui->m_changeDirectory->setDefaultAction(m_changeDirectoryAct);
    

    // **************** CONNECT ********************************
    connect(m_addAction, SIGNAL(triggered(bool)), this, SLOT(addFiles()));
    connect(m_openPlayList, SIGNAL(triggered(bool)), this, SLOT(openPlayList()));
    connect(m_addStreamAction, SIGNAL(triggered(bool)), this, SLOT(openStream()));
    connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(removeFile()));
    connect(m_clearList,SIGNAL(triggered(bool)),this,SLOT(removeAll()));
    connect(m_ui->m_songList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(startMediaByModelIndex(QModelIndex)));
    updateIcon();
    connect(&m_player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
    connect(&m_player,SIGNAL(durationChanged(qint64)),this,SLOT(setDuration(qint64)));
   // connect(m_playAct,SIGNAL(triggered()),&m_player,SLOT(play()));
    connect(m_playAct,SIGNAL(triggered()),this,SLOT(playSelectedSong()));

    connect(m_stopAct,SIGNAL(triggered()),&m_player,SLOT(stop()));
    connect(m_pauseAct,SIGNAL(triggered()),&m_player,SLOT(pause()));
    connect(m_ui->m_timeSlider,SIGNAL(sliderMoved(int)),this,SLOT(setTime(int)));
    connect(m_ui->m_volumeSlider,SIGNAL(valueChanged(int)),&m_player,SLOT(setVolume(int)));
    connect(m_ui->m_volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(saveVolumeValue(int)));
    connect(&m_player,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(sourceChanged(QMediaContent)));
    connect(&m_player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(playerStatusChanged(QMediaPlayer::State)));
    connect(m_volumeMutedAct,SIGNAL(toggled(bool)),&m_player,SLOT(setMuted(bool)));
    connect(m_volumeMutedAct,SIGNAL(toggled(bool)),this,SLOT(updateIcon()));
    connect(m_changeDirectoryAct,SIGNAL(triggered()),this,SLOT(changeDirectory()));
    connect(&m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(&m_player,SIGNAL(error(QMediaPlayer::Error)),this,SLOT(errorOccurs(QMediaPlayer::Error)));
    connect(&m_player,SIGNAL(seekableChanged(bool)),this,SLOT(seekHasChanged(bool)));
    connect(m_ui->m_label,SIGNAL(textChanged(QString)),this,SLOT(labelTextChanged()));

    connect(m_repeatAct,SIGNAL(triggered()),this,SLOT(triggeredPlayingModeAction()));
    connect(m_uniqueAct,SIGNAL(triggered()),this,SLOT(triggeredPlayingModeAction()));

    connect(m_loadTableTopAudioPlayListAct,SIGNAL(triggered()),this,SLOT(loadPlayList()));
    connect(m_savePlayList,SIGNAL(triggered()),this,SLOT(savePlaylist()));


}
void PlayerWidget::startMediaByModelIndex(QModelIndex p)//double click
{

      startMedia(m_model->getMediaByModelIndex(p),m_model->data(p).toString());
      m_model->setCurrentSong(p);
          //  m_mediaObject->play();
}

void PlayerWidget::removeFile()
{
    QModelIndexList list = m_ui->m_songList->selectionModel()->selectedIndexes();
    m_model->removeSong(list);
}
void PlayerWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if((current.isValid())&&(m_player.mediaStatus() == QMediaPlayer::NoMedia))
    {
            startMedia(m_model->getMediaByModelIndex(current),current.data().toString(),false);
    }
}
void PlayerWidget::playSelectedSong()
{
    QModelIndex current = m_ui->m_songList->currentIndex();
    if((current.isValid())&&((m_player.mediaStatus() == QMediaPlayer::NoMedia)||(m_player.mediaStatus()==QMediaPlayer::EndOfMedia)||(m_player.state()==QMediaPlayer::StoppedState)))
    {
            startMedia(m_model->getMediaByModelIndex(current),current.data().toString());
    }
    else
    {
        m_player.play();
    }
}
void PlayerWidget::addSongIntoModel(QString str)
{
    QStringList songs;
    songs<< str;
    m_model->addSong(songs);
}

void PlayerWidget::addFiles()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Add song"), m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString(), tr("Audio files (%1)").arg(m_audioFileFilter));
    if (fileList.isEmpty())
        return;
    QFileInfo fileinfo(fileList[0]);
    m_preferences->registerValue("MusicDirectoryGM",fileinfo.absolutePath());
    m_model->addSong(fileList);
}
bool PlayerWidget::askToDeleteAll()
{
    if(m_model->rowCount()!=0)
    {
        if(QMessageBox::Ok == QMessageBox::warning(this,tr("Attention!"),tr("You are about to load an new playlist. All previously load file will be dropped."),QMessageBox::Ok, QMessageBox::Cancel))
        {
            m_model->removeAll();
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}
void PlayerWidget::openPlayList()
{
    if(askToDeleteAll())
    {
        QString filename = QFileDialog::getOpenFileName(this, tr("Open Play List"), m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString(), tr("PlayList (*.m3u)"));
        if (filename.isEmpty())
            return;
        QFileInfo fileinfo(filename);
        m_preferences->registerValue("MusicDirectoryGM",fileinfo.absolutePath());
        readM3uPlayList(filename);
    }

}
void PlayerWidget::openStream()
{
    bool a;
    QString value = QInputDialog::getText(this,tr("Open audio Stream"),tr("URL"),QLineEdit::Normal,QString(),&a);
    if(a)
    {
        QUrl url(value);
        if(url.isValid())
        {
            QStringList values;
            values << value;
            m_model->addSong(values);
        }
    }
}

void PlayerWidget::readM3uPlayList(QString filepath)
{
    QFile file(filepath);
    /// @todo make this job in thread.
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream read(&file);
        QString line;
        QStringList result;
        while(!read.atEnd())
        {
            line= read.readLine();
            if(!line.startsWith("#EXTINF",Qt::CaseSensitive))
            {
                result.append(line);
            }
        }
        m_model->addSong(result);

    }
}
void PlayerWidget::contextMenuEvent(QContextMenuEvent* ev)
{
    ev->ignore();
}
void PlayerWidget::addActionsIntoMenu(QMenu* menu)
{
    menu->addAction(m_playAct);
    menu->addAction(m_pauseAct);
    menu->addAction(m_stopAct);
    menu->addSeparator();
    menu->addAction(m_addAction);
    menu->addAction(m_addStreamAction);
    menu->addAction(m_openPlayList);
    menu->addAction(m_loadTableTopAudioPlayListAct);
    menu->addAction(m_savePlayList);
    menu->addAction(m_clearList);
    menu->addAction(m_deleteAction);
    menu->addSeparator();
}

void PlayerWidget::updateUi(bool isGM)
{
   /* if(isGM)
    {// Game Master*/
        m_ui->m_playButton->setVisible(isGM);
        m_ui->m_stopButton->setVisible(isGM);
        m_ui->m_pauseButton->setVisible(isGM);
        m_ui->m_uniqueMode->setVisible(isGM);
        m_ui->m_repeatMode->setVisible(isGM);
        m_ui->m_timeSlider->setVisible(isGM);
        m_ui->m_addButton->setVisible(isGM);
        //m_ui->m_volumeMutedButton->setVisible(true);
        m_ui->m_deleteButton->setVisible(isGM);
        m_ui->m_songList->setVisible(isGM);
        m_ui->m_savePlaylist->setVisible(isGM);
        m_ui->m_changeDirectory->setVisible(!isGM);
        m_ui->m_timerDisplay->setVisible(isGM);
   // }
   /* else//Player
    {
        m_ui->m_playButton->setVisible(false);
        m_ui->m_stopButton->setVisible(false);
        m_ui->m_pauseButton->setVisible(false);
        m_ui->m_uniqueMode->setVisible(false);
        m_ui->m_repeatMode->setVisible(false);
        m_ui->m_timeSlider->setVisible(false);
        //m_ui->m_volumeMutedButton->setVisible(true);
        m_ui->m_addButton->setVisible(false);
        m_ui->m_deleteButton->setVisible(false);
        m_ui->m_songList->setVisible(false);
        m_ui->m_savePlaylist->setVisible(false);
        m_ui->m_changeDirectory->setVisible(true);
        m_ui->m_timerDisplay->setVisible(false);

        m_ui->m_label->setEchoMode(QLineEdit::Password);

    }*/
    m_ui->m_volumeSlider->setValue(m_preferences->value(QString("volume_player_%1").arg(m_id),50).toInt());

}

void PlayerWidget::updateIcon()
{
    if(m_volumeMutedAct->isChecked())
    {
        m_volumeMutedAct->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else
    {
       m_volumeMutedAct->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}
void PlayerWidget::setTime(int time)
{
    m_player.setPosition(time);
}
void PlayerWidget::sourceChanged(const QMediaContent & media)
{
    if(media.canonicalUrl().isLocalFile())
    {
        emit newSongPlayed(m_id,media.canonicalUrl().fileName());
    }
    else
    {
        emit newSongPlayed(m_id,media.canonicalUrl().toString());
    }
}
void PlayerWidget::playerStatusChanged(QMediaPlayer::State newState)
{
    switch (newState)
    {
    case QMediaPlayer::StoppedState:
        emit playerStopped(m_id);
        break;
    case QMediaPlayer::PlayingState:
        emit playerIsPlaying(m_id,m_player.position());
        break;
    case QMediaPlayer::PausedState:
        emit playerIsPaused(m_id);
        break;
    }
}
void PlayerWidget::saveVolumeValue(int volume)
{
    m_preferences->registerValue(QString("volume_player_%1").arg(m_id),volume,true);
}
void PlayerWidget::removeAll()
{
    m_model->removeAll();
}
void PlayerWidget::playSong(quint64 pos)
{
    m_player.play();
}

void PlayerWidget::stop()
{
    m_player.stop();
}

void PlayerWidget::pause()
{
    m_player.pause();
}

void PlayerWidget::setPositionAt(quint64 pos)
{
    m_player.setPosition(pos);
}

void PlayerWidget::setSourceSong(QString file)
{

    QString path("%1/%2");
    QString dir = m_preferences->value(QString("MusicDirectoryPlayer_%1").arg(m_id),QDir::homePath()).toString();
    QUrl url(file,QUrl::StrictMode);
    if((!url.isValid())||(url.isRelative()))
    {
        url = QUrl::fromLocalFile(path.arg(dir).arg(file));
    }
    QMediaContent currentContent(url);

    m_ui->m_label->setText(file);
    m_player.setMedia(currentContent);
}
void PlayerWidget::changeDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Load Directory"), m_preferences->value(QString("MusicDirectoryPlayer_%1").arg(m_id),QDir::homePath()).toString());
    if(!dir.isEmpty())
    {
        m_preferences->registerValue(QString("MusicDirectoryPlayer_%1").arg(m_id),dir,true);
    }
}
void PlayerWidget::modeHasBeenChanged()
{
    if(m_repeatAct->isChecked())
    {
        m_playingMode = LOOP;
    }
    else if(m_uniqueAct->isChecked())
    {
        m_playingMode = UNIQUE;
    }
    else
    {
        m_playingMode = NEXT;
    }

}
void PlayerWidget::triggeredPlayingModeAction()
{
    QAction* act = qobject_cast<QAction*>(sender());
    bool status = act->isChecked();
    if(status)
    {
        if(m_uniqueAct == act)
        {
            m_repeatAct->setChecked(false);
        }
        else if(m_repeatAct == act)
        {
            m_uniqueAct->setChecked(false);
        }
    }

}
void PlayerWidget::loadPlayList()
{
    QStringList list;
    list << "http://tabletopaudio.com/download.php?downld_file=70_Age_of_Steam.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=69_Forest_Night.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=68_1940s_Office.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=67_Asylum.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=66_Royal_Salon.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=65_Dungeon_I.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=64_Mountain_Pass.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=63_Industrial_Shipyard.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=62_Middle_Earth_Dawn.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=61_Orbital_Platform.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=60_Dark_and_Stormy.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=59_Dinotopia.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=58_Terror.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=57_Colosseum.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=56_Medieval_Town.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=55_Ice_Cavern.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=54_Mountain_Tavern.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=53_Strangers_on_a_Train.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=52_Warehouse_13.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=51_Woodland_Campsite.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=50_Super_Hero.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=49_Goblin's_Cave.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=48_Overland_with_Oxen.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=47_There_be_Dragons.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=46_Cathedral.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=45_Samurai_HQ.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=44_Victorian_London.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=43_Dome_City_Center.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=42_Rise_of_the_Ancients.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=41_Starship_Bridge.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=40_The_Long_Rain.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=39_Temple_of_the_Eye.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=38_Into_the_Deep.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=34_Clash_of_Kings.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=36_Down_by_the_Sea.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=37_Catacombs.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=30_Los_Vangeles_3030.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=35_Swamplandia.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=33_Far_Above_the_World.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=32_City_and_the_City.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=29_Kaltoran_Craft_FE.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=28_Nephilim_Labs_FE.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=31_Frozen_Wastes.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=26_Uncommon_Valor_a.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=27_Xingu_Nights.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=25_Deep_Space_EVA.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=24_Forbidden_Galaxy.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=23_The_Slaughtered_Ox.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=21_Derelict_Freighter.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=22_True_West_a.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=20_Dark_Continent_aa.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=19_Age_of_Sail.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=18_House_on_the_Hill.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=17_Alien_Night_Club.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=15_Alien_Machine_Shop.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=16_Busy_Space_Port.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=4_Solemn_Vow-a.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=6_Abyssal_Gaze.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=7_The_Desert_Awaits.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=8_New_Dust_to_Dust.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=9_Before_The_Storm.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=10_In_The_Shadows.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=11_Shelter_from_the_Storm.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=12_Disembodied_Spirits.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=13_Cave_of_Time.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=14_Protean_Fields.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=1_The_Inner_Core.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=2_Bubbling_Pools.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=5_Desert_Bazaar.mp3"
        << "http://tabletopaudio.com/download.php?downld_file=3_The_March_of_the_Faithful.mp3";

    if(askToDeleteAll())
    {
        list.sort();
        m_model->removeAll();
        m_model->addSong(list);
    }
}
void  PlayerWidget::savePlaylist()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Play List"), m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString(), tr("PlayList (*.m3u)"));
    if (filename.isEmpty())
        return;


    if(!filename.endsWith(".m3u"))
    {
        filename.append(".m3u");
    }
    QFile file(filename);

    // if(file.isWritable())
     {
         file.open(QIODevice::WriteOnly);
         QTextStream in(&file);
         m_model->saveIn(in);
     }


 }
 void PlayerWidget::errorOccurs(QMediaPlayer::Error e)
 {
     QString Error("Error %1 : %2");
     m_ui->m_label->setText(Error.arg(m_player.errorString()).arg(m_player.currentMedia().canonicalUrl().toString()));
 }
 void PlayerWidget::labelTextChanged()
 {
     if(m_ui->m_label->text().startsWith("Error") && m_player.error()!=QMediaPlayer::NoError)
     {
         m_ui->m_label->setStyleSheet("color: red");
         m_ui->m_label->setEchoMode(QLineEdit::Normal);
     }
     else
     {
         m_ui->m_label->setStyleSheet("color: black");
         if(m_preferences->value("isPlayer",false).toBool())
         {// Player
                 m_ui->m_label->setEchoMode(QLineEdit::Password);
         }
     }
 }
 void PlayerWidget::seekHasChanged(bool seekable)
 {
  //   m_ui->m_timeSlider->setEnabled(seekable);
 }
