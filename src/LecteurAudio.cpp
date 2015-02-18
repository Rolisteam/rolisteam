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

#include "LecteurAudio.h"

#include "ClientServeur.h"
#include "Liaison.h"

#include "variablesGlobales.h"
#include "constantesGlobales.h"
#include "types.h"

#include <QDebug>
#define FACTOR_WAIT 4
LecteurAudio * LecteurAudio::m_singleton = NULL;

LecteurAudio::LecteurAudio(QWidget *parent)
    : QDockWidget(parent),m_currentSource(NULL)
{
    m_preferences = PreferencesManager::getInstance();
    setObjectName("MusicPlayer");

    m_endFile= false;
    m_currentPlayingMode = NEXT;
    m_formerItemFile =NULL;
    m_currentItemFile =NULL;
    if(G_joueur)/// fully defined by the GM
    {
            m_currentPlayingMode=UNIQUE;
    }
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    m_mediaObject = new Phonon::MediaObject(this);
    path = new Phonon::Path();
    m_time = 0;


    *path = Phonon::createPath(m_mediaObject, audioOutput);
    setupUi();


    setWidget(m_mainWidget);
}

LecteurAudio*  LecteurAudio::getInstance(QWidget *parent)
 {
        if(m_singleton==NULL)
        {
            m_singleton = new LecteurAudio(parent);
        }
        return m_singleton;
}
void LecteurAudio::onfinished()
{
    /// @todo the m_currentsource must be checked
//    if (m_currentPlayingMode==LOOP)
//    {
//        if(m_currentSource!=NULL)
//        {
//            m_mediaObject->setCurrentSource(*m_currentSource);
//            m_mediaObject->play();

//        }
//        else
//        {
//            m_mediaObject->stop();
//        }
//    }
//    if(G_joueur)
//    {
//        m_mediaObject->stop();
//    }
}
LecteurAudio::~LecteurAudio()
{
    delete m_mainWidget;
    delete path;
}

