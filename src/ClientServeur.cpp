/************************************************************************
*   Copyright (C) 2007 by Romain Campioni                               *
*   Copyright (C) 2009 by Renaud Guezennec                              *
*   Copyright (C) 2010 by Joseph Boudou                                 *
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


#include "ClientServeur.h"

#include <QTcpSocket>
#include <QMessageBox>

#include "Liaison.h"
#include "connectiondialog.h"
#include "initialisation.h"
#include "constantesGlobales.h"
#include "variablesGlobales.h"


/********************************************************************/
/* Variables globales utilisees par tous les elements de            */
/* l'application                                                    */
/********************************************************************/    

// True si l'utilisateur est un joueur, false s'il est MJ
bool G_joueur;
// True si l'ordinateur local est client, false s'il est serveur
bool G_client;
// Couleur du joueur local
QColor G_couleurJoueurLocal;





PlayerTransfer::PlayerTransfer(QString id,QString name,QColor color,bool gm)
    : m_id(id),m_name(name),m_color(color),m_GM(gm)
{

}


QString PlayerTransfer::name()
{
    return m_name;
}

QColor PlayerTransfer::color()
{
    return m_color;
}

bool PlayerTransfer::isGM()
{
    return m_GM;
}

QString PlayerTransfer::id()
{
    return m_id;
}










/********************
 * Global functions *
 ********************/

void emettre(char *donnees, quint32 taille, Liaison *sauf)
{
    G_clientServeur->emettreDonnees(donnees, taille, sauf);
}

void emettre(char *donnees, quint32 taille, int numeroLiaison)
{
    G_clientServeur->emettreDonnees(donnees, taille, numeroLiaison);
}


/********************************
 * Private non-member functions *
 ********************************/

void synchronizeInitialisation(const ConnectionConfigDialog & dialog)
{
    G_initialisation.nomUtilisateur     = dialog.getName();
    G_initialisation.couleurUtilisateur = G_couleurJoueurLocal = dialog.getColor();
    G_initialisation.joueur             = G_joueur             = !dialog.isGM();
    G_initialisation.client             = G_client             = !dialog.isServer();
    G_initialisation.ipServeur          = dialog.getHost();
    G_initialisation.portServeur        = dialog.getPort();
    G_initialisation.portClient.setNum(G_initialisation.portServeur);
}


void emettreIdentite(const QString & playerName)
{
    // Taille des donnees
    quint32 tailleCorps =
        // Taille du nom
        sizeof(quint16) + playerName.size()*sizeof(QChar) +
        // Taille de l'identifiant
        sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
        // Taille de la couleur
        sizeof(QRgb) +
        // Taille de la nature de l'utilisateur (MJ ou joueur)
        sizeof(quint8);

    // Buffer d'emission
    char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

    // Creation de l'entete du message
    enteteMessage *entete;
    entete = (enteteMessage *) donnees;
    entete->categorie = joueur;
    entete->action = connexionJoueur;
    entete->tailleDonnees = tailleCorps;

    // Creation du corps du message
    int p = sizeof(enteteMessage);
    // Ajout du nom
    quint16 tailleNom = playerName.size();
    memcpy(&(donnees[p]), &tailleNom, sizeof(quint16));
    p+=sizeof(quint16);
    memcpy(&(donnees[p]), playerName.data(), tailleNom*sizeof(QChar));
    p+=tailleNom*sizeof(QChar);
    // Ajout de l'identifiant
    quint8 tailleId = G_idJoueurLocal.size();
    memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
    p+=sizeof(quint8);
    memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleId*sizeof(QChar));
    p+=tailleId*sizeof(QChar);
    // Ajout de la couleur
    QRgb rgb = G_couleurJoueurLocal.rgb();
    memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
    p+=sizeof(QRgb);
    // Ajout de la nature de l'utilisateur (MJ ou joueur)
    quint8 mj = !G_joueur;
    memcpy(&(donnees[p]), &mj, sizeof(quint8));
    p+=sizeof(quint8);
    // Emission des donnees
    emettre(donnees, tailleCorps + sizeof(enteteMessage));
    // Liberation du buffer d'emission
    delete[] donnees;
}


/*****************
 * ClientServeur *
 *****************/

ClientServeur::ClientServeur()
: QObject(), m_server(NULL)
{
}


ClientServeur::~ClientServeur()
{
    // QObject should delete all for us.
}

PlayerTransfer* ClientServeur::currentUser()
{
       return m_user;
}

