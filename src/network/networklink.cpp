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

#include "network/networklink.h"

#include "map/Carte.h"
#include "map/bipmapwindow.h"
#include "network/networkmanager.h"
#include "map/DessinPerso.h"
#include "Image.h"
#include "persons.h"
#include "playersList.h"
#include "network/receiveevent.h"

#ifndef NULL_PLAYER
#include "audioPlayer.h"
#endif

#include "types.h"
#include "variablesGlobales.h"

NetworkLink::NetworkLink(QTcpSocket *socket)
    : QObject(NULL),m_mainWindow(NULL)
{
    m_mainWindow = MainWindow::getInstance();
    m_networkManager = m_mainWindow->getNetWorkManager();
    m_socketTcp = socket;
    receptionEnCours = false;
	ReceiveEvent::registerNetworkReceiver(NetMsg::PictureCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::MapCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::NPCCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::DrawCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::ConnectionCategory,m_mainWindow);
#ifndef NULL_PLAYER
    m_audioPlayer = AudioPlayer::getInstance();
    ReceiveEvent::registerNetworkReceiver(NetMsg::MusicCategory,m_audioPlayer);
#endif

    setSocket(socket);
    //makeSignalConnection();



	// Si l'ordi local est un client, on ajoute tt de suite la NetworkLink a la liste, et on connecte le signal d'emission des donnees
    // Le serveur effectue cette operation a la fin de la procedure de connexion du client
    if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
		m_networkManager->ajouterNetworkLink(this);
    }
}
void NetworkLink::initialize()
{
    makeSignalConnection();
}

NetworkLink::~NetworkLink()
{
    if(NULL!=m_socketTcp)
    {
        delete m_socketTcp;
        m_socketTcp=NULL;
    }
}
void NetworkLink::makeSignalConnection()
{


    connect(m_socketTcp, SIGNAL(readyRead()),
            this, SLOT(reception()));
    connect(m_socketTcp, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(erreurDeConnexion(QAbstractSocket::SocketError)));
    connect(m_socketTcp, SIGNAL(disconnected()),
            this, SLOT(p_disconnect()));
}

void NetworkLink::setMainWindow(MainWindow* mainWindow)
{
    m_mainWindow = mainWindow;
}


void NetworkLink::erreurDeConnexion(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur);
    if(NULL==m_socketTcp)
    {
        return;
    }
    qWarning("Une erreur réseau est survenue : %s", qPrintable(m_socketTcp->errorString()));
}

void NetworkLink::p_disconnect()
{
    qWarning("Emit disconneted signal : s");
    emit disconnected(this);
}


void NetworkLink::emissionDonnees(char *donnees, quint32 taille, NetworkLink *sauf)
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


void NetworkLink::reception()
{
    if(NULL==m_socketTcp)
    {
        return;
    }
    quint32 lu=0;




    static int laps=0;
    while (m_socketTcp->bytesAvailable())
    {
        // S'il s'agit d'un nouveau message
        if (!receptionEnCours)
        {
            // On recupere l'entete du message
            m_socketTcp->read((char *)&entete, sizeof(NetworkMessageHeader));
            // Reservation du tampon
            tampon = new char[entete.dataSize];
            // Initialisation du restant a lire
            restant = entete.dataSize;
            emit readDataReceived(entete.dataSize,entete.dataSize);

        }

        // Lecture des donnees a partir du dernier point
        lu = m_socketTcp->read(&(tampon[entete.dataSize-restant]), restant);

        // Si toutes les donnees n'ont pu etre lues
        if (lu < restant)
        {
            restant-=lu;
            receptionEnCours = true;
            emit readDataReceived(restant,entete.dataSize);
            //qDebug("Reception par morceau");
        }

        // Si toutes les donnees ont pu etre lu
        else
        {

            // Envoie la notification sur la mainWindows
            QApplication::alert(m_mainWindow);
            emit readDataReceived(0,0);

            // Send event
            if (ReceiveEvent::hasReceiverFor(entete.category, entete.action))
            {
                ReceiveEvent * event = new ReceiveEvent(entete, tampon, this);
                event->postToReceiver();
            }
            //if(m_receiverMap.contains((NetMsg::Category)entete.category))
            if (ReceiveEvent::hasNetworkReceiverFor((NetMsg::Category)entete.category))
            {
                NetworkMessageReader data(entete,tampon);
                NetWorkReceiver* tmp = ReceiveEvent::getNetWorkReceiverFor((NetMsg::Category)entete.category);
                switch(tmp->processMessage(&data))
                {
                 case NetWorkReceiver::AllExceptMe:
                    faireSuivreMessage(false);
                    break;
                case NetWorkReceiver::ALL:
                    faireSuivreMessage(true);
                    break;
                case NetWorkReceiver::NONE:
                    break;
                }
            }

            switch((categorieAction)(entete.category))
            {
                case joueur :
                    receptionMessageJoueur();
                    break;
                case persoJoueur :
                    receptionMessagePersoJoueur();
                    break;
                case parametres :
                    receptionMessageParametres();
                    break;
                default :
                    qWarning()<< tr("Unknown network package received!");
                    //receptionEnCours = false;
                    break;
            }
            // On libere le tampon
            delete[] tampon;
            // On indique qu'aucun message n'est en cours de reception
            receptionEnCours = false;
        }

    } // Fin du while

}
void NetworkLink::receptionMessageJoueur()
{

    // Un nouveau joueur vient de se connecter au serveur (serveur uniquement)
    if (entete.action == connexionJoueur)
    {
		// Ajout de la NetworkLink a la liste
		m_networkManager->ajouterNetworkLink(this);

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
		qWarning("Action Joueur inconnue (receptionMessageJoueur - NetworkLink.cpp)");
        return;
    }
}

/********************************************************************/
/* Reception d'un message de categorie PersoJoueur                  */
/********************************************************************/
void NetworkLink::receptionMessagePersoJoueur()
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
        Map *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche un message d'erreur
        if (!carte)
			qWarning("Carte introuvable a la reception d'une demande d'affichage/masquage de PJ (receptionMessagePersoJoueur - NetworkLink.cpp)");

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
        Map *carte = m_mainWindow->trouverCarte(idPlan);
        // Si la carte est introuvable on affiche un message d'erreur
        if (!carte)
			qWarning("Carte introuvable a la reception d'une demande de changement de taille des PJ (receptionMessagePersoJoueur - NetworkLink.cpp)");

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
		qWarning("Action persoJoueur inconnue (receptionMessagePersoJoueur - NetworkLink.cpp)");
        return;
    }
}
void NetworkLink::receptionMessageParametres()
{
    if (entete.action == NetMsg::AddFeatureAction)
    {
        faireSuivreMessage(false);
    }
}


void NetworkLink::faireSuivreMessage(bool tous)
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
void NetworkLink::disconnectAndClose()
{
    if(NULL!=m_socketTcp)
    {
        m_socketTcp->close();
        delete m_socketTcp;
        m_socketTcp=NULL;
    }
}
void NetworkLink::setSocket(QTcpSocket* socket, bool makeConnection)
{
    m_socketTcp=socket;
    if(makeConnection)
    {
        makeSignalConnection();
    }

}
void NetworkLink::insertNetWortReceiver(NetWorkReceiver* receiver,NetMsg::Category cat)
{
    m_receiverMap.insert(cat,receiver);
}