void LecteurAudio::setupUi()
{
        setWindowTitle(tr("Background Music"));
        setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        setFeatures(QDockWidget::AllDockWidgetFeatures);
        setMinimumWidth(255);
        m_mainWidget = new QWidget();
        m_displayWidget = new QWidget();
        m_commandWidget = new QWidget();
        layoutPrincipal = new QVBoxLayout(m_mainWidget);
        //layoutPrincipal = new QVBoxLayout(this);
        layoutPrincipal->setMargin(0);
        QWidget *separateur1 = new QWidget();
        separateur1->setFixedHeight(2);
        layoutPrincipal->addWidget(separateur1);
        layoutPrincipal->addWidget(m_displayWidget);
        layoutPrincipal->addWidget(m_commandWidget);
        QHBoxLayout *layoutAffichage = new QHBoxLayout(m_displayWidget);
        layoutAffichage->setMargin(0);
        m_titleDisplay = new QLineEdit();
        m_titleDisplay->setReadOnly(true);

        layoutAffichage->addWidget(m_titleDisplay);
        if (G_joueur)
        {
                QAction *actionChangerDossier = new QAction(QIcon(":/resources/icones/dossier.png"), tr("Select the directory contening all music files"), m_displayWidget);
                QToolButton *boutonChangerDossier = new QToolButton(m_displayWidget);
                boutonChangerDossier->setDefaultAction(actionChangerDossier);
                boutonChangerDossier->setFixedSize(20, 20);
                layoutAffichage->addWidget(boutonChangerDossier);
                connect(actionChangerDossier, SIGNAL(triggered()), this, SLOT(pChangeDirectory()));
        }

        m_volumeLevelSlider = new Phonon::VolumeSlider(this);
        m_volumeLevelSlider->setAudioOutput(audioOutput);
        m_volumeLevelSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

        layoutAffichage->addWidget(m_volumeLevelSlider);

        QVBoxLayout *layoutCommande = new QVBoxLayout(m_commandWidget);
        layoutCommande->setMargin(0);

        QHBoxLayout *layoutTemps = new QHBoxLayout();
        layoutTemps->setMargin(0);

        m_timePosition = new Phonon::SeekSlider(this);
        m_timePosition->setMediaObject(m_mediaObject);
        m_timePosition->setStyle(new QCleanlooksStyle());

        m_timerDisplay = new QLCDNumber();
        m_timerDisplay->setNumDigits(5);
        m_timerDisplay->setSegmentStyle(QLCDNumber::Flat);
        m_timerDisplay->display("0:00");
        m_timerDisplay->setFixedWidth(40);

        layoutTemps->addWidget(m_timePosition);
        layoutTemps->addWidget(m_timerDisplay);

        layoutCommande->addLayout(layoutTemps);

        QHBoxLayout *layoutBoutons = new QHBoxLayout();
        layoutBoutons->setMargin(0);
        layoutBoutons->setSpacing(0);

        m_loopAction	= new QAction(QIcon(":/resources/icones/boucle.png"), tr("Loop"), m_commandWidget);
        m_uniqueAction	= new QAction(QIcon(":/resources/icones/lecture unique.png"), tr("Single shot"), m_commandWidget);
        m_addAction 	= new QAction(tr("Add"), m_commandWidget);
        m_deleteAction	= new QAction(tr("Remove"), m_commandWidget);

        m_playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
        m_playAction->setShortcut(Qt::Key_Space);
        m_playAction->setDisabled(true);
        m_pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
        m_pauseAction->setDisabled(true);
        m_stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
        m_stopAction->setDisabled(true);
        m_stopAction->setCheckable(true);

        m_addAction->setToolTip(tr("Add song to the list"));
        m_deleteAction->setToolTip(tr("Remove selected file"));
        m_playAction->setCheckable(true);
        m_pauseAction->setCheckable(true);
        m_loopAction->setCheckable(true);
        m_uniqueAction->setCheckable(true);

        QToolButton *boutonLecture= new QToolButton(m_commandWidget);
        QToolButton *boutonPause= new QToolButton(m_commandWidget);
        QToolButton *boutonStop= new QToolButton(m_commandWidget);
        QToolButton *boutonBoucle= new QToolButton(m_commandWidget);
        QToolButton *boutonUnique= new QToolButton(m_commandWidget);
        QToolButton *boutonAjouter= new QToolButton(m_commandWidget);
        QToolButton *boutonSupprimer= new QToolButton(m_commandWidget);

        boutonLecture->setDefaultAction(m_playAction);
        boutonPause->setDefaultAction(m_pauseAction);
        boutonStop->setDefaultAction(m_stopAction);
        boutonBoucle->setDefaultAction(m_loopAction);
        boutonUnique->setDefaultAction(m_uniqueAction);
        boutonAjouter->setDefaultAction(m_addAction);
        boutonSupprimer->setDefaultAction(m_deleteAction);

        boutonLecture->setAutoRaise(true);
        boutonPause->setAutoRaise(true);
        boutonStop->setAutoRaise(true);
        boutonBoucle->setAutoRaise(true);
        boutonUnique->setAutoRaise(true);

        boutonLecture->setFixedSize(20, 20);
        boutonPause->setFixedSize(20, 20);
        boutonStop->setFixedSize(20, 20);
        boutonBoucle->setFixedSize(20, 20);
        boutonUnique->setFixedSize(20, 20);
        boutonAjouter->setFixedSize(70, 20);
        boutonSupprimer->setFixedSize(70, 20);

        QWidget *separateur2 = new QWidget();

        layoutBoutons->addWidget(boutonLecture);
        layoutBoutons->addWidget(boutonPause);
        layoutBoutons->addWidget(boutonStop);
        layoutBoutons->addWidget(boutonBoucle);
        layoutBoutons->addWidget(boutonUnique);
        layoutBoutons->addWidget(separateur2);
        layoutBoutons->addWidget(boutonAjouter);
        layoutBoutons->addWidget(boutonSupprimer);

        layoutCommande->addLayout(layoutBoutons);
        m_songList = new QListWidget();
        m_songList->setSelectionMode(QAbstractItemView::SingleSelection);
        layoutCommande->addWidget(m_songList);

        connect(m_playAction, SIGNAL(triggered()), m_mediaObject, SLOT(play()));
        connect(m_pauseAction, SIGNAL(triggered()), m_mediaObject, SLOT(pause()));
        connect(m_stopAction, SIGNAL(triggered()), m_mediaObject, SLOT(stop()));
        connect(m_stopAction, SIGNAL(triggered()), this, SLOT(pstop()));
        connect(m_loopAction, SIGNAL(triggered()), this, SLOT(updatePlayingMode()));
        connect(m_uniqueAction, SIGNAL(triggered()), this, SLOT(updatePlayingMode()));
        connect(m_addAction, SIGNAL(triggered(bool)), this, SLOT(addFiles()));
        connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(removeFile()));
        connect(m_songList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(clickOnList(QListWidgetItem *)));
        connect(m_songList,SIGNAL(itemSelectionChanged()),this,SLOT(selectionHasChanged()));
        connect(m_mediaObject, SIGNAL(finished()), this, SLOT(isAboutToFinish()));


        m_playAction->setEnabled(false);
        m_pauseAction->setEnabled(false);
        m_stopAction->setEnabled(false);

        m_deleteAction->setEnabled(false);
        m_timePosition->setEnabled(false);

}
void LecteurAudio::clickOnList(QListWidgetItem * p)//double click
{
            defineSource(p);
            m_mediaObject->play();
}
void LecteurAudio::updateUi()
{
    if(!G_joueur)
    {
        connect(m_mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
        connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
        this, SLOT(stateChanged(Phonon::State, Phonon::State)));
        connect(m_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
        this, SLOT(sourceChanged(const Phonon::MediaSource &)));
        connect(G_clientServeur, SIGNAL(linkAdded(Liaison *)), this, SLOT(emettreEtat(Liaison *)));
    }
    else
    {
           playerWidget();
    }
}

