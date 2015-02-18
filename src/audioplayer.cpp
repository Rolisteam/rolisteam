/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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


#include <QtGui>
#include "audioplayer.h"


#include "preferencesmanager.h"

AudioPlayer * AudioPlayer::singleton = NULL;



AudioPlayer::AudioPlayer(QWidget *parent)
: QDockWidget(parent)
{
    m_options = PreferencesManager::getInstance();

    m_audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    m_mediaObject = new Phonon::MediaObject(this);
    m_path = new Phonon::Path();

    m_time = 0;
     connect(m_mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
     this, SLOT(stateChanged(Phonon::State, Phonon::State)));

connect(m_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
     this, SLOT(sourceChanged(const Phonon::MediaSource &)));

connect(m_mediaObject, SIGNAL(aboutToFinish()), this, SLOT(finishedSongSlot()));
        *m_path = Phonon::createPath(m_mediaObject, m_audioOutput);
setupUi();

        //autoriserOuIntedireCommandes();
        setWidget(m_mainWidget);
    }




AudioPlayer*  AudioPlayer::getInstance(QWidget *parent)
 {
        if(singleton==NULL)
            singleton = new AudioPlayer(parent);
        return singleton;
}
void AudioPlayer::setupUi()
{
        setWindowTitle(tr("Background music"));
        setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        setFeatures(QDockWidget::AllDockWidgetFeatures);
        setMinimumWidth(255);

        m_mainWidget = new QWidget();
        m_displayWidget = new QWidget();
        m_commandWidget = new QWidget();
        m_mainLayout = new QVBoxLayout(m_mainWidget);
        m_mainLayout->setMargin(0);

        QWidget *separateur1 = new QWidget();
        separateur1->setFixedHeight(2);
        m_mainLayout->addWidget(separateur1);

        m_mainLayout->addWidget(m_displayWidget);
        m_mainLayout->addWidget(m_commandWidget);

        QHBoxLayout *layoutAffichage = new QHBoxLayout(m_displayWidget);
        layoutAffichage->setMargin(0);
        m_informationScreen = new QLineEdit();
        m_informationScreen->setReadOnly(true);
        layoutAffichage->addWidget(m_informationScreen);


        // Creation du selecteur de volume
        m_volumeLevel = new Phonon::VolumeSlider(this);
        m_volumeLevel->setAudioOutput(m_audioOutput);
        m_volumeLevel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);



        layoutAffichage->addWidget(m_volumeLevel);

        QVBoxLayout *layoutCommande = new QVBoxLayout(m_commandWidget);
        layoutCommande->setMargin(0);
        QHBoxLayout *layoutTemps = new QHBoxLayout();
        layoutTemps->setMargin(0);

        m_timeSlider = new Phonon::SeekSlider(this);
        m_timeSlider->setMediaObject(m_mediaObject);
        m_timeSlider->setStyle(new QCleanlooksStyle());

        m_timeCounter = new QLCDNumber();
        m_timeCounter->setNumDigits(5);
        m_timeCounter->setSegmentStyle(QLCDNumber::Flat);
        m_timeCounter->display("0:00");
        m_timeCounter->setFixedWidth(40);
        layoutTemps->addWidget(m_timeSlider);
        layoutTemps->addWidget(m_timeCounter);
        layoutCommande->addLayout(layoutTemps);
        QHBoxLayout *layoutBoutons = new QHBoxLayout();
        layoutBoutons->setMargin(0);
        layoutBoutons->setSpacing(0);
        m_repeatAct	= new QAction(QIcon(":/resources/icons/boucle.png"), tr("Repeat"), m_commandWidget);
        m_oneperoneAct	= new QAction(QIcon(":/resources/icons/lecture unique.png"), tr("One Playing"), m_commandWidget);
        m_addAct 	= new QAction(tr("Add"), m_commandWidget);
        m_removeAct	= new QAction(tr("Remove"), m_commandWidget);


        m_playAct = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
        m_playAct->setShortcut(Qt::Key_Space);
        m_playAct->setDisabled(true);
        m_pauseAct = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
        m_pauseAct->setDisabled(true);
        m_stopAct = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
        m_stopAct->setDisabled(true);



        m_addAct->setToolTip(tr("Add one song into the list"));
        m_removeAct->setToolTip(tr("Remove the selected song"));
        m_playAct->setCheckable(true);
        m_pauseAct->setCheckable(true);
        m_repeatAct->setCheckable(true);
        m_oneperoneAct->setCheckable(true);
        QToolButton *boutonLecture		= new QToolButton(m_commandWidget);
        QToolButton *boutonPause		= new QToolButton(m_commandWidget);
        QToolButton *boutonStop			= new QToolButton(m_commandWidget);
        QToolButton *boutonBoucle		= new QToolButton(m_commandWidget);
        QToolButton *boutonUnique		= new QToolButton(m_commandWidget);
        QToolButton *boutonAjouter		= new QToolButton(m_commandWidget);
        QToolButton *boutonSupprimer	= new QToolButton(m_commandWidget);
        // Association des actions aux boutons
        boutonLecture	->setDefaultAction(m_playAct);
        boutonPause		->setDefaultAction(m_pauseAct);
        boutonStop		->setDefaultAction(m_stopAct);
        boutonBoucle	->setDefaultAction(m_repeatAct);
        boutonUnique	->setDefaultAction(m_oneperoneAct);
        boutonAjouter	->setDefaultAction(m_addAct);
        boutonSupprimer	->setDefaultAction(m_removeAct);
        // Boutons de commande en mode AutoRaise, plus lisible
        boutonLecture	->setAutoRaise(true);
        boutonPause		->setAutoRaise(true);
        boutonStop		->setAutoRaise(true);
        boutonBoucle	->setAutoRaise(true);
        boutonUnique	->setAutoRaise(true);
        // On fixe la taille des boutons
        boutonLecture	->setFixedSize(20, 20);
        boutonPause		->setFixedSize(20, 20);
        boutonStop		->setFixedSize(20, 20);
        boutonBoucle	->setFixedSize(20, 20);
        boutonUnique	->setFixedSize(20, 20);
        boutonAjouter	->setFixedSize(70, 20);
        boutonSupprimer	->setFixedSize(70, 20);
        // Creation d'un widget separateur
        QWidget *separateur2 = new QWidget();
        // Ajout des boutons au layout
        layoutBoutons->addWidget(boutonLecture);
        layoutBoutons->addWidget(boutonPause);
        layoutBoutons->addWidget(boutonStop);
        layoutBoutons->addWidget(boutonBoucle);
        layoutBoutons->addWidget(boutonUnique);
        layoutBoutons->addWidget(separateur2);
        layoutBoutons->addWidget(boutonAjouter);
        layoutBoutons->addWidget(boutonSupprimer);
        layoutCommande->addLayout(layoutBoutons);
        m_songListWidget = new QListWidget();



        m_songListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        layoutCommande->addWidget(m_songListWidget);



        connect(m_playAct, SIGNAL(triggered(bool)), this, SLOT(pressPlay(bool)));

        connect(m_pauseAct, SIGNAL(triggered()), m_mediaObject, SLOT(pause()));
        connect(m_stopAct, SIGNAL(triggered()), m_mediaObject, SLOT(stop()));


        connect(m_repeatAct, SIGNAL(triggered(bool)), this, SLOT(onRepeated(bool)));
        connect(m_oneperoneAct, SIGNAL(triggered(bool)), this, SLOT(onOnePerOne(bool)));
        connect(m_addAct, SIGNAL(triggered(bool)), this, SLOT(addSong()));
        connect(m_removeAct, SIGNAL(triggered(bool)), this, SLOT(removeSong()));


        connect(m_songListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(changeSong(QListWidgetItem *)));
        connect(m_mediaObject, SIGNAL(finished()), this, SLOT(finishedSongSlot()));

        m_playAct->setEnabled(false);
        m_pauseAct->setEnabled(false);
        m_stopAct->setEnabled(false);
        m_repeatAct->setEnabled(false);
        m_oneperoneAct->setEnabled(false);
        m_removeAct->setEnabled(false);
        m_timeSlider->setEnabled(false);

}
void AudioPlayer::changeSong(QListWidgetItem * p)
{
             m_currentsource = new Phonon::MediaSource(m_pathList[m_songListWidget->row(p)]);
             m_mediaObject->setCurrentSource(*m_currentsource);
             m_currentSong = m_songListWidget->row(p);
             m_mediaObject->play();
}

