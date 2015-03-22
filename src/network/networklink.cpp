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

#include "map/map.h"
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
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterPlayerCategory,m_mainWindow);
#ifndef NULL_PLAYER
    m_audioPlayer = AudioPlayer::getInstance();
    ReceiveEvent::registerNetworkReceiver(NetMsg::MusicCategory,m_audioPlayer);
#endif

    setSocket(socket);

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


void NetworkLink::erreurDeConnexion(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur);
    if(NULL==m_socketTcp)
    {
        return;
    }
    qWarning() << tr("Network error occurs :") << m_socketTcp->errorString();
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
            NetworkMessageReader data(entete,tampon);
            if (ReceiveEvent::hasNetworkReceiverFor((NetMsg::Category)entete.category))
            {

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

            switch(data.category())
            {
                case NetMsg::PlayerCategory :
                    processPlayerMessage(&data);
                    break;
                case NetMsg::SetupCategory :
                    processSetupMessage(&data);
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
void NetworkLink::processPlayerMessage(NetworkMessageReader* msg)
{
    if(NetMsg::PlayerCategory==msg->category())
    {
        if(NetMsg::PlayerConnectionAction == msg->action())
        {
            m_networkManager->ajouterNetworkLink(this);

            // On indique au nouveau joueur que le processus de connexion vient d'arriver a son terme
            NetworkMessageHeader uneEntete;
            uneEntete.category = NetMsg::SetupCategory;
            uneEntete.action = NetMsg::EndConnectionAction;
            uneEntete.dataSize = 0;

            emissionDonnees((char *)&uneEntete, sizeof(NetworkMessageHeader));
        }
        else if(NetMsg::AddPlayerAction == msg->action())
        {

        }
        else if(NetMsg::DelPlayerAction == msg->action())
        {
            faireSuivreMessage(false);
        }
        else if(NetMsg::ChangePlayerNameAction == msg->action())
        {
           faireSuivreMessage(false);
        }
        else if(NetMsg::ChangePlayerColorAction == msg->action())
        {
           faireSuivreMessage(false);
        }
    }
}
void NetworkLink::processSetupMessage(NetworkMessageReader* msg)
{
    if (msg->action() == NetMsg::AddFeatureAction)
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