void LecteurAudio::defineSource(QListWidgetItem * p)
{

     m_formerItemFile = m_currentItemFile;
     m_currentItemFile = p;

     m_mutex.lock();
     setSource(m_pathList[m_songList->row(m_currentItemFile)]);
     m_mutex.unlock();
     m_mediaObject->clear();
     m_mediaObject->setCurrentSource(*m_currentSource);
     emettreCommande(nouveauMorceau, p->text());
}


void LecteurAudio::tick(qint64 time)
{
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
     if((!G_joueur) && ((time>m_time+(FACTOR_WAIT*m_mediaObject->tickInterval()))||(time<m_time)))
     {
          emettreCommande(nouvellePositionMorceau, "", time);
          emitCurrentState();
     }
     m_time = time;
     m_timerDisplay->display(displayTime.toString("mm:ss"));
}

void LecteurAudio::sourceChanged(const Phonon::MediaSource &source)
{
     qDebug() << "sourceChanged" << source.fileName() << m_currentItemFile;


     if(m_currentItemFile==NULL)
     {
         return;
     }

     m_titleDisplay->setText(source.fileName().right(source.fileName().length()-source.fileName().lastIndexOf("/")-1));



     if(m_formerItemFile!=NULL)
     {
         QFont ft = m_formerItemFile->font();
         ft.setBold(false);
         m_formerItemFile->setFont(ft);
     }
     QFont ft2 = m_currentItemFile->font();
     ft2.setBold(true);
     m_currentItemFile->setFont(ft2);

     m_timerDisplay->display("00:00");
}
void LecteurAudio::emitCurrentState()
{
    switch (m_mediaObject->state())
    {
        case Phonon::ErrorState:
            break;
        case Phonon::PlayingState:
                emettreCommande(lectureMorceau,"",m_time);
                break;
        case Phonon::StoppedState:
                emettreCommande(arretMorceau);
                break;
        case Phonon::PausedState:
                emettreCommande(pauseMorceau);
                break;
        case Phonon::BufferingState:
        case Phonon::LoadingState:
        default:
                qDebug() << "default State";
             break;
    }
}

