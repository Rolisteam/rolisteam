/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                                *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyrigth (C) 2010 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
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


#ifndef CLIENT_SERVEUR_H
#define CLIENT_SERVEUR_H

#include <QTcpServer>
#include <QList>
#include <QColor>
#include <QTimer>
#include <QDialog>


#include "connectiondialog.h"
//#include "ui_timerdialog.h"
#include "initialisation.h"
#include "connectionretrydialog.h"
#include "preferencesmanager.h"
#include "playersList.h"


class Player;
class Liaison;



/**
 * @brief hold the list of socket (Liaison).
 * On startup displays the configDialog.
 */
class ClientServeur : public QObject
{
    Q_OBJECT

public :
    ClientServeur();
    ~ClientServeur();

    /**
     * @brief Display the configDialog and make the connection.
     * @return true if connection has been established, false if the user has clicked on the Quit button.
     */
    bool configAndConnect();

    void emettreDonnees(char *donnees, quint32 taille, Liaison *sauf);

    void ajouterLiaison(Liaison *liaison);

    bool isServer() const;



    bool isConnected() const;

    Liaison* getLinkToServer();


public slots:
    void disconnectAndClose();
    bool startConnection();

signals :
    void emissionDonnees(char *donnees, quint32 taille, Liaison *sauf);

    void linkAdded(Liaison * link);
    void linkDeleted(Liaison * link);

    void stopConnectionTry();

    void connectionStateChanged(bool);




private :
    void synchronizePreferences();
    void setConnectionState(bool);

    QTcpServer * m_server;
    QList<Liaison *> liaisons;
    Liaison * m_liaisonToServer;
    quint16 m_port;
    QString m_address;
    QTimer* m_reconnect;
    Player * m_localPlayer;
    bool m_disconnectAsked;
    PreferencesManager* m_preferences;

    ConnectionRetryDialog* m_dialog;
    ConnectionConfigDialog* m_configDialog;

    PlayersList* m_playersList;

    bool m_connectionState;


private slots :
    void nouveauClientConnecte();
    void finDeLiaison(Liaison * link);
    bool startConnectionToServer();
    bool startListening();
};

#endif
