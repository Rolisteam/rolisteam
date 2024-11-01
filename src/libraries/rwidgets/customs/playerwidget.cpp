/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
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
#include <QInputDialog>
#include <QMessageBox>
#include <QMimeData>

#include "playerwidget.h"

#include "controller/audioplayercontroller.h"
#include "ui_playerwidget.h"
#define FACTOR_WAIT 4

QString getExistingFile(const QString& rootDir, const QString& pathOnGM)
{
    QFileInfo info(pathOnGM);

    auto list= pathOnGM.split("/");
    QStringList result;
    for(auto& item : list)
    {
        result.prepend(item);
    }
    QString consumedPath= "";

    for(auto& item : result)
    {
        if(consumedPath.isEmpty())
            consumedPath= item;
        else
            consumedPath= item + "/" + consumedPath;
        auto temp= QStringLiteral("%1/%2").arg(rootDir, consumedPath);
        if(QFile::exists(temp))
        {
            return temp;
        }
    }
    return rootDir + "/" + info.fileName(); // error message
}

PlayerWidget::PlayerWidget(AudioPlayerController* ctrl, QWidget* parent)
    : QWidget(parent), m_ctrl(ctrl), m_ui(new Ui::PlayerWidgetUI)
{
    setAcceptDrops(true);

    m_ui->setupUi(this);

    if(!m_ctrl)
        return;

    setupUi();

    m_ui->m_songList->setModel(m_ctrl->model());

    connect(m_ctrl, &AudioPlayerController::localIsGmChanged, this, &PlayerWidget::updateUi);
    updateUi();
}

