#include <QFileDialog>
#include <QMessageBox>

#include "playerwidget.h"

#include "ui_audiowidget.h"
#include "networkmessage.h"

PlayerWidget::PlayerWidget(int id, QWidget* parent)
    : QWidget(parent),m_id(id),m_ui(new Ui::AudioWidgetUI)
{
    m_preferences = PreferencesManager::getInstance();
    m_ui->setupUi(this);
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
     /*if((!G_joueur) && ((time>m_time+(FACTOR_WAIT*m_mediaPlayer->notifyInterval()))||(time<m_time)))
     {
          emettreCommande(nouvellePositionMorceau, "", time);
          emitCurrentState();
     }*/
    // m_time = time;
     m_ui->m_timeSlider->setValue(time);
     //m_timerDisplay->display(displayTime.toString("mm:ss"));

}

void PlayerWidget::setupUi()
{
    m_ui->m_timeSlider->setMinimum(0);
    m_ui->m_volumeSlider->setRange(0,100);
    // **************** CREATE ACTIONS ********************************
    m_playAct = new QAction(style()->standardIcon(QStyle::SP_MediaPlay),tr("Play"),this);
    m_pauseAct = new QAction(style()->standardIcon(QStyle::SP_MediaPause),tr("Pause"),this);
    m_stopAct = new QAction(style()->standardIcon(QStyle::SP_MediaStop),tr("Stop"),this);
    m_uniqueAct = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward),tr("Next"),this);
    m_repeatAct = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward),tr("Previous"),this);
    m_volumeMutedAct = new QAction(this);

    m_openPlayList= new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton),tr("Open Playlist"),this);
    m_savePlayList= new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton),tr("Save"),this);
    m_clearList= new QAction(style()->standardIcon(QStyle::SP_DialogResetButton),tr("Clear"),this);

    m_addAction 	= new QAction(tr("Add"), this);
    m_deleteAction	= new QAction(tr("Remove"), this);

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
    

    // **************** CONNECT ********************************
    connect(m_addAction, SIGNAL(triggered(bool)), this, SLOT(addFiles()));
    connect(m_openPlayList, SIGNAL(triggered(bool)), this, SLOT(openPlayList()));
    connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(removeFile()));
    connect(m_ui->m_songList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(clickOnList(QModelIndex)));
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



}
void PlayerWidget::clickOnList(QModelIndex p)//double click
{

      startMedia(m_model->getMediaByModelIndex(p));   
          //  m_mediaObject->play();
}

void PlayerWidget::removeFile()
{

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
       // QString titre = fi.fileName();

        {

            //sendCommand(nouveauMorceau, titre);
            // On active tous les boutons
        }
        //morceau->setToolTip(fichier);

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
        m_ui->m_volumeMutedButton->setVisible(true);

    }
    else
    {
        m_ui->m_playButton->setVisible(false);
        m_ui->m_stopButton->setVisible(false);
        m_ui->m_pauseButton->setVisible(false);
        m_ui->m_uniqueMode->setVisible(false);
        m_ui->m_repeatMode->setVisible(false);
        m_ui->m_timeSlider->setVisible(false);
        m_ui->m_volumeMutedButton->setVisible(false);

    }
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

}
void PlayerWidget::playerStatusChanged(QMediaPlayer::State newState)
{
    switch (newState)
    {
    case QMediaPlayer::StoppedState:
        //sendCommand(NetMsg::StopSong);
        break;
    case QMediaPlayer::PlayingState:
        //sendCommand(NetMsg::PlaySong,"",m_time);
        break;
    case QMediaPlayer::PausedState:
        //sendCommand(NetMsg::PauseSong);
        break;
    }
}
void PlayerWidget::saveVolumeValue(int volume)
{
    m_preferences->registerValue(QString("volume_player_%1").arg(m_id),volume,true);
}
