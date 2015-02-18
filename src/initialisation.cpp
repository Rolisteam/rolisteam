/*************************************************************************
 *	   Copyright (C) 2007 by Romain Campioni                             *
 *	   Copyright (C) 2009 by Renaud Guezennec                            *
 *     Copyright (C) 2010 by Joseph Boudou                               *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include "initialisation.h"

#include <QDir>

#include "constantesGlobales.h"

Initialisation::Initialisation()
{
    m_confdir = QString("%1/.%2").arg(QDir::homePath(), QString(NOM_APPLICATION));
    m_filename = QString("%1/%2.ini").arg(m_confdir, QString(NOM_APPLICATION));

    QFile file(m_filename);
    m_version = QObject::tr("Unknown");
    #ifdef VERSION_MINOR
        #ifdef VERSION_MAJOR
            #ifdef VERSION_MIDDLE
                m_version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
            #endif
        #endif
    #endif
    
    // Si le fichier d'initialisation existe, on le charge
    if (file.exists() && file.open(QIODevice::ReadOnly))
    {
        // On cree un flux de donnees rattache au fichier
        QDataStream fluxFichier(&file);
        
        // On recupere la version de l'application
        // This field is ignored actually.
        fluxFichier >> m_version;
        // ...le nom de l'utilisateur
        fluxFichier >> nomUtilisateur;
        // ...la couleur de l'utilisateur
        fluxFichier >> couleurUtilisateur;
        // ...la nature de l'utilisateur (joueur ou MJ)
        fluxFichier >> joueur;
        // ...la nature de l'ordinateur local
        fluxFichier >> client;
        // ...l'adresse IP du serveur
        fluxFichier >> ipServeur;
        // ...le port du serveur
        fluxFichier >> portServeur;
        if (portServeur < 1024)
            portServeur = 6660;
        // ...le port de connexion pour les clients
        fluxFichier >> portClient;
        if (portClient.toInt() < 1024)
            portClient = QString("6660");
        // ...le chemin pour les musiques
        fluxFichier >> dossierMusiquesMj;
        // ...le chemin pour les musiques des joueurs
        fluxFichier >> dossierMusiquesJoueur;
        // ...le chemin pour les images
        fluxFichier >> dossierImages;
        // ...le chemin pour les plans
        fluxFichier >> dossierPlans;
        // ...le chemin pour les scenarii
        fluxFichier >> dossierScenarii;
        // ...le chemin pour les notes
        fluxFichier >> dossierNotes;
        // ...le chemin pour les tchats
        fluxFichier >> dossierTchats;
        // ...les couleurs personnelles
        for (int i=0; i<16; i++)
            fluxFichier >> couleurPersonnelle[i];
        // ...le volume du lecteur audio
        fluxFichier >> niveauVolume;
        // Fermeture du fichier
        file.close();
    }

    // If we don't have the file, we set default values
    else
    {

        nomUtilisateur        = QString();
        couleurUtilisateur    = QColor();
        couleurUtilisateur.setHsv(qrand()%360, qrand()%200 + 56, qrand()%190 + 50);
        joueur                = true;
        client                = true;
        ipServeur             = QString();
        portServeur           = 6660;
        portClient            = QString().setNum(portServeur);
        dossierMusiquesMj     = m_confdir + "/audio";
        dossierMusiquesJoueur = dossierMusiquesMj;
        dossierImages         = m_confdir + "/images";
        dossierPlans          = m_confdir + "/plans";
        dossierScenarii       = m_confdir + "/scenarii";
        dossierNotes          = m_confdir + "/notes";
        dossierTchats         = m_confdir + "/tchat";
        for (int i=0; i<16; i++)
            couleurPersonnelle[i] = QColor(i*16, i*16, i*16);
        niveauVolume          = 100;
    }
}

Initialisation::~Initialisation()
{
    QDir  dir(m_confdir);
    if (!dir.exists())
        dir.mkpath(m_confdir);
    QFile file(m_filename);

    // Ouverture du fichier en ecriture seule
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("Unable to open parameters file for writing. No parameters will be saved.");
        return;
    }

    // On cree un flux de donnees rattache au fichier
    QDataStream fluxFichier(&file);
    // On sauvegarde la version de l'application
    // We force the actual version string.
    fluxFichier << m_version;
    // ...le nom de l'utilisateur
    fluxFichier << nomUtilisateur;
    // ...la couleur de l'utilisateur
    fluxFichier << couleurUtilisateur;
    // ...la nature de l'utilisateur (joueur ou MJ)
    fluxFichier << joueur;
    // ...la nature de l'ordinateur local
    fluxFichier << client;
    // ...l'adresse IP du serveur
    fluxFichier << ipServeur;
    // ...le port du serveur
    fluxFichier << portServeur;
    // ...le port de connexion pour les clients
    fluxFichier << portClient;
    // ...le chemin pour les musiques
    fluxFichier << dossierMusiquesMj;
    // ...le chemin pour les musiques des joueurs
    fluxFichier << dossierMusiquesJoueur;
    // ...le chemin pour les images
    fluxFichier << dossierImages;
    // ...le chemin pour les plans
    fluxFichier << dossierPlans;
    // ...le chemin pour les scenarii
    fluxFichier << dossierScenarii;
    // ...le chemin pour les notes
    fluxFichier << dossierNotes;
    // ...le chemin pour les tchats
    fluxFichier << dossierTchats;
    // ...les couleurs personnelles
    for (int i=0; i<16; i++)
            fluxFichier << couleurPersonnelle[i];
    // ...le volume du lecteur audio
	fluxFichier << niveauVolume;

    // Fermeture du fichier
    file.close();
}   