void PlayerWidget::setupUi()
{

    m_ui->m_timeSlider->setMinimum(0);
    m_ui->m_volumeSlider->setRange(0, 100);
    // **************** CREATE ACTIONS ********************************
    m_playAct= new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    m_pauseAct= new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    m_stopAct= new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);

    m_uniqueAct= new QAction(QIcon::fromTheme("playunique"), tr("Stop at the end"), this);
    m_uniqueAct->setShortcut(QKeySequence("Ctrl+U"));
    m_uniqueAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_uniqueAct->setCheckable(true);

    m_repeatAct= new QAction(QIcon::fromTheme("playloop"), tr("Play in loop"), this);
    m_repeatAct->setCheckable(true);
    m_repeatAct->setShortcut(QKeySequence("Ctrl+R"));
    m_repeatAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_shuffleAct= new QAction(QIcon::fromTheme("shuffle_mode"), tr("Shuffle Mode"), this);
    m_shuffleAct->setCheckable(true);
    m_shuffleAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_volumeMutedAct= new QAction(tr("Mute Volume"), this);
    m_volumeMutedAct->setCheckable(true);
    m_volumeMutedAct->setShortcut(QKeySequence("Ctrl+M"));
    m_volumeMutedAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_loadTableTopAudioPlayListAct= new QAction(tr("load TableTopAudio.com playlist"), this);

    m_openPlayList= new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), tr("Open Playlist"), this);
    m_openPlayList->setShortcut(QKeySequence("Ctrl+J"));
    m_openPlayList->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_savePlayList= new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save Playlist"), this);
    m_savePlayList->setShortcut(QKeySequence("Ctrl+E"));
    m_savePlayList->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_clearList= new QAction(style()->standardIcon(QStyle::SP_DialogResetButton), tr("Clear"), this);
    m_clearList->setShortcut(QKeySequence("Ctrl+Del"));
    m_clearList->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    m_addAction= new QAction(QIcon::fromTheme("add"), tr("Add Songs"), this);
    m_addAction->setShortcut(QKeySequence("Ctrl+A"));
    m_addAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    m_addStreamAction= new QAction(tr("Open Stream"), this);

    m_deleteAction= new QAction(QIcon::fromTheme("remove"), tr("Remove Song"), this);
    m_deleteAction->setShortcut(QKeySequence("Del"));
    m_deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    if(m_ctrl)
        m_ui->m_volumeSlider->setValue(m_ctrl->volume());
    // m_audioFileFilter= m_preferences->value("AudioFileFilter", "*.wav *.mp2 *.mp3 *.ogg *.flac").toString();

    // **************** Add ACTIONS ********************************

    addAction(m_uniqueAct);
    addAction(m_repeatAct);
    addAction(m_shuffleAct);
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
    m_ui->m_shuffleBtn->setDefaultAction(m_shuffleAct);
    m_ui->m_repeatMode->setDefaultAction(m_repeatAct);

    m_ui->m_addPathBtn->setDefaultAction(m_ui->m_addPathAct);
    m_ui->m_removePathBtn->setDefaultAction(m_ui->m_removePathAct);
    m_ui->m_upPathBtn->setDefaultAction(m_ui->m_moveUpPath);
    m_ui->m_downPathBtn->setDefaultAction(m_ui->m_moveDownAct);

    // **************** CONNECT ********************************
    connect(m_clearList, &QAction::triggered, m_ctrl, &AudioPlayerController::clear);
    connect(m_addAction, &QAction::triggered, this, &PlayerWidget::addFiles);
    connect(m_openPlayList, &QAction::triggered, this, &PlayerWidget::openPlayList);
    connect(m_addStreamAction, &QAction::triggered, this, &PlayerWidget::openStream);
    connect(m_deleteAction, &QAction::triggered, this,
            [this]() { m_ctrl->removeSong(m_ui->m_songList->selectionModel()->selectedIndexes()); });
    connect(m_ui->m_songList, &QListView::doubleClicked, m_ctrl,
            [this](const QModelIndex& index)
            {
                m_ctrl->setMedia(index);
                m_ctrl->play();
            });

    connect(m_ui->m_addPathAct, &QAction::triggered, this,
            [this]()
            {
                auto dir= QFileDialog::getExistingDirectory(this, tr("Add new directory"), QDir::homePath());

                if(dir.isEmpty())
                    return;
                m_ctrl->addDirectory(dir);
            });
    connect(m_ui->m_removePathAct, &QAction::triggered, this,
            [this]()
            {
                auto idx= m_ui->m_songList->currentIndex();
                if(!idx.isValid())
                    return;
                m_ctrl->removeDirectory(idx.row());
            });

    connect(m_ui->m_moveUpPath, &QAction::triggered, this,
            [this]()
            {
                auto idx= m_ui->m_songList->currentIndex();
                if(!idx.isValid())
                    return;
                m_ctrl->moveDirectory(idx.row(), true);
            });
    connect(m_ui->m_moveDownAct, &QAction::triggered, this,
            [this]()
            {
                auto idx= m_ui->m_songList->currentIndex();
                if(!idx.isValid())
                    return;
                m_ctrl->moveDirectory(idx.row(), false);
            });

    updateIcon();
    // connect(&m_player, SIGNAL(durationChanged(qint64)), this, SLOT(setDuration(qint64)));
    connect(m_playAct, &QAction::triggered, this,
            [this]()
            {
                if(m_ctrl->state() == AudioPlayerController::PausedState)
                    m_ctrl->play();
                else
                {
                    auto index= m_ui->m_songList->currentIndex();
                    if(!index.isValid())
                        return;
                    m_ctrl->setMedia(index);
                    m_ctrl->play();
                }
            }); //&AudioPlayerController::play
    connect(m_stopAct, &QAction::triggered, m_ctrl, &AudioPlayerController::stop);
    connect(m_pauseAct, &QAction::triggered, m_ctrl, &AudioPlayerController::pause);

    // time
    connect(m_ctrl, &AudioPlayerController::timeChanged, m_ui->m_timeSlider,
            [this]() { m_ui->m_timeSlider->setValue(m_ctrl->time()); });
    connect(m_ui->m_timeSlider, &QSlider::sliderMoved, m_ctrl,
            [this](qint64 time)
            {
                if(!m_ui->m_timeSlider || !m_ui->m_timeSlider->isVisible())
                    return;

                m_ctrl->setTime(time);
            });

    // Volume
    connect(m_ctrl, &AudioPlayerController::volumeChanged, m_ui->m_volumeSlider, &QSlider::setValue);
    connect(m_ui->m_volumeSlider, &QSlider::sliderMoved, m_ctrl, &AudioPlayerController::setVolume);
    // Mute
    connect(m_ctrl, &AudioPlayerController::mutedChanged, this, &PlayerWidget::updateIcon);
    connect(m_volumeMutedAct, &QAction::triggered, m_ctrl, &AudioPlayerController::mute);

    // duration
    connect(m_ctrl, &AudioPlayerController::durationChanged, m_ui->m_timeSlider, &QSlider::setMaximum);
    connect(m_ctrl, &AudioPlayerController::timeChanged, m_ui->m_timerDisplay,
            [this](qint64 position)
            {
                auto posInSecond= position / 1000;
                auto min= posInSecond / 60;
                auto second= posInSecond - (min * 60);
                if(min > 0)
                    m_ui->m_timerDisplay->setText(
                        QString("%1min%2s").arg(min).arg(second, 2, 10, QLatin1Char('0'))); //, 3, 10, QLatin1Char('0')
                else
                    m_ui->m_timerDisplay->setText(QString("%1s").arg(second, 2, 10, QLatin1Char('0')));
            });
    // QOverload<int>::of(&QLCDNumber::display));
    connect(m_ctrl, &AudioPlayerController::visibleChanged, this, &PlayerWidget::setVisible);
    setVisible(m_ctrl->visible());

    connect(m_ctrl, &AudioPlayerController::errorChanged, this,
            [this]()
            {
                m_ui->m_label->setStyleSheet("color: red");
                m_ui->m_label->setEchoMode(QLineEdit::Normal);
                m_ui->m_label->setText(m_ctrl->error());
            });
    connect(m_ctrl, &AudioPlayerController::textChanged, this,
            [this]()
            {
                m_ui->m_label->setStyleSheet("color: black");
                if(!m_ctrl->localIsGm())
                    m_ui->m_label->setEchoMode(QLineEdit::Password);
                m_ui->m_label->setText(m_ctrl->text());
            });

    connect(m_repeatAct, &QAction::triggered, this,
            [this](bool b) { m_ctrl->setPlayingMode(b ? AudioPlayerController::LOOP : AudioPlayerController::NEXT); });
    connect(m_uniqueAct, &QAction::triggered, this,
            [this](bool b)
            { m_ctrl->setPlayingMode(b ? AudioPlayerController::UNIQUE : AudioPlayerController::NEXT); });
    connect(m_shuffleAct, &QAction::triggered, this,
            [this](bool b)
            { m_ctrl->setPlayingMode(b ? AudioPlayerController::SHUFFLE : AudioPlayerController::NEXT); });

    connect(m_loadTableTopAudioPlayListAct, &QAction::triggered, this, &PlayerWidget::openPlayList);
    connect(m_savePlayList, &QAction::triggered, this, &PlayerWidget::savePlaylist);

    connect(m_ctrl, &AudioPlayerController::stateChanged, this,
            [this]()
            {
                auto state= m_ctrl->state();
                m_playAct->setEnabled(state == AudioPlayerController::PausedState
                                      || state == AudioPlayerController::StoppedState);

                m_pauseAct->setEnabled(state == AudioPlayerController::PlayingState
                                       || state == AudioPlayerController::StoppedState);
                m_stopAct->setEnabled(state == AudioPlayerController::PlayingState);
            });

    auto updateMode= [this]()
    {
        auto m= m_ctrl->mode();
        m_uniqueAct->setChecked(m == AudioPlayerController::UNIQUE);
        m_shuffleAct->setChecked(m == AudioPlayerController::SHUFFLE);
        m_repeatAct->setChecked(m == AudioPlayerController::LOOP);
    };
    connect(m_ctrl, &AudioPlayerController::modeChanged, this, updateMode);

    updateMode();
}