void LecteurAudio::stateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    if(G_joueur)
        return;

     switch (newState)
     {
         case Phonon::ErrorState:
            qDebug() << "error State" << m_mediaObject->errorString();
             break;
         case Phonon::PlayingState:
             qDebug() << "playing State";
                 m_playAction->setEnabled(false);
                 m_playAction->setChecked(true);
                 m_pauseAction->setEnabled(true);
                 m_pauseAction->setChecked(false);
                 m_stopAction->setEnabled(true);
                 m_stopAction->setChecked(false);
                 m_timePosition->setEnabled(true);
                 emettreCommande(lectureMorceau,"",m_time);
                 break;
         case Phonon::StoppedState:
                     emettreCommande(arretMorceau);
                     m_stopAction->setEnabled(false);
                     m_playAction->setEnabled(true);
                     m_playAction->setChecked(false);
                     m_pauseAction->setChecked(false);
                     m_pauseAction->setEnabled(false);
                     m_timePosition->setEnabled(true);
                     m_timerDisplay->display("00:00");
                 break;
         case Phonon::PausedState:
                 qDebug() << "paused State";
                 /// @attention Workaround for phonon issue with some files. Pause state is sometime called
                     if(m_stopAction->isChecked())
                     {

                     }
                     else if(!m_pauseAction->isChecked())
                     {
                         //isAboutToFinish();
                         m_mediaObject->play();
                     }
                     else if(m_pauseAction->isChecked())
                     {
                         emettreCommande(pauseMorceau);
                         m_stopAction->setEnabled(true);
                         m_playAction->setEnabled(true);
                         m_playAction->setChecked(false);
                         m_pauseAction->setEnabled(false);
                    }
                 break;
         case Phonon::BufferingState:
                 qDebug() << "buffering State";
                 break;
         case Phonon::LoadingState:
                 qDebug() << "Phonon::LoadingState State";
              break;
         default:
                 qDebug() << "default State";
              break;
     }

}
void LecteurAudio::playerWidget()
{
    m_titleDisplay->setToolTip(tr("No songs"));
    m_commandWidget->hide();
    QWidget *separateur3 = new QWidget();
    separateur3->setFixedHeight(2);
    layoutPrincipal->addWidget(separateur3);
    setFixedHeight(66);

}
void LecteurAudio::updatePlayingMode()
{
    QAction* tmp = static_cast<QAction*>(sender());
    if((tmp==m_loopAction)&&(m_loopAction->isChecked())&&(m_uniqueAction->isChecked()))
    {
        m_uniqueAction->setChecked(false);
    }
    if((tmp==m_uniqueAction)&&(m_loopAction->isChecked())&&(m_uniqueAction->isChecked()))
    {
        m_loopAction->setChecked(false);
    }


    if(m_loopAction->isChecked())
    {
        m_currentPlayingMode=LOOP;
    }
    else if(m_uniqueAction->isChecked())
    {
        m_currentPlayingMode=UNIQUE;
    }
    else if((!m_uniqueAction->isChecked())&&(!m_loopAction->isChecked()))
    {
        m_currentPlayingMode=NEXT;
    }
}
void LecteurAudio::addFiles()
{

    QStringList listeFichiers = QFileDialog::getOpenFileNames(this, tr("Add song"), m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString(), tr("Audio files (*.wav *.mp2 *.mp3 *.ogg *.flac)"));

        if (listeFichiers.isEmpty())
                return;
        QFileInfo fileinfo(listeFichiers[0]);
        m_preferences->registerValue("MusicDirectoryGM",fileinfo.absolutePath());


        while (!listeFichiers.isEmpty())
        {
                QString fichier = listeFichiers.takeFirst();

                QFileInfo fi(fichier);
                QString titre = fi.fileName();
                if (m_pathList.isEmpty())
                {

                        emettreCommande(nouveauMorceau, titre);
                        // On active tous les boutons



                }
                QListWidgetItem *morceau = new QListWidgetItem(titre, m_songList);
                morceau->setToolTip(fichier);
                m_pathList.append(fichier);
        }
}
void LecteurAudio::removeFile()
{
    m_mutex.lock();
/// @todo test to perform with several computers. The sound must stop on both side.
        QList<QListWidgetItem *> titreSelectionne = m_songList->selectedItems();
        if (titreSelectionne.isEmpty())
        {
                return;
        }

        foreach(QListWidgetItem * tmp, titreSelectionne)
        {
            m_pathList.removeAt(m_songList->row(tmp));
            if(m_currentItemFile == tmp)
            {
                delete m_currentSource;
                m_currentSource = NULL;
                delete tmp;
                tmp = NULL;
                m_currentItemFile = NULL;
                m_mediaObject->clear();
                isAboutToFinish();
            }
            if(tmp!=NULL)
            {
                delete tmp;
            }
        }
        m_mutex.unlock();
}