bool ClientServeur::configAndConnect()
{
    ConnectionConfigDialog configDialog(
    G_initialisation.nomUtilisateur, G_initialisation.couleurUtilisateur, !G_initialisation.joueur,
    G_initialisation.ipServeur, G_initialisation.portServeur, !G_initialisation.client);

    ConnectionWaitDialog   waitDialog;

    QMessageBox            errorDialog;
    errorDialog.setText(QObject::tr("Impossible de se connecter"));
    errorDialog.setIcon(QMessageBox::Warning);

    QTcpSocket * socket;

    bool cont = true;
    while (cont)
    {
        // If the user abort configDialog, we quit
        if (configDialog.exec() == QDialog::Rejected)
            return false;

        // Server setup
        if (configDialog.isServer())
        {
            if (m_server == NULL)
            {
                m_server = new QTcpServer(this);
                connect(m_server, SIGNAL(newConnection()), this, SLOT(nouveauClientConnecte()));
            }

            // Listen successed
            if (m_server->listen(QHostAddress::Any, configDialog.getPort()))
            {
                synchronizeInitialisation(configDialog);
                m_user = new PlayerTransfer(G_idJoueurLocal,configDialog.getName(),configDialog.getColor(),configDialog.isGM());


//                ecrireLogUtilisateur(tr("Serveur en place sur le port ") + G_initialisation.portServeur);
                cont = false;
            }

            // Listen failed
            else
            {
                errorDialog.setInformativeText(m_server->errorString());
                errorDialog.exec();
            }
        }

        // Client setup
        else
        {
            socket = waitDialog.connectTo(configDialog.getHost(), configDialog.getPort());

            // connect successed
            if (socket != NULL)
            {
                synchronizeInitialisation(configDialog);
                Liaison * link = new Liaison(socket, this);
                link->start();

                emettreIdentite(configDialog.getName());
                g_featuresList.sendThemAll();
                cont = false;
            }
            else
            {
                errorDialog.setInformativeText(waitDialog.getError());
                errorDialog.exec();
            }
        }
    }

    return true;
}


void ClientServeur::emettreDonnees(char *donnees, quint32 taille, Liaison *sauf)
{
    // Demande d'emission vers toutes les liaisons
    emit emissionDonnees(donnees, taille, sauf);
}


void ClientServeur::emettreDonnees(char *donnees, quint32 taille, int numeroLiaison)
{
    // Emission des donnees vers la liaison selectionnee
    liaisons[numeroLiaison]->emissionDonnees(donnees, taille);
}


void ClientServeur::ajouterLiaison(Liaison *liaison)
{
    liaisons.append(liaison);
}


void ClientServeur::nouveauClientConnecte()
{
    // Recuperation du socket lie a la demande de connexion
    QTcpSocket *socketTcp = m_server->nextPendingConnection();

    // Creation de la liaison qui va gerer le socket
    Liaison *liaison = new Liaison(socketTcp, this);
    // Mise en route de la liaison
    liaison->start();
}


void ClientServeur::finDeLiaison()
{
    // Si l'ordinateur local est un client
    if (G_client)
    {
        // On quitte l'application    
        G_mainWindow->quitterApplication(true);
    }

    // Si l'ordinateur local est le serveur
    else
    {
        bool trouve = false;
        int i;
        int n = liaisons.size();
        // On recherche le thread qui vient de se terminer
            for (i = 0; ((i<n) && (!trouve)); i++)
            if (liaisons[i]->isFinished())
                trouve = true;
        i--;
        
        if (!trouve)
        {
            qWarning("Un thread inconnu vient de se terminer (finDeLiaison - ClientServeur.cpp)");
            return;
        }
        
        // On supprime la liaison de la liste, apres l'avoir detruite
        delete liaisons[i];
        liaisons.removeAt(i);
        // Recuperation de l'identifiant du joueur correspondant a la liaison
        // (en tenant compte du fait que le 1er utilisateur est toujours le serveur)
        QString identifiant = G_listeUtilisateurs->indentifiantUtilisateur(i+1);
#ifndef NULL_PLAYER
        // Si l'utilisateur etait le MJ, on reinitialise le lecteur audio
            if (G_listeUtilisateurs->estUnMj(i+1))
            {
                LecteurAudio * lecteurAudio = LecteurAudio::getInstance();
                if(lecteurAudio==NULL)
                    lecteurAudio = LecteurAudio::getInstance();
                lecteurAudio->pselectNewFile("");
            }
#endif
        // Suppression de l'utilisateur dans la liste
        G_listeUtilisateurs->supprimerJoueur(identifiant);
        // On supprime le tchat associe
        G_mainWindow->supprimerTchat(identifiant);
        
        // On envoie un message a l'ensemble des clients

        // Taille des donnees
        quint32 tailleCorps =
            // Taille de l'identifiant
            sizeof(quint8) + identifiant.size()*sizeof(QChar);

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];
        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->tailleDonnees = tailleCorps;
        uneEntete->categorie = joueur;
        uneEntete->action = supprimerJoueur;
        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout de l'identifiant
        quint8 tailleId = identifiant.size();
        memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), identifiant.data(), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        
        // Emission de la demande de suppression du joueur a l'ensemble des clients
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;
    }
}