void AudioPlayer::pressPlay(bool state)
{
    if(m_pathList.isEmpty())
        return;

    if(state)
    {
        QListWidgetItem * p = m_songListWidget->currentItem();
        if(p != NULL)
            changeSong(p);
        else
            changeSong(m_songListWidget->item(0));
    }


}


void AudioPlayer::tick(qint64 time)
{
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);



        m_time = time;
        m_timeCounter->display(displayTime.toString("mm:ss"));
}
void AudioPlayer::sourceChanged(const Phonon::MediaSource &source)
{


     m_informationScreen->setText(source.fileName().right(source.fileName().length()-source.fileName().lastIndexOf("/")-1));
     m_timeCounter->display("00:00");
}

void AudioPlayer::stateChanged(Phonon::State newState, Phonon::State /*oldState*/)
{

     switch (newState) {
         case Phonon::ErrorState:

             break;
         case Phonon::PlayingState:
                 m_playAct->setEnabled(false);
                 m_pauseAct->setEnabled(true);
                 m_stopAct->setEnabled(true);
                 break;
         case Phonon::StoppedState:
                 //stop();
                 m_stopAct->setEnabled(false);
                 m_playAct->setEnabled(true);
                 m_pauseAct->setEnabled(false);
                 m_timeCounter->display("00:00");
                 break;
         case Phonon::PausedState:
                 m_stopAct->setEnabled(true);
                 m_currentState = PAUSE;
                 m_playAct->setEnabled(true);
                 m_pauseAct->setEnabled(true);
                 break;
         case Phonon::BufferingState:
                 break;
         default:
              break;
     }

}


 bool AudioPlayer::eventFilter(QObject *object, QEvent *event)
 {
     return false;
 }


