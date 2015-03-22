/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
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


#ifndef NetworkLink_H
#define NetworkLink_H

#include <QtNetwork>
#include <QTcpSocket>

#include "network/networkmessage.h"
#include "mainwindow.h"

class Map;
#ifndef NULL_PLAYER
    class AudioPlayer;
#endif

/**
 * @brief The NetworkLink class
 */
class NetworkLink : public QObject
{
Q_OBJECT

public :
	NetworkLink(QTcpSocket *socket);
	~NetworkLink();

    void setSocket(QTcpSocket* socket, bool makeConnection = true);
    void disconnectAndClose();
    void initialize();
    void insertNetWortReceiver(NetWorkReceiver*,NetMsg::Category cat);
    void processPlayerMessage(NetworkMessageReader* msg);
    void processSetupMessage(NetworkMessageReader* msg);

signals:
	void disconnected(NetworkLink * link);
    void readDataReceived(quint64,quint64);

public slots :
	void emissionDonnees(char *donnees, quint32 taille, NetworkLink *sauf = 0);

private :
    void makeSignalConnection();
    void receptionMessageConnexion();
    void receptionMessageJoueur();
    void receptionMessagePersoJoueur();
    void receptionMessagePersoNonJoueur();
    //void receptionMessagePersonnage();
   // void receptionMessageDessin();
    void receptionMessageParametres();

    void postTo(QObject * obj) const;
    void faireSuivreMessage(bool tous);
    int extrairePersonnage(Map *carte, char *tampon);


    QTcpSocket* m_socketTcp;		 // Socket gere par le thread
    NetworkMessageHeader entete; // Contient l'entete du message en cours de reception
    bool receptionEnCours;		 // Indique si un message est actuellement en cours de reception
    char *tampon;				 // Tampon contenant le message en court de reconstitution
    quint32 restant;			 // Taille des donnees restant a receptionner
#ifndef NULL_PLAYER
    AudioPlayer* m_audioPlayer;
#endif
    MainWindow* m_mainWindow;
    NetworkManager* m_networkManager;
    QTime m_time;
    QTime m_time2;
    QMap<NetMsg::Category,NetWorkReceiver*> m_receiverMap;

private slots :
    void reception();
    void erreurDeConnexion(QAbstractSocket::SocketError);
    void p_disconnect();

};

#endif
