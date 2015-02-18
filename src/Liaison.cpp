/************************************************************************
*   Copyright (C) 2007 by Romain Campioni                               *
*   Copyright (C) 2009 by Renaud Guezennec                              *
*   Copyright (C) 2011 by Joseph Boudou                                 *
*                                                                       *
*     http://www.rolisteam.org/                                         *
*                                                                       *
*   Rolisteam is free software; you can redistribute it and/or modify   *
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

#include <QApplication>
#include <QTcpSocket>

#include "Liaison.h"

#include "Carte.h"
#include "CarteFenetre.h"
#include "ClientServeur.h"
#include "DessinPerso.h"
#include "Image.h"
#include "persons.h"
#include "playersList.h"
#include "receiveevent.h"

#ifndef NULL_PLAYER
#include "LecteurAudio.h"
#endif

#include "types.h"
#include "variablesGlobales.h"







Liaison::Liaison(QTcpSocket *socket)
    : QObject(NULL),m_mainWindow(NULL)
{
    m_mainWindow = MainWindow::getInstance();
    m_networkManager = m_mainWindow->getNetWorkManager();
    m_socketTcp = socket;
    receptionEnCours = false;
#ifndef NULL_PLAYER
    m_audioPlayer = LecteurAudio::getInstance();
#endif

    setSocket(socket);
    //makeSignalConnection();



    // Si l'ordi local est un client, on ajoute tt de suite la liaison a la liste, et on connecte le signal d'emission des donnees
    // Le serveur effectue cette operation a la fin de la procedure de connexion du client
    if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        m_networkManager->ajouterLiaison(this);
    }
}
void Liaison::initialize()
{
    makeSignalConnection();
}

Liaison::~Liaison()
{
    if(NULL!=m_socketTcp)
    {
        delete m_socketTcp;
        m_socketTcp=NULL;
    }
}
void Liaison::makeSignalConnection()
{


    connect(m_socketTcp, SIGNAL(readyRead()),
            this, SLOT(reception()));
    connect(m_socketTcp, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(erreurDeConnexion(QAbstractSocket::SocketError)));
    connect(m_socketTcp, SIGNAL(disconnected()),
            this, SLOT(p_disconnect()));
}

void Liaison::setMainWindow(MainWindow* mainWindow)
{
    m_mainWindow = mainWindow;
}


void Liaison::erreurDeConnexion(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur);
    if(NULL==m_socketTcp)
    {
        return;
    }
    qWarning("Une erreur réseau est survenue : %s", qPrintable(m_socketTcp->errorString()));
}

void Liaison::p_disconnect()
{
    qWarning("Emit disconneted signal : s");
    emit disconnected(this);
}


void Liaison::emissionDonnees(char *donnees, quint32 taille, Liaison *sauf)
{
    if(NULL==m_socketTcp)
    {
        return;
    }
    if (sauf != this)
    {
        // Emission des donnees
        int t = m_socketTcp->write(donnees, taille);

        if (t < 0)
        {
            qWarning() << "Tranmission error :" << m_socketTcp->errorString();
        }
      /*  else
            qDebug() << "Emit - data size : " << t << taille;*/
    }
}


void Liaison::reception()
{
    if(NULL==m_socketTcp)
    {
        return;
    }
    quint32 lu=0;


    static quint64 readDataSum=0;
    static quint64 readDataSum2=0;

    static int laps=0;
    while (m_socketTcp->bytesAvailable())
    {
        // S'il s'agit d'un nouveau message
        if (!receptionEnCours)
        {
            m_time.start();
            m_time2.start();
            // On recupere l'entete du message
            m_socketTcp->read((char *)&entete, sizeof(NetworkMessageHeader));
            // Reservation du tampon
            tampon = new char[entete.dataSize];
            // Initialisation du restant a lire
            restant = entete.dataSize;
        }

        // Lecture des donnees a partir du dernier point
        lu = m_socketTcp->read(&(tampon[entete.dataSize-restant]), restant);

        // Si toutes les donnees n'ont pu etre lues
        if (lu < restant)
        {
            restant-=lu;
            receptionEnCours = true;

            readDataSum+=lu;
            readDataSum2+=lu;

            if(laps%10)
            {
                int millisec = m_time.elapsed();
                int millisec2 = m_time2.elapsed();

                if(millisec>0)
                {
                    qDebug() << (readDataSum/(millisec/1000.0)) << "ko/s firstMethod" << m_time.secsTo(QTime::currentTime());
                    qDebug() << (readDataSum2*(1000.0/millisec2)) << "ko/s second Method";
                }

                laps = 0;
                m_time2.restart();
                readDataSum2=0;
            }
            laps++;


        }

        // Si toutes les donnees ont pu etre lu
        else
        {

            // Envoie la notification sur la mainWindows
            QApplication::alert(m_mainWindow);

            // Send event
            if (ReceiveEvent::hasReceiverFor(entete.category, entete.action))
            {
                ReceiveEvent * event = new ReceiveEvent(entete, tampon, this);
                event->postToReceiver();
            }


            switch((categorieAction)(entete.category))
            {
                case connexion :
                    receptionMessageConnexion();
                    break;
                case joueur :
                    receptionMessageJoueur();
                    break;
                case persoJoueur :
                    receptionMessagePersoJoueur();
                    break;
                case persoNonJoueur :
                    receptionMessagePersoNonJoueur();
                    break;
                case personnage :
                    receptionMessagePersonnage();
                    break;
                case dessin :
                    receptionMessageDessin();
                    break;
                case plan :
                    receptionMessagePlan();
                    break;
                case image :
                    receptionMessageImage();
                    break;
                case discussion :
                    receptionMessageDiscussion();
                    break;
                case musique :
                    receptionMessageMusique();
                    break;
                case parametres :
                    receptionMessageParametres();
                    break;
                default :
                    qWarning()<< tr("Unknown network package received!");
                    receptionEnCours = false;
                    return;
            }
            // On libere le tampon
            delete[] tampon;
            // On indique qu'aucun message n'est en cours de reception
            receptionEnCours = false;
        }

    } // Fin du while

}