void LecteurAudio::isAboutToFinish()
{
    qDebug() << m_currentPlayingMode << "joueur" << G_joueur;
        if(G_joueur)
        {
            return;
        }
        else if (m_currentPlayingMode==UNIQUE)
        {
                emettreCommande(arretMorceau);
                //m_mediaObject->stop();
                qDebug() << "finDeTitreSlot" << "lecture unique";
        }
        else if (m_currentPlayingMode==NEXT)
        {
            int position= m_songList->row(m_currentItemFile);
            if(position<0)
            {
                m_mediaObject->stop();
                return;
            }

            position++;
            if(position>=m_pathList.size())
            {
                position =0;
            }
            m_mediaObject->stop();
            m_formerItemFile = m_currentItemFile;
            m_currentItemFile = m_songList->item(position);

            // m_currentSource = new Phonon::MediaSource();
            setSource(m_pathList[position]);
            m_mediaObject->clear();
            m_mediaObject->setCurrentSource(*m_currentSource);
            emettreCommande(nouveauMorceau, m_currentItemFile->text());

            qDebug() << "Changement de titre 2" << m_pathList[position] << m_currentItemFile->text();
            m_mediaObject->play();

        }
        else if(m_currentPlayingMode==LOOP)
        {
            m_mediaObject->setCurrentSource(*m_currentSource);
            m_mediaObject->play();
        }
}

void LecteurAudio::emettreCommande(actionMusique action, QString nomFichier, quint64 position, Liaison * link)
{

        int p;
        quint16 tailleNomFichier;

        char *donnees = NULL;

        enteteMessage uneEntete;
        uneEntete.categorie = musique;
        uneEntete.action = action;
        switch(action)
        {
                case nouveauMorceau :
                        uneEntete.tailleDonnees = sizeof(quint16) + nomFichier.size()*sizeof(QChar);
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        p = sizeof(enteteMessage);
                        tailleNomFichier = nomFichier.size();
                        memcpy(&(donnees[p]), &tailleNomFichier, sizeof(quint16));
                        p+=sizeof(quint16);
                        memcpy(&(donnees[p]), nomFichier.data(), tailleNomFichier*sizeof(QChar));
                        break;
                case lectureMorceau :
                        uneEntete.tailleDonnees = 0;
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        break;

                case pauseMorceau :
                        uneEntete.tailleDonnees = 0;
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        break;

                case arretMorceau :
                        uneEntete.tailleDonnees = 0;
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        break;

                case nouvellePositionMorceau :
                        uneEntete.tailleDonnees = sizeof(quint32);
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        // Ajout de la position dans le message
                        memcpy(&(donnees[sizeof(enteteMessage)]), &position, sizeof(quint32));
                        break;

                default :
                        qWarning() <<(tr("Unknown command send to users"));
                        break;
        }
        if(donnees == NULL)
            return;

        memcpy(donnees, &uneEntete, sizeof(enteteMessage));

        if (link == NULL)
        {
            emettre(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage));
        }
        else
        {
            link->emissionDonnees(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage));
        }

        delete[] donnees;
}


