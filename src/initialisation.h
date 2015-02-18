/*************************************************************************
 *     Copyright (C) 2007 by Romain Campioni                             *
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


#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <QColor>

/**
 * @brief Hold session parameters
 * @note
 * This class comes from a struct. A usefull constructor and destructor that read and write from/to
 * a file has been haded. But all members are still publics.
 */
class Initialisation
{
    public:
        /**
         * @brief Constructor
         * Get last session parameters from file or set defaults.
         */
        Initialisation();

        /**
         * @brief Destructor
         * Write parameters to file.
         */
        ~Initialisation();


        QString versionApplication;    // Version de l'application ayant cree le fichier d'initialisation
        QString nomUtilisateur;        // Nom de l'utilisateur tel qu'indique dans la fenetre de connexion
        QColor couleurUtilisateur;     // Couleur choisie par l'utilisateur
        bool joueur;                   // True si l'utilisateur est joueur, false s'il est MJ
        bool client;                   // True si l'utilisateur est client, false s'il est serveur
        QString ipServeur;             // Adresse IP du serveur (renseigne par un utilisateur client)
        quint16 portServeur;           // Port du serveur (renseigne par un utilisateur client)
        QString portClient;            // Port de connexion pour les clients (renseigne par un utilisateur serveur)
        QString dossierMusiquesMj;     // Dossier de chargement des musiques pour le MJ
        QString dossierMusiquesJoueur; // Dossier de chargement des musiques pour les joueurs
        QString dossierImages;         // Dossier de chargement des images
        QString dossierPlans;          // Dossier de chargement et de sauvegarde des plans
        QString dossierScenarii;       // Dossier de chargement et de sauvegarde des scenarii
        QString dossierNotes;          // Dossier de chargement des notes
        QString dossierTchats;         // Dossier de sauvegarde des tchats
        QColor couleurPersonnelle[16]; // Couleurs personnelles definies par l'utilisateur
        int niveauVolume;              // Volume du lecteur audio

    private:
        QString m_confdir;
        QString m_filename;
};

#endif