/********************************************************************/
/* Reception d'un message de categorie Joueur                       */
/********************************************************************/
void Liaison::receptionMessageConnexion()
{
    // Le serveur indique que la processus de connexion vient de se terminer
    if (entete.action == finProcessusConnexion)
    {
        // Message sur le log utilisateur
        MainWindow::notifyUser(tr("End of the connection process"));
        // On met a jour l'espace de travail
        m_mainWindow->mettreAJourEspaceTravail();
    }
}

/********************************************************************/
/* Reception d'un message de categorie Joueur                       */
/********************************************************************/
void Liaison::receptionMessageJoueur()
{

    // Un nouveau joueur vient de se connecter au serveur (serveur uniquement)
    if (entete.action == connexionJoueur)
    {
        // Ajout de la liaison a la liste
        m_networkManager->ajouterLiaison(this);

        // On indique au nouveau joueur que le processus de connexion vient d'arriver a son terme
        NetworkMessageHeader uneEntete;
        uneEntete.category = connexion;
        uneEntete.action = finProcessusConnexion;
        uneEntete.dataSize = 0;

        emissionDonnees((char *)&uneEntete, sizeof(NetworkMessageHeader));
    }

    // L'hote demande au soft local d'ajouter un joueur a la liste des utilisateurs
    else if (entete.action == ajouterJoueur)
    {
    }

    // Suppression d'un joueur qui vient de se deconnecter
    else if (entete.action == supprimerJoueur)
    {
        faireSuivreMessage(false);
    }

    // L'hote demande au soft local de changer le nom d'un joueur
    else if (entete.action == changerNomJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement de nom a l'ensemble des clients
        faireSuivreMessage(false);
    }

    // L'hote demande au soft local de changer la couleur d'un joueur
    else if (entete.action == changerCouleurJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement de couleur a l'ensemble des clients
        faireSuivreMessage(false);
    }

    else
    {
        qWarning("Action Joueur inconnue (receptionMessageJoueur - Liaison.cpp)");
        return;
    }
}