void LecteurAudio::emettreEtat(Liaison * link)
{
        if(m_titleDisplay->text().isEmpty())
        {
           emettreCommande(nouveauMorceau, "", 0, link);
        }
        else
        {
            emettreCommande(nouveauMorceau, m_titleDisplay->text(), 0, link);
            switch(m_mediaObject->state())
            {
                case Phonon::PausedState:
                    emettreCommande(pauseMorceau,"", 0, link);
                    emettreCommande(nouvellePositionMorceau, "", m_time, link);
                    break;
                case Phonon::StoppedState:
                    emettreCommande(arretMorceau, "",0, link);
                    break;
            case Phonon::PlayingState :
                    emettreCommande(pauseMorceau,"", 0, link);
                    emettreCommande(nouvellePositionMorceau, "", m_time, link);
                    emettreCommande(lectureMorceau, "", 0, link);
                    break;
                default :
                    break;
            }
        }
}

qreal LecteurAudio::volume()
{
        return audioOutput->volume();
}



void LecteurAudio::pplay()
{
    m_mediaObject->play();
}

void LecteurAudio::ppause()
{
    m_mediaObject->pause();
}

void LecteurAudio::pstop()
{
    m_mediaObject->stop();
}
void LecteurAudio::pselectNewFile(QString file)
{
        m_currentFile = file;
        qDebug() << " file = " << m_currentFile;

    if(m_currentFile.isEmpty())
    {
        qDebug() << " file vide= " << m_currentFile;
        m_titleDisplay->clear();
        m_titleDisplay->setToolTip(tr("No songs"));
        m_mediaObject->stop();
    }
    else
    {


        QString path(tr("%1/%2").arg(m_preferences->value("MusicDirectoryPlayer",QDir::homePath()).toString()).arg(m_currentFile));

        QFileInfo fileInfo(path);
        if (!fileInfo.exists())
        {
            //qDebug() << " file n'existe pas = " << path;
            QPalette palette(m_titleDisplay->palette());
            palette.setColor(QPalette::Normal, QPalette::Text, Qt::red);
            m_titleDisplay->setEchoMode(QLineEdit::Normal);
            m_titleDisplay->setPalette(palette);
            m_titleDisplay->setText(tr("%1 : file can not be found or opened").arg(path));
            m_titleDisplay->setToolTip(tr("File can not be found or opened : %1").arg(path));
        }
        else
        {
            qDebug() << " file existe = " << path;
            //m_currentSource = new Phonon::MediaSource(path);
            setSource(path);
            m_mediaObject->setCurrentSource(*m_currentSource);
            m_titleDisplay->setEchoMode(QLineEdit::Password);
            QPalette palette(m_titleDisplay->palette());
            palette.setColor(QPalette::Normal, QPalette::Text, Qt::black);
            m_titleDisplay->setPalette(palette);
            m_titleDisplay->setText(m_currentFile);
        }



    }

}
void LecteurAudio::pseek(quint32 position)
{
    qDebug()<< "is Seekable" << m_mediaObject->isSeekable()<< m_mediaObject->state();
    if(m_mediaObject->isSeekable())
    {
            m_mediaObject->seek(position);
    }
}

void LecteurAudio::pChangeDirectory()
{
    QString tmp = QFileDialog::getExistingDirectory(0 , tr("Select the songs directory"), m_preferences->value("MusicDirectoryPlayer",QDir::homePath()).toString(),
            QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
        m_preferences->registerValue("MusicDirectoryPlayer",tmp);

}
void LecteurAudio::selectionHasChanged()
{
     QList<QListWidgetItem *> selection = m_songList->selectedItems();


    if(selection.size()>0)
    {
        switch(m_mediaObject->state())
        {
            case Phonon::PlayingState:

            break;
            case Phonon::BufferingState:
            case Phonon::PausedState:
            case Phonon::StoppedState:
            case Phonon::LoadingState:
            case Phonon::ErrorState:
                    defineSource(selection[0]);
            break;
       }
       m_deleteAction->setEnabled(true);




    }
    else
    {
        m_deleteAction->setEnabled(false);

    }
}
void LecteurAudio::setSource(QString path)
{
   /* if(m_currentSource!=NULL)
    {
        delete m_currentSource;
        m_currentSource=NULL;
    }*/

    qDebug()<<  "path set source" <<path;
    if(!path.isEmpty())
    {
        m_currentSource = new Phonon::MediaSource(path);
    }

}
