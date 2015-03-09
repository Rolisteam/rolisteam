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
    //m_ui->m_groupBox->setTitle(title);
    m_model = new MusicModel(this);
    m_ui->m_songList->setModel(m_model);

    updateUi();
}

void PlayerWidget::startMedia(QMediaContent* p)
{
    m_content = p;
    m_player.setMedia(*m_content);
    m_ui->m_timeSlider->setMinimum(0);
    m_ui->m_label->setText(p->canonicalUrl().fileName());

    m_player.play();
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
     //m_timerDisplay->display(displayTime.toString("mm:ss"));

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
    switch (m_playingMode)
    {
    case NEXT:
    {
        QModelIndex index = m_ui->m_songList->currentIndex();
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
    m_uniqueAct = new QAction(QIcon("://resources/icones/playunique.png"),tr("Next"),this);
    m_uniqueAct->setCheckable(true);
    m_repeatAct = new QAction(QIcon("://resources/icones/playloop.png"),tr("Previous"),this);
    m_repeatAct->setCheckable(true);
    m_changeDirectoryAct = new QAction(style()->standardIcon(QStyle::SP_DirIcon),tr("Open Directory"),this);
    m_volumeMutedAct = new QAction(this);
    m_volumeMutedAct->setCheckable(true);


    m_openPlayList= new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton),tr("Open Playlist"),this);
    m_savePlayList= new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton),tr("Save"),this);
    m_clearList= new QAction(style()->standardIcon(QStyle::SP_DialogResetButton),tr("Clear"),this);

    m_addAction 	= new QAction(QIcon("://resources/icones/add.png"),tr("Add"), this);
    m_deleteAction	= new QAction(QIcon("://resources/icones/remove.png"),tr("Remove"), this);


    m_ui->m_volumeSlider->setValue(m_preferences->value(QString("volume_player_%1").arg(m_id),50).toInt());



    // **************** TOOLTIP ACTIONS ********************************
    m_addAction->setToolTip(tr("Add song to the list"));
    m_deleteAction->setToolTip(tr("Remove selected file"));



    // **************** Add Action In Buttons ********************************
    m_ui->m_deleteButton->setDefaultAction(m_deleteAction);
    m_ui->m_deleteButton->addAction(m_clearList);
    m_ui->m_savePlaylist->setDefaultAction(m_savePlayList);
    m_ui->m_addButton->setDefaultAction(m_addAction);
    m_ui->m_addButton->addAction(m_openPlayList);

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
    connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(removeFile()));
    connect(m_clearList,SIGNAL(triggered(bool)),this,SLOT(removeAll()));
    connect(m_ui->m_songList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(startMediaByModelIndex(QModelIndex)));
    updateIcon();
    connect(&m_player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
    connect(&m_player,SIGNAL(durationChanged(qint64)),this,SLOT(setDuration(qint64)));
    connect(m_playAct,SIGNAL(triggered()),&m_player,SLOT(play()));
    connect(m_stopAct,SIGNAL(triggered()),&m_player,SLOT(stop()));
    connect(m_pauseAct,SIGNAL(triggered()),&m_player,SLOT(pause()));
    connect(m_ui->m_timeSlider,SIGNAL(sliderMoved(int)),this,SLOT(setTime(int)));
    connect(m_ui->m_volumeSlider,SIGNAL(valueChanged(int)),&m_player,SLOT(setVolume(int)));
    connect(&m_player,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(sourceChanged(QMediaContent)));
    connect(&m_player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(playerStatusChanged(QMediaPlayer::State)));
    connect(m_volumeMutedAct,SIGNAL(toggled(bool)),&m_player,SLOT(setMuted(bool)));
    connect(m_volumeMutedAct,SIGNAL(toggled(bool)),this,SLOT(updateIcon()));
    connect(m_changeDirectoryAct,SIGNAL(triggered()),this,SLOT(changeDirectory()));
    connect(&m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(m_repeatAct,SIGNAL(triggered()),this,SLOT(triggeredPlayingModeAction()));
    connect(m_uniqueAct,SIGNAL(triggered()),this,SLOT(triggeredPlayingModeAction()));


}
void PlayerWidget::startMediaByModelIndex(QModelIndex p)//double click
{

      startMedia(m_model->getMediaByModelIndex(p));   
          //  m_mediaObject->play();
}

void PlayerWidget::removeFile()
{
    QModelIndexList list = m_ui->m_songList->selectionModel()->selectedIndexes();
    m_model->removeSong(list);
}


void PlayerWidget::addFiles()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Add song"), m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString(), tr("Audio files (*.wav *.mp2 *.mp3 *.ogg *.flac)"));
    if (fileList.isEmpty())
        return;
    QFileInfo fileinfo(fileList[0]);
    m_preferences->registerValue("MusicDirectoryGM",fileinfo.absolutePath());
    m_model->addSong(fileList);
    while (!fileList.isEmpty())
    {
        QString fichier = fileList.takeFirst();
        QFileInfo fi(fichier);
    }
}
void PlayerWidget::openPlayList()
{
    if(m_model->rowCount()!=0)
    {
        if(QMessageBox::Ok == QMessageBox::warning(this,tr("Attention!"),tr("You are about to load an new playlist. All previously load file will be dropped."),QMessageBox::Ok, QMessageBox::Cancel))
        {
            m_model->removeAll();
        }
        else
        {
            return;
        }
    }
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Play List"), m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString(), tr("PlayList (*.m3u)"));
    if (filename.isEmpty())
        return;
    QFileInfo fileinfo(filename);
    m_preferences->registerValue("MusicDirectoryGM",fileinfo.absolutePath());
    readM3uPlayList(filename);


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
    menu->addAction(m_openPlayList);
    menu->addAction(m_savePlayList);
    menu->addAction(m_clearList);
    menu->addAction(m_deleteAction);
    menu->addSeparator();
}

void PlayerWidget::updateUi()
{
    if(!m_preferences->value("isPlayer",false).toBool())
    {
        m_ui->m_playButton->setVisible(true);
        m_ui->m_stopButton->setVisible(true);
        m_ui->m_pauseButton->setVisible(true);
        m_ui->m_uniqueMode->setVisible(true);
        m_ui->m_repeatMode->setVisible(true);
        m_ui->m_timeSlider->setVisible(true);
        m_ui->m_addButton->setVisible(true);
        //m_ui->m_volumeMutedButton->setVisible(true);
        m_ui->m_deleteButton->setVisible(true);
        m_ui->m_songList->setVisible(true);
        m_ui->m_savePlaylist->setVisible(true);
        m_ui->m_changeDirectory->setVisible(false);
    }
    else
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

    }
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
   emit newSongPlayed(m_id,media.canonicalUrl().fileName());
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

    QMediaContent currentContent(QUrl::fromLocalFile(path.arg(dir).arg(file)));
    qDebug() << dir << file;
    m_player.setMedia(currentContent);
}
void PlayerWidget::changeDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Load Directory"), m_preferences->value(QString("MusicDirectoryPlayer_%1").arg(m_id),QDir::homePath()).toString());
    qDebug() << "directory change"<< dir;
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