void PlayerWidget::addFiles()
{
    auto fileList
        = QFileDialog::getOpenFileUrls(this, tr("Add song"), QUrl(), tr("Audio files (%1)").arg("*.mp3 *.flac *.webm"));
    if(fileList.isEmpty())
        return;
    m_ctrl->addSong(fileList);
}
bool PlayerWidget::askToDeleteAll()
{
    if(m_ctrl->model()->rowCount() == 0)
        return true;

    return (
        QMessageBox::Ok
        == QMessageBox::warning(this, tr("Attention!"),
                                tr("You are about to load an new playlist. All previously load file will be dropped."),
                                QMessageBox::Ok, QMessageBox::Cancel));
}
void PlayerWidget::openPlayList()
{
    if(!askToDeleteAll())
    {
        return;
    }
    QString filename= QFileDialog::getOpenFileName(this, tr("Open Play List"), "", tr("PlayList (*.m3u)"));
    if(filename.isEmpty())
        return;

    m_ctrl->loadPlayList(filename);
}
void PlayerWidget::openStream()
{
    bool a;
    QString value= QInputDialog::getText(this, tr("Open audio Stream"), tr("URL"), QLineEdit::Normal, QString(), &a);
    if(!a)
        return;

    QUrl url(value);
    if(!url.isValid())
        return;

    m_ctrl->addSong({url});
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

void PlayerWidget::updateUi()
{
    auto isGM= m_ctrl->localIsGm();

    m_ui->m_playButton->setVisible(isGM);
    m_ui->m_stopButton->setVisible(isGM);
    m_ui->m_pauseButton->setVisible(isGM);
    m_ui->m_uniqueMode->setVisible(isGM);
    m_ui->m_repeatMode->setVisible(isGM);
    m_ui->m_shuffleBtn->setVisible(isGM);
    m_ui->m_timeSlider->setVisible(isGM);
    m_ui->m_addButton->setVisible(isGM);
    m_ui->m_deleteButton->setVisible(isGM);
    // m_ui->m_songList->setVisible(isGM);
    m_ui->m_savePlaylist->setVisible(isGM);
    m_ui->m_timerDisplay->setVisible(isGM);
    m_ui->m_volumeSlider->setValue(m_ctrl->volume());

    m_ui->m_songList->setModel(isGM ? static_cast<QAbstractItemModel*>(m_ctrl->model()) :
                                      static_cast<QAbstractItemModel*>(m_ctrl->directories()));

    m_ui->m_addPathBtn->setVisible(!isGM);
    m_ui->m_removePathBtn->setVisible(!isGM);
    m_ui->m_upPathBtn->setVisible(!isGM);
    m_ui->m_downPathBtn->setVisible(!isGM);
    m_ui->m_pathLabel->setVisible(!isGM);
}

void PlayerWidget::updateIcon()
{
    m_volumeMutedAct->setIcon(
        style()->standardIcon(m_ctrl->muted() ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume));
    m_volumeMutedAct->setChecked(m_ctrl->muted());
}

void PlayerWidget::savePlaylist()
{
    QString filename= QFileDialog::getSaveFileName(this, tr("Save Play List"), "", tr("PlayList (*.m3u)"));
    if(filename.isEmpty())
        return;

    m_ctrl->loadPlayList(filename);
}

void PlayerWidget::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData= event->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> list= mimeData->urls();
        for(auto& url : list)
        {
            if(url.toLocalFile().endsWith(".m3u")) // play list
            {
                m_ctrl->loadPlayList(url.toLocalFile());
            }
            else
            {
                m_ctrl->addSong({url.toLocalFile()});
            }
        }
        event->acceptProposedAction();
    }
}