void AudioPlayer::onRepeated(bool etatBouton)
{
        m_repeated = etatBouton;

        if (etatBouton)
        {
                m_oneperoneAct->setChecked(false);
                m_oneperone = false;
        }
}

void AudioPlayer::onOnePerOne(bool etatBouton)
{
        m_oneperone = etatBouton;

        if (etatBouton)
        {
                m_repeatAct->setChecked(false);
                m_repeated = false;
        }
}

void AudioPlayer::addSong()
{
        QStringList listeFichiers = QFileDialog::getOpenFileNames(this, tr("Ajouter un titre"), m_options->value("MusicDirectory",QVariant(".")).toString(), tr("Fichiers audio (*.wav *.mp2 *.mp3 *.ogg)"));

        if (listeFichiers.isEmpty())
                return;

        int dernierSlash = listeFichiers[0].lastIndexOf("/");
        m_options->registerValue("MusicDirectory",listeFichiers[0].left(dernierSlash));


        while (!listeFichiers.isEmpty())
        {
                QString fichier = listeFichiers.takeFirst();
                int dernierSlash = fichier.lastIndexOf("/");
                QString titre = fichier.right(fichier.length()-dernierSlash-1);

                if (m_pathList.isEmpty())
                {

                        m_playAct->setEnabled(true);
                        m_pauseAct->setEnabled(true);
                        m_stopAct->setEnabled(true);
                        m_repeatAct->setEnabled(true);
                        m_oneperoneAct->setEnabled(true);
                        m_removeAct->setEnabled(true);
                        m_timeSlider->setEnabled(true);

                        m_currentSong = 0;
                        addSong(titre, fichier);
                }

                // On ajoute le morceau a la liste
                QListWidgetItem *morceau = new QListWidgetItem(titre, m_songListWidget);
                morceau->setToolTip(fichier);
                m_pathList.append(fichier);
        }
}

void AudioPlayer::removeSong()
{
        QList<QListWidgetItem *> titreSelectionne = m_songListWidget->selectedItems();

        if (titreSelectionne.isEmpty())
                return;


        int ligne = m_songListWidget->row(titreSelectionne[0]);


        m_songListWidget->takeItem(ligne);
        m_pathList.takeAt(ligne);


        if (ligne == m_currentSong)
        {


                if (m_currentSong < m_pathList.size())
                {

                        addSong(m_songListWidget->item(m_currentSong)->text(), m_pathList[m_currentSong]);

                }


                else if (m_currentSong != 0)
                {
                        m_currentSong--;


                        addSong(m_songListWidget->item(m_currentSong)->text(), m_pathList[m_currentSong]);
                }

                else
                {



                        m_informationScreen->clear();
                        m_informationScreen->setToolTip("");

                        m_playAct->setEnabled(false);
                        m_pauseAct->setEnabled(false);
                        m_stopAct->setEnabled(false);
                        m_repeatAct->setEnabled(false);
                        m_oneperoneAct->setEnabled(false);
                        m_removeAct->setEnabled(false);
                        m_timeSlider->setEnabled(false);
                }
        }


        else if (ligne < m_currentSong)
                m_currentSong--;


}



void AudioPlayer::addSong(QString titre, QString fichier)
{

        m_informationScreen->setText(titre);
        m_informationScreen->setCursorPosition(0);
        m_informationScreen->setToolTip(fichier);
        m_currentsource = new Phonon::MediaSource(fichier);
        m_mediaObject->setCurrentSource(*m_currentsource);
}

