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


/********************************************************************/
/*                                                                  */
/* Chaque Liaison est un thread qui gere un socket, donc la         */
/* reception et l'emission des messages vers les clients ou le      */
/* serveur. En regle generale a la reception d'un message le thread */
/* regarde si l'ordinateur local est le serveur, si c'est le cas il */
/* fait suivre le message aux autres clients.                       */
/*                                                                  */
/********************************************************************/	


#ifndef LIAISON_H
#define LIAISON_H

#include <QtNetwork>
#include <QThread>
#include <QTcpSocket>

#include "Carte.h"
#include "types.h"
#ifndef NULL_PLAYER
#include "LecteurAudio.h"
#endif
class Liaison : public QThread
{
Q_OBJECT

public :
    Liaison(QTcpSocket *socket, QObject * parent = 0);
    ~Liaison();

public slots :
    void emissionDonnees(char *donnees, quint32 taille, Liaison *sauf = 0);

signals :

protected :
    void run();

private :
    void receptionMessageConnexion();
    void receptionMessageJoueur();
    void receptionMessagePersoJoueur();
    void receptionMessagePersoNonJoueur();
    void receptionMessagePersonnage();
    void receptionMessageDessin();
    void receptionMessagePlan();
    void receptionMessageImage();
    void receptionMessageDiscussion();
    void receptionMessageMusique();
    void receptionMessageParametres();
    void faireSuivreMessage(bool tous);
    void emettreUtilisateur(utilisateur *util, bool multi = false);
    int extrairePersonnage(Carte *carte, char *tampon);

    QTcpSocket *socketTcp;		// Socket gere par le thread
    enteteMessage entete;		// Contient l'entete du message en cours de reception
    bool receptionEnCours;		// Indique si un message est actuellement en cours de reception
    char *tampon;				// Tampon contenant le message en court de reconstitution
    quint32 restant;			// Taille des donnees restant a receptionner
#ifndef NULL_PLAYER
    LecteurAudio* G_lecteurAudio;
#endif
private slots :
    void reception();
    void erreurDeConnexion(QAbstractSocket::SocketError);

};

#endif