/********************************************************************/
/* Reception d'un message de categorie PersoJoueur                  */
/********************************************************************/
void Liaison::receptionMessagePersoJoueur()
{
    int p = 0;

    if (entete.action == ajouterPersoJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre la creation de PJ aux autres clients
        faireSuivreMessage(false);
    }

    else if (entete.action == supprimerPersoJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre la suppression de PJ aux autres clients
        faireSuivreMessage(false);
    }

    else if (entete.action == afficherMasquerPersoJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre l'affichage/masquage de PJ aux autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere l'identifiant du PJ
        quint8 tailleIdPj;
        memcpy(&tailleIdPj, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPj = new QChar[tailleIdPj];
        memcpy(tableauIdPj, &(tampon[p]), tailleIdPj*sizeof(QChar));
        p+=tailleIdPj*sizeof(QChar);
        QString idPerso(tableauIdPj, tailleIdPj);
        // On recupere l'info affichage/masquage
        quint8 affichage;
        memcpy(&affichage, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);

        // On recherche la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche un message d'erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'une demande d'affichage/masquage de PJ (receptionMessagePersoJoueur - Liaison.cpp)");

        // Si la carte a ete trouvee
        else
        {
            // On affiche/masque le PJ sur la carte
            carte->affichageDuPj(idPerso, affichage);
        }

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
        delete[] tableauIdPj;
    }

    else if (entete.action == changerTaillePersoJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement de taille de PJ aux autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);

        // extract character Id
        quint8 charIdPlanSize;
        memcpy(&charIdPlanSize, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *charIdPlan = new QChar[charIdPlanSize];
        memcpy(charIdPlan, &(tampon[p]), charIdPlanSize*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);

        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere la nouvelle taille des PJ
        quint8 taillePj;
        memcpy(&taillePj, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);

        // On recherche la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche un message d'erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'une demande de changement de taille des PJ (receptionMessagePersoJoueur - Liaison.cpp)");

        // Si la carte a ete trouvee
        else
        {
            // On met a jour les PJ sur la carte
            QString tmp(charIdPlan,charIdPlanSize);
            carte->selectCharacter(tmp);
            carte->changerTaillePjCarte(taillePj + 11);
        }

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
    }

    else if (entete.action == changerNomPersoJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement de nom a l'ensemble des clients
        faireSuivreMessage(false);
    }

    else if (entete.action == changerCouleurPersoJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement de couleur a l'ensemble des clients
        faireSuivreMessage(false);
    }

    else
    {
        qWarning("Action persoJoueur inconnue (receptionMessagePersoJoueur - Liaison.cpp)");
        return;
    }
}

/********************************************************************/
/* Reception d'un message de categorie PersoNonJoueur               */
/********************************************************************/
void Liaison::receptionMessagePersoNonJoueur()
{
    int p = 0;

    if (entete.action == ajouterPersoNonJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre l'ajout de PNJ a l'ensemble des autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche une erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un PNJ a ajouter (receptionMessagePersoNonJoueur - Liaison.cpp)");
        // Sinon on ajoute le PNJ a la carte
        else
            extrairePersonnage(carte, &(tampon[p]));

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
    }

    else if (entete.action == supprimerPersoNonJoueur)
    {
        // Si l'ordinateur local est le serveur, on fait suivre l'ajout de PNJ a l'ensemble des autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere l'identifiant du PNJ
        quint8 tailleIdPnj;
        memcpy(&tailleIdPnj, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPnj = new QChar[tailleIdPnj];
        memcpy(tableauIdPnj, &(tampon[p]), tailleIdPnj*sizeof(QChar));
        p+=tailleIdPnj*sizeof(QChar);
        QString idPerso(tableauIdPnj, tailleIdPnj);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche une erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un PNJ a supprimer (receptionMessagePersoNonJoueur - Liaison.cpp)");
        // Sinon on supprime le PNJ de la carte
        else
            carte->eraseCharacter(idPerso);

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
        delete[] tableauIdPnj;
    }

    else
    {
        qWarning("Action persoNonJoueur inconnue (receptionMessagePnj - Liaison.cpp)");
        return;
    }
}

/********************************************************************/
/* Reception d'un message de categorie Personnage                   */
/********************************************************************/
void Liaison::receptionMessagePersonnage()
{
    int p = 0;

    if (entete.action == ajouterListePerso)
    {
        // Si l'ordinateur local est le serveur on fait suivre la liste de persos a l'ensemble des autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant
        quint8 tailleId;
        memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauId = new QChar[tailleId];
        memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        QString idPlan(tableauId, tailleId);
        // On recupere le nombre de personnages presents dans le message
        quint16 nbrPersonnages;
        memcpy(&nbrPersonnages, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche une erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'une liste de personnages a ajouter (receptionMessagePersonnage - Liaison.cpp)");
        // Sinon on recupere l'ensemble des personnages et on les ajoute a la carte
        else
            for (int i=0; i<nbrPersonnages; i++)
                p += extrairePersonnage(carte, &(tampon[p]));

        // Liberation de la memoire allouee
        delete[] tableauId;
    }

    else if (entete.action == deplacerPerso)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le deplacement de perso a l'ensemble des autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere l'identifiant du personnage
        quint8 tailleIdPerso;
        memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPerso = new QChar[tailleIdPerso];
        memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
        p+=tailleIdPerso*sizeof(QChar);
        QString idPerso(tableauIdPerso, tailleIdPerso);
        // On recupere le nbr de points du deplacement
                    qint32 nbrPoints;
        memcpy(&nbrPoints, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);
        // On recupere les points du deplacement
        QList<QPoint> listeDeplacement;
        QPoint point;
        for (int i=0; i<nbrPoints; i++)
        {
            qint16 pointX, pointY;
            memcpy(&pointX, &(tampon[p]), sizeof(qint16));
            p+=sizeof(qint16);
            memcpy(&pointY, &(tampon[p]), sizeof(qint16));
            p+=sizeof(qint16);
            point = QPoint(pointX, pointY);
            listeDeplacement.append(point);
        }

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche une erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un deplacement de personnage (receptionMessagePersonnage - Liaison.cpp)");
        // Sinon lance le deplacement du personnage
        else
            carte->lancerDeplacementPersonnage(idPerso, listeDeplacement);

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
        delete[] tableauIdPerso;
    }

    else if (entete.action == changerEtatPerso)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement d'etat a l'ensemble des autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere l'identifiant du perso
        quint8 tailleIdPerso;
        memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPerso = new QChar[tailleIdPerso];
        memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
        p+=tailleIdPerso*sizeof(QChar);
        QString idPerso(tableauIdPerso, tailleIdPerso);
        // On recupere le numero d'etat de sante
        quint16 numEtat;
        memcpy(&numEtat, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche une erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un changement d'etat de perso (receptionMessagePersonnage - Liaison.cpp)");
        // Sinon on change l'etat du perso
        else
        {
            DessinPerso *perso = carte->trouverPersonnage(idPerso);

            if (NULL!=perso)
                perso->changerEtatDeSante(numEtat);
            else
                qWarning("Personnage introuvable a la reception d'un changement d'etat de perso (receptionMessagePersonnage - Liaison.cpp)");
        }

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
        delete[] tableauIdPerso;
    }

    else if (entete.action == changerOrientationPerso)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement d'orientation a l'ensemble des autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere l'identifiant du perso
        quint8 tailleIdPerso;
        memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPerso = new QChar[tailleIdPerso];
        memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
        p+=tailleIdPerso*sizeof(QChar);
        QString idPerso(tableauIdPerso, tailleIdPerso);
        // On recupere la nouvelle orientation
        qint16 pointX, pointY;
        memcpy(&pointX, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&pointY, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        QPoint orientation(pointX, pointY);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche une erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un changement d'orientation (receptionMessagePersonnage - Liaison.cpp)");
        // Sinon on change l'orientation du personnage
        else
        {
            DessinPerso *perso = carte->trouverPersonnage(idPerso);

            if (NULL!=perso)
                perso->nouvelleOrientation(orientation);
            else
                qWarning("Personnage introuvable a la reception d'un changement d'orientation (receptionMessagePersonnage - Liaison.cpp)");
        }

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
        delete[] tableauIdPerso;
    }

    else if (entete.action == afficherMasquerOrientationPerso)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le changement d'affichage d'orientation a l'ensemble des autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere l'identifiant du perso
        quint8 tailleIdPerso;
        memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPerso = new QChar[tailleIdPerso];
        memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
        p+=tailleIdPerso*sizeof(QChar);
        QString idPerso(tableauIdPerso, tailleIdPerso);
        // On recupere l'info d'affichage/masquage d'orientation
        quint8 afficheOrientation;
        memcpy(&afficheOrientation, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche une erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un affichage/masquage d'orientation (receptionMessagePersonnage - Liaison.cpp)");
        // Sinon on change l'etat de l'affichage de l'orientation du personnage
        else
        {
            DessinPerso *perso = carte->trouverPersonnage(idPerso);

            if (NULL!=perso)
                perso->afficherOrientation(afficheOrientation);
            else
                qWarning("Personnage introuvable a la reception d'un affichage/masquage d'orientation (receptionMessagePersonnage - Liaison.cpp)");
        }

        // Liberation de la memoire allouee
        delete[] tableauIdPlan;
        delete[] tableauIdPerso;
    }
}

/********************************************************************/
/* Reception d'un message de categorie Dessin                       */
/********************************************************************/
void Liaison::receptionMessageDessin()
{
    int p = 0;

    if (entete.action == traceCrayon)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le trace a l'ensemble des clients
        // (y compris celui qui l'a emis, afin de conserver la coherence des plans)
        faireSuivreMessage(true);

        // On recupere l'identifiant du joueur
        quint8 tailleIdJoueur;
        memcpy(&tailleIdJoueur, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdJoueur = new QChar[tailleIdJoueur];
        memcpy(tableauIdJoueur, &(tampon[p]), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        QString idJoueur(tableauIdJoueur, tailleIdJoueur);
        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);
        // On recupere le nombre de points
                    qint32 nbrPoints;
        memcpy(&nbrPoints, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);
        // On recupere les points du trace
        QList<QPoint> listePoints;
        QPoint point;
        for (int i=0; i<nbrPoints; i++)
        {
            qint16 pointX, pointY;
            memcpy(&pointX, &(tampon[p]), sizeof(qint16));
            p+=sizeof(qint16);
            memcpy(&pointY, &(tampon[p]), sizeof(qint16));
            p+=sizeof(qint16);
            point = QPoint(pointX, pointY);
            listePoints.append(point);
        }
        // On recupere le rectangle a rafraichir
        qint16 zoneX, zoneY, zoneW, zoneH;
        memcpy(&zoneX, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneY, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneW, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneH, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        QRect zoneARafraichir(zoneX, zoneY, zoneW, zoneH);
        // On recupere le diametre
        quint8 diametre;
        memcpy(&diametre, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        // On recupere la couleur
        couleurSelectionee couleur;
        memcpy(&couleur, &(tampon[p]), sizeof(couleurSelectionee));
        p+=sizeof(couleurSelectionee);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche un message d'erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un trace de crayon (receptionMessageDessin - Liaison.cpp)");
        // Sinon on demande le dessin du trace
        else
            carte->dessinerTraceCrayon(&listePoints, zoneARafraichir, diametre, couleur, idJoueur==G_idJoueurLocal);

        // Liberation de la memoire allouee
        delete[] tableauIdJoueur;
        delete[] tableauIdPlan;
    }

    else if (entete.action == traceTexte)
    {
        // Si l'ordinateur local est le serveur, on fait suivre le trace a l'ensemble des clients
        // (y compris celui qui l'a emis, afin de conserver la coherence des plans)
        faireSuivreMessage(true);

        // On recupere l'identifiant
        quint8 tailleId;
        memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauId = new QChar[tailleId];
        memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        QString idPlan(tableauId, tailleId);
        // On recupere le texte
        quint16 tailleTexte;
        memcpy(&tailleTexte, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        QChar *tableauTexte = new QChar[tailleTexte];
        memcpy(tableauTexte, &(tampon[p]), tailleTexte*sizeof(QChar));
        p+=tailleTexte*sizeof(QChar);
        QString texte(tableauTexte, tailleTexte);
        // On recupere le point d'arrivee
        qint16 positionSourisX, positionSourisY;
        memcpy(&positionSourisX, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&positionSourisY, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        QPoint positionSouris(positionSourisX, positionSourisY);
        // On recupere le rectangle a rafraichir
        qint16 zoneX, zoneY, zoneW, zoneH;
        memcpy(&zoneX, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneY, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneW, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneH, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        QRect zoneARafraichir(zoneX, zoneY, zoneW, zoneH);
        // On recupere la couleur
        couleurSelectionee couleur;
        memcpy(&couleur, &(tampon[p]), sizeof(couleurSelectionee));
        p+=sizeof(couleurSelectionee);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche un message d'erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un trace texte (receptionMessageDessin - Liaison.cpp)");
        // Sinon on demande le dessin du trace
        else
            carte->dessinerTraceTexte(texte, positionSouris, zoneARafraichir, couleur);

        // Liberation de la memoire allouee
        delete[] tableauId;
        delete[] tableauTexte;
    }

    else if (entete.action == traceMain)
    {
    }

    // Tous les autres outils
    else
    {
        // Si l'ordinateur local est le serveur, on fait suivre le trace a l'ensemble des clients
        // (y compris celui qui l'a emis, afin de conserver la coherence des plans)
        faireSuivreMessage(true);

        // On recupere l'identifiant
        quint8 tailleId;
        memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauId = new QChar[tailleId];
        memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        QString idPlan(tableauId, tailleId);
        // On recupere le point de depart
        qint16 departX, departY;
        memcpy(&departX, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&departY, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        QPoint pointDepart(departX, departY);
        // On recupere le point d'arrivee
        qint16 arriveeX, arriveeY;
        memcpy(&arriveeX, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&arriveeY, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        QPoint pointArrivee(arriveeX, arriveeY);
        // On recupere le rectangle a rafraichir
        qint16 zoneX, zoneY, zoneW, zoneH;
        memcpy(&zoneX, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneY, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneW, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        memcpy(&zoneH, &(tampon[p]), sizeof(qint16));
        p+=sizeof(qint16);
        QRect zoneARafraichir(zoneX, zoneY, zoneW, zoneH);
        // On recupere le diametre
        quint8 diametre;
        memcpy(&diametre, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        // On recupere la couleur
        couleurSelectionee couleur;
        memcpy(&couleur, &(tampon[p]), sizeof(couleurSelectionee));
        p+=sizeof(couleurSelectionee);

        // Recherche de la carte concernee
        Carte *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche un message d'erreur
        if (!carte)
            qWarning("Carte introuvable a la reception d'un trace general (receptionMessageDessin - Liaison.cpp)");
        // Sinon on demande le dessin du trace
        else
            carte->dessinerTraceGeneral((actionDessin)(entete.action), pointDepart, pointArrivee, zoneARafraichir, diametre, couleur);

        // Liberation de la memoire allouee
        delete[] tableauId;
    }
}

/********************************************************************/
/* Reception d'un message de categorie Plan                         */
/********************************************************************/
void Liaison::receptionMessagePlan()
{
    int p = 0;

    // L'hote demande a l'ordinateur local de creer un nouveau plan vide
    if (entete.action == nouveauPlanVide)
    {
        // Si l'ordinateur local est le serveur, on renvoie le messages aux clients
        faireSuivreMessage(false);

        // On recupere le titre
        quint16 tailleTitre;
        memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        QChar *tableauTitre = new QChar[tailleTitre];
        memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
        p+=tailleTitre*sizeof(QChar);
        QString titre(tableauTitre, tailleTitre);
        // On recupere l'identifiant
        quint8 tailleId;
        memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauId = new QChar[tailleId];
        memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        QString idPlan(tableauId, tailleId);
        // On recupere la couleur
        QRgb rgb;
        memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
        p+=sizeof(QRgb);
        QColor couleur(rgb);
        // On recupere les dimensions du plan
        quint16 largeur, hauteur;
        memcpy(&largeur, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&hauteur, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        // On recupere la taille des PJ
        quint8 taillePj;
        memcpy(&taillePj, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        // Permission analyse
        quint8 permission;
        memcpy(&permission,&(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);

        // On cree la carte
        QSize mapSize(largeur,hauteur);
        m_mainWindow->buildNewMap(titre, idPlan,couleur, mapSize,(Carte::PermissionMode)permission);

        // Message sur le log utilisateur
        MainWindow::notifyUser(tr("New map: %1").arg(titre));

        // Liberation de la memoire allouee
        delete[] tableauTitre;
        delete[] tableauId;
    }

    // L'hote demande a l'ordinateur local de creer un plan a partir de l'image passee dans le message
    else if (entete.action == chargerPlan)
    {
        // Si l'ordinateur local est le serveur, on renvoie le messages aux clients
        faireSuivreMessage(false);

        // On recupere le titre
        quint16 tailleTitre;
        memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        QChar *tableauTitre = new QChar[tailleTitre];
        memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
        p+=tailleTitre*sizeof(QChar);
        QString titre(tableauTitre, tailleTitre);
        // On recupere l'identifiant
        quint8 tailleId;
        memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauId = new QChar[tailleId];
        memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        QString idPlan(tableauId, tailleId);
        // On recupere la taille des PJ
        quint8 taillePj;
        memcpy(&taillePj, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);

        quint8 permission;
        memcpy(&permission,&(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);

        qDebug() << permission << "Permission chargerPlan" << taillePj << tailleId << tableauTitre;

        // On recupere l'information "plan masque a l'ouverture?"
        quint8 masquerPlan;
        memcpy(&masquerPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        // On recupere l'image
        quint32 tailleImage;
        memcpy(&tailleImage, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);
        QByteArray byteArray(tailleImage, 0);
        memcpy(byteArray.data(), &(tampon[p]), tailleImage);
        p+=tailleImage;

        // Creation de l'image
        QImage image;
        bool ok = image.loadFromData(byteArray, "jpeg");
        if (!ok)
            qWarning("Probleme de decompression de l'image (receptionMessagePlan - Liaison.cpp)");

        // Creation de la carte
        Carte *carte = new Carte(idPlan, &image, masquerPlan);
        carte->setPermissionMode((Carte::PermissionMode)permission);
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte,m_mainWindow);
        // Ajout de la carte au workspace
        m_mainWindow->ajouterCarte(carteFenetre, titre);

        // Message sur le log utilisateur
        MainWindow::notifyUser(tr("Receiving map: %1").arg(titre));

        // Liberation de la memoire allouee
        delete[] tableauTitre;
        delete[] tableauId;
    }

    // Reception d'un plan complet
    else if (entete.action == importerPlanComplet)
    {
        // Si l'ordinateur local est le serveur, on renvoie le messages aux clients
        faireSuivreMessage(false);

        // On recupere le titre
        quint16 tailleTitre;
        memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        QChar *tableauTitre = new QChar[tailleTitre];
        memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
        p+=tailleTitre*sizeof(QChar);
        QString titre(tableauTitre, tailleTitre);
        // On recupere l'identifiant
        quint8 tailleId;
        memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauId = new QChar[tailleId];
        memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        QString idPlan(tableauId, tailleId);
        // On recupere la taille des PJ
        quint8 taillePj;
        memcpy(&taillePj, &(tampon[p]), sizeof(quint8));


        p+=sizeof(quint8);
        quint8 permission;
        memcpy(&permission,&(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        qDebug() << permission << "Permission importerPlanComplet" << taillePj << tailleId << *tableauTitre;

        // On recupere l'intensite de la couche alpha
        quint8 intensiteAlpha;
        memcpy(&intensiteAlpha, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        // On recupere le fond original
        quint32 tailleFondOriginal;
        memcpy(&tailleFondOriginal, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);
        QByteArray baFondOriginal(tailleFondOriginal, 0);
        memcpy(baFondOriginal.data(), &(tampon[p]), tailleFondOriginal);
        p+=tailleFondOriginal;
        // On recupere le fond
        quint32 tailleFond;
        memcpy(&tailleFond, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);
        QByteArray baFond(tailleFond, 0);
        memcpy(baFond.data(), &(tampon[p]), tailleFond);
        p+=tailleFond;
        // On recupere le fond original
        quint32 tailleAlpha;
        memcpy(&tailleAlpha, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);
        QByteArray baAlpha(tailleAlpha, 0);
        memcpy(baAlpha.data(), &(tampon[p]), tailleAlpha);
        p+=tailleAlpha;

        bool ok;
        // Creation de l'image de fond original
        QImage fondOriginal;
        ok = fondOriginal.loadFromData(baFondOriginal, "jpeg");
        if (!ok)
            qWarning("Probleme de decompression du fond original (receptionMessagePlan - Liaison.cpp)");
        // Creation de l'image de fond
        QImage fond;
        ok = fond.loadFromData(baFond, "jpeg");
        if (!ok)
            qWarning("Probleme de decompression du fond (receptionMessagePlan - Liaison.cpp)");
        // Creation de la couche alpha
        QImage alpha;
        ok = alpha.loadFromData(baAlpha, "jpeg");
        if (!ok)
            qWarning("Probleme de decompression de la couche alpha (receptionMessagePlan - Liaison.cpp)");

        // Creation de la carte
        Carte *carte = new Carte(idPlan, &fondOriginal, &fond, &alpha);
        carte->setPermissionMode((Carte::PermissionMode)permission);
        // On adapte la couche alpha a la nature de l'utilisateur local (MJ ou joueur)
        carte->adapterCoucheAlpha(intensiteAlpha);
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte,m_mainWindow);
        // Ajout de la carte au workspace
        m_mainWindow->ajouterCarte(carteFenetre, titre);

        // Message sur le log utilisateur
        MainWindow::notifyUser(tr("Receiving map: %1").arg(titre));

        // Liberation de la memoire allouee
        delete[] tableauTitre;
        delete[] tableauId;
    }

    // Demande de fermeture d'un plan
    else if (entete.action == fermerPlan)
    {
        // Si l'ordinateur local est le serveur, on fait suivre la fermeture de plan aux autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdPlan;
        memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdPlan = new QChar[tailleIdPlan];
        memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
        p+=tailleIdPlan*sizeof(QChar);
        QString idPlan(tableauIdPlan, tailleIdPlan);


        m_mainWindow->removeMapFromId(idPlan);
        delete[] tableauIdPlan;
    }

    else
    {
        qWarning()<< "Unknown map Action (receptionMessagePlan - Liaison.cpp)";
        return;
    }
}

/********************************************************************/
/* Reception d'un message de categorie Image                        */
/********************************************************************/
void Liaison::receptionMessageImage()
{
    int p = 0;

    // L'hote demande a l'ordinateur local de creer l'image passee dans le message
    if (entete.action == chargerImage)
    {
        // Si l'ordinateur local est le serveur, on renvoie le messages aux clients
        faireSuivreMessage(false);

        // On recupere le titre
        quint16 tailleTitre;
        memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        QChar *tableauTitre = new QChar[tailleTitre];
        memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
        p+=tailleTitre*sizeof(QChar);
        QString titre(tableauTitre, tailleTitre);
        // On recupere l'identifiant de l'image
        quint8 tailleIdImage;
        memcpy(&tailleIdImage, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdImage = new QChar[tailleIdImage];
        memcpy(tableauIdImage, &(tampon[p]), tailleIdImage*sizeof(QChar));
        p+=tailleIdImage*sizeof(QChar);
        QString idImage(tableauIdImage, tailleIdImage);
        // On recupere l'identifiant du joueur
        quint8 tailleIdJoueur;
        memcpy(&tailleIdJoueur, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdJoueur = new QChar[tailleIdJoueur];
        memcpy(tableauIdJoueur, &(tampon[p]), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        QString idJoueur(tableauIdJoueur, tailleIdJoueur);
        // On recupere l'image
        quint32 tailleImage;
        memcpy(&tailleImage, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);
        QByteArray byteArray(tailleImage, 0);
        memcpy(byteArray.data(), &(tampon[p]), tailleImage);
        p+=tailleImage;

        qDebug() << "taille donnee" << tailleTitre*sizeof(QChar)+sizeof(quint16)
                                      +tailleIdImage*sizeof(QChar)+sizeof(quint8)
                                      +sizeof(quint8) +tailleIdJoueur*sizeof(QChar)
                                      +tailleImage+sizeof(quint32) << "taille image:"<< tailleImage << tailleTitre << tailleIdJoueur << tailleIdImage;

        // Creation de l'image
        QImage *img = new QImage;
        bool ok = img->loadFromData(byteArray, "jpg");
        if (!ok)
            qWarning("Cannot read received image (receptionMessageImage - Liaison.cpp)");

        // Creation de l'Image
        Image *imageFenetre = new Image(m_mainWindow,idImage, idJoueur, img);
        // Ajout de la carte au workspace
        m_mainWindow->ajouterImage(imageFenetre, titre);


        qDebug() << "titre" << titre;
        MainWindow::notifyUser(tr("Receiving picture: %1").arg(titre.left(titre.size()-QString(tr(" (Picture)")).size())));

        // Liberation de la memoire allouee
        delete[] tableauTitre;
        delete[] tableauIdImage;
        delete[] tableauIdJoueur;
    }

    // Demande de fermeture d'une image
    else if (entete.action == fermerImage)
    {
        // Si l'ordinateur local est le serveur, on fait suivre la fermeture de l'image aux autres clients
        faireSuivreMessage(false);

        // On recupere l'identifiant de la carte
        quint8 tailleIdImage;
        memcpy(&tailleIdImage, &(tampon[p]), sizeof(quint8));
        p+=sizeof(quint8);
        QChar *tableauIdImage = new QChar[tailleIdImage];
        memcpy(tableauIdImage, &(tampon[p]), tailleIdImage*sizeof(QChar));
        p+=tailleIdImage*sizeof(QChar);
        QString idImage(tableauIdImage, tailleIdImage);


        m_mainWindow->removePictureFromId(idImage);

        delete[] tableauIdImage;
    }

    else
    {
        qWarning("Action image inconnue (receptionMessageImage - Liaison.cpp)");
        return;
    }
}

/********************************************************************/
/* Reception d'un message de categorie Discussion                   */
/********************************************************************/
void Liaison::receptionMessageDiscussion()
{
    // All should be done by ChatList.
}

/********************************************************************/
/* Reception d'un message de categorie Musique                      */
/********************************************************************/
void Liaison::receptionMessageMusique()
{
#ifndef NULL_PLAYER
    int p = 0;

    // Si l'ordinateur local est le serveur, on fait suivre la commande du lecteur audio aux autres clients
    faireSuivreMessage(false);
    switch(entete.action)
    {
        case nouveauMorceau:
            qDebug() << " nouveau morceau";
            break;
        case lectureMorceau:
            qDebug() << " lecture morceau";
            break;
        case pauseMorceau:
            qDebug() << " pause morceau";
            break;
        case arretMorceau:
            qDebug() << " arret morceau";
            break;
        case nouvellePositionMorceau:
            qDebug() << " nouvelle position morceau";
            break;
    }


    // L'hote demande a l'ordinateur local de charger la musique dont le nom est passe en parametre
    if (entete.action == nouveauMorceau)
    {
        // On recupere le nom du fichier
        quint16 tailleNomFichier;
        memcpy(&tailleNomFichier, &(tampon[p]), sizeof(quint16));
        p+=sizeof(quint16);
        QChar *tableauNomFichier = new QChar[tailleNomFichier];
        memcpy(tableauNomFichier, &(tampon[p]), tailleNomFichier*sizeof(QChar));
        p+=tailleNomFichier*sizeof(QChar);
        QString nomFichier(tableauNomFichier, tailleNomFichier);

        // On charge le nouveau fichier dans le lecteur
                    qDebug() << nomFichier <<endl;
        m_audioPlayer->pselectNewFile(nomFichier);

        // Liberation de la memoire allouee
        delete[] tableauNomFichier;
    }

    // Demande de lecture de la musique
    else if (entete.action == lectureMorceau)
    {
        // On demande au lecteur audio de mettre la lecture en pause
        qDebug() << " pplay";
        m_audioPlayer->pplay();
    }

    // Demande de mise en pause de la lecture
    else if (entete.action == pauseMorceau)
    {
        // On demande au lecteur audio de mettre la lecture en pause
        qDebug() << " ppause";
        m_audioPlayer->ppause();
    }

    // Demande d'arret de la lecture
    else if (entete.action == arretMorceau)
    {
        // On demande au lecteur audio de mettre la lecture en pause
         qDebug() << " pstop";
        m_audioPlayer->pstop();
    }

    // Demande de changement de position de la lecture
    else if (entete.action == nouvellePositionMorceau)
    {
        // On recupere la nouvelle position
        quint32 newPosition;
        memcpy(&newPosition, &(tampon[p]), sizeof(quint32));
        p+=sizeof(quint32);

        qDebug() << "new position" << newPosition;
        m_audioPlayer->pseek(newPosition);
    }

    else
    {
        qWarning("Unknown music Action (receptionMessageMusique - Liaison.cpp)");
        return;
    }
#endif
}


void Liaison::receptionMessageParametres()
{
    if (entete.action == NetMsg::AddFeatureAction)
    {
        faireSuivreMessage(false);
    }
}


void Liaison::faireSuivreMessage(bool tous)
{
    // Uniquement si l'ordinateur local est le serveur
    if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        char *donnees = new char[entete.dataSize + sizeof(NetworkMessageHeader)];
        // Recopie de l'entete
        memcpy(donnees, &entete, sizeof(NetworkMessageHeader));
        // Recopie du corps du message
        memcpy(&(donnees[sizeof(NetworkMessageHeader)]), tampon, entete.dataSize);
        if (tous)
        {
            emettre(donnees, entete.dataSize + sizeof(NetworkMessageHeader));
        }
        else
        {
            emettre(donnees, entete.dataSize + sizeof(NetworkMessageHeader), this);
        }
        delete[] donnees;
    }
}

/********************************************************************/
/* Extrait un personnage d'un message et l'ajoute a la carte passee */
/* en parametre                                                     */
/********************************************************************/
int Liaison::extrairePersonnage(Carte *carte, char *tampon)
{
    int p=0;

    // On recupere le nom
    quint16 tailleNom;
    memcpy(&tailleNom, &(tampon[p]), sizeof(quint16));
    p+=sizeof(quint16);
    QChar *tableauNom = new QChar[tailleNom];
    memcpy(tableauNom, &(tampon[p]), tailleNom*sizeof(QChar));
    p+=tailleNom*sizeof(QChar);
    QString nomPerso(tableauNom, tailleNom);
    // On recupere l'identifiant du perso
    quint8 tailleIdPerso;
    memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
    p+=sizeof(quint8);
    QChar *tableauIdPerso = new QChar[tailleIdPerso];
    memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
    p+=tailleIdPerso*sizeof(QChar);
    QString idPerso(tableauIdPerso, tailleIdPerso);
    // On recupere le type du personnage
    quint8 type;
    memcpy(&type, &(tampon[p]), sizeof(quint8));
    p+=sizeof(quint8);
    DessinPerso::typePersonnage typePerso = (DessinPerso::typePersonnage)type;
    // On recupere le numero de PNJ
    quint8 numeroPnj;
    memcpy(&numeroPnj, &(tampon[p]), sizeof(quint8));
    p+=sizeof(quint8);
    // On recupere le diametre
    quint8 diametre;
    memcpy(&diametre, &(tampon[p]), sizeof(quint8));
    p+=sizeof(quint8);
    // On recupere la couleur
    QRgb rgb;
    memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
    p+=sizeof(QRgb);
    QColor couleurPerso(rgb);
    // On recupere le point central du perso
    qint16 centreX, centreY;
    memcpy(&centreX, &(tampon[p]), sizeof(qint16));
    p+=sizeof(qint16);
    memcpy(&centreY, &(tampon[p]), sizeof(qint16));
    p+=sizeof(qint16);
    QPoint centre(centreX, centreY);
    // On recupere l'orientation du perso
    qint16 orientationX, orientationY;
    memcpy(&orientationX, &(tampon[p]), sizeof(qint16));
    p+=sizeof(qint16);
    memcpy(&orientationY, &(tampon[p]), sizeof(qint16));
    p+=sizeof(qint16);
    QPoint orientation(orientationX, orientationY);
    // On recupere la couleur de l'etat de sante
    memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
    p+=sizeof(QRgb);
    QColor couleurEtat(rgb);
    // On recupere le nom de l'etat
    quint16 tailleEtat;
    memcpy(&tailleEtat, &(tampon[p]), sizeof(quint16));
    p+=sizeof(quint16);
    QChar *tableauEtat = new QChar[tailleEtat];
    memcpy(tableauEtat, &(tampon[p]), tailleEtat*sizeof(QChar));
    p+=tailleEtat*sizeof(QChar);
    QString nomEtat(tableauEtat, tailleEtat);
    // On recupere le numero de l'etat de sante
    quint16 numEtat;
    memcpy(&numEtat, &(tampon[p]), sizeof(quint16));
    p+=sizeof(quint16);
    // On recupere l'information visible/cache
    quint8 visible;
    memcpy(&visible, &(tampon[p]), sizeof(quint8));
    p+=sizeof(quint8);
    // On recupere l'information orientation affichee/masquee
    quint8 orientationAffichee;
    memcpy(&orientationAffichee, &(tampon[p]), sizeof(quint8));
    p+=sizeof(quint8);

    // Creation du PNJ dans la carte
    DessinPerso *pnj = new DessinPerso(carte, idPerso, nomPerso, couleurPerso, diametre, centre, typePerso, numeroPnj);
    // S'il doit etre visible, on l'affiche (s'il s'agit d'un PNJ et que l'utilisateur est le MJ, alors on affiche automatiquement le perso)
    if (visible || (typePerso == DessinPerso::pnj && !G_joueur))
        pnj->afficherPerso();
    // On m.a.j l'orientation
    pnj->nouvelleOrientation(orientation);
    // Affichage de l'orientation si besoin
    if (orientationAffichee)
        pnj->afficheOuMasqueOrientation();
    // M.a.j de l'etat de sante du personnage
    DessinPerso::etatDeSante sante;
    sante.couleurEtat = couleurEtat;
    sante.nomEtat = nomEtat;
    pnj->nouvelEtatDeSante(sante, numEtat);
    // Affiche ou masque le PNJ selon qu'il se trouve sur une zone masquee ou pas
    carte->afficheOuMasquePnj(pnj);

    // Liberation de la memoire allouee
    delete[] tableauNom;
    delete[] tableauIdPerso;
    delete[] tableauEtat;

    // On renvoie le nbr d'octets lus
    return p;
}
void Liaison::disconnectAndClose()
{
    if(NULL!=m_socketTcp)
    {
        m_socketTcp->close();
        delete m_socketTcp;
        m_socketTcp=NULL;
    }
}
void Liaison::setSocket(QTcpSocket* socket, bool makeConnection)
{
    m_socketTcp=socket;
    if(makeConnection)
    {
        makeSignalConnection();
    }

}