/*void AudioPlayer::stop()
{
        m_pauseAct->setEnabled(false);
        m_playAct->setEnabled(false);
        m_currentState = arret;
}*/

/*void AudioPlayer::aboutToStop()
{

}*/

void AudioPlayer::finishedSongSlot()
{
        if (m_repeated)
        {

                m_mediaObject->enqueue(m_mediaObject->currentSource());


        }

        else if (m_oneperone)
        {

        }

        else
        {
                if (m_currentSong < m_pathList.size()-1)
                {

                        m_currentSong++;
                        m_currentState = PLAYING;
                        m_currentsource = new Phonon::MediaSource(m_pathList[m_currentSong]);
                        m_mediaObject->enqueue(*m_currentsource);

                }

                else
                {
                        //arreter();
                }
        }
}


qreal AudioPlayer::volume()
{
        return m_audioOutput->volume();
}

/*void AudioPlayer::NewFile(QString nomFichier)
{

        if (nomFichier.isEmpty())
        {
                // On efface l'afficheur de titre
                m_informationScreen->clear();
                m_informationScreen->setToolTip(tr("Aucun titre"));
                // On met le lecteur a l'arret
                m_currentState = arret;
                // On sort de la fonction
                return;
        }

        // Creation du chemin complet du fichier
        QString chemin(m_options->value("MusicDirectory",QVariant(".")).toString() + "/" + nomFichier);
        QFileInfo fileInfo(chemin);
        // Si l'ouverture s'est mal passee on affiche un message
        if (!fileInfo.exists())
        {
                qWarning("Impossible d'ouvrir le fichier audio (joueurNouveauFichier - AudioPlayer.cpp)");
                // On affiche le message en clair
                m_informationScreen->setEchoMode(QLineEdit::Normal);
                // Changement de la couleur du texte en rouge
                QPalette palette(m_informationScreen->palette());
                palette.setColor(QPalette::Normal, QPalette::Text, Qt::red);
                m_informationScreen->setPalette(palette);
                // On affiche le titre du nouveau morceau en rouge (indique que le fichier n'est pas present ou impossible a ouvrir)
                m_informationScreen->setText(nomFichier + tr(" : fichier introuvable ou impossible  ouvrir"));
                m_informationScreen->setCursorPosition(0);
                m_informationScreen->setToolTip(tr("Fichier introuvable ou impossible  ouvrir : ") + chemin);
                // On quitte la fonction
                return;
        }

        // Si l'ouverture du fichier s'est bien passee on ecrit en noir avec des asterisques
        else
        {
                m_currentsource = new Phonon::MediaSource(chemin);
                m_mediaObject->setCurrentSource(*m_currentsource);
                m_mediaObject->play();
                m_informationScreen->setEchoMode(QLineEdit::Password);
                // On ecrit en noir
                QPalette palette(m_informationScreen->palette());
                palette.setColor(QPalette::Normal, QPalette::Text, Qt::black);
                m_informationScreen->setPalette(palette);
        }

        // On affiche le titre du nouveau morceau
        m_informationScreen->setText(nomFichier);
        m_informationScreen->setCursorPosition(0);
        m_informationScreen->setToolTip(tr("Titre masqué"));

        // On met le lecteur a l'arret
        m_currentState = arret;
        // La lecture reprend depuis le debut
        playerTimePosition = 0;
}
void AudioPlayer::playerPlaying()
{
    if((m_mediaObject->state()==Phonon::PausedState)||(m_mediaObject->state()==Phonon::StoppedState))
    {
        m_mediaObject->play();
        m_currentState = lecture;


    }
}

void AudioPlayer::playerPause()
{
    if(m_mediaObject->state()==Phonon::PlayingState)
    {
        m_mediaObject->pause();
        m_currentState = pause;


    }
}

void AudioPlayer::joueurArretMorceau()
{
if(m_mediaObject->state()==Phonon::PlayingState)
    {
        m_mediaObject->stop();
        m_currentState = arret;

        playerTimePosition = 0;
    }
}
*/
void AudioPlayer::seek(int position)
{
    if(m_mediaObject->isSeekable())
            m_mediaObject->seek(position);
}

void AudioPlayer::changeSongDirectory()
{
        m_options->registerValue("MusicDirectory",QFileDialog::getExistingDirectory(0 , tr("Music directory"), m_options->value("MusicDirectory",QVariant(".")).toString(),
                QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks));
}

