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


#include "initialisation.h"
#include "Liaison.h"
#include "mainwindow.h"
#include "persons.h"


#include "variablesGlobales.h"



// True si l'utilisateur est un joueur, false s'il est MJ
bool G_joueur;
// True si l'ordinateur local est client, false s'il est serveur
bool G_client;

#define second 1000
/********************
 * Global functions *
 ********************/

void emettre(char *donnees, quint32 taille, Liaison *sauf)
{
    G_clientServeur->emettreDonnees(donnees, taille, sauf);
}

/*****************
 * ClientServeur *
 *****************/

ClientServeur::ClientServeur()
    : QObject(), m_server(NULL),m_liaisonToServer(NULL),m_disconnectAsked(false),m_connectionState(false)
{

    m_reconnect = new QTimer(this);
    m_preferences =  PreferencesManager::getInstance();
    //m_thread = new ConnectionRetryThread();
    m_dialog = new ConnectionRetryDialog();
    connect(m_dialog,SIGNAL(tryConnection()),this,SLOT(startConnectionToServer()));
    connect(m_dialog,SIGNAL(rejected()),this,SIGNAL(stopConnectionTry()));



}


ClientServeur::~ClientServeur()
{
    if(NULL!=m_configDialog)
    {
        delete m_configDialog;
        m_configDialog = NULL;
    }
    if(NULL!=m_dialog)
    {
        delete m_dialog;
        m_dialog = NULL;
    }
    delete m_reconnect;
}
void ClientServeur::synchronizePreferences()
{
    m_preferences->registerValue("UserName",m_configDialog->getName());
    m_preferences->registerValue("UserColor",m_configDialog->getColor());
    m_preferences->registerValue("isPlayer",!m_configDialog->isGM());
    m_preferences->registerValue("ipaddress",m_configDialog->getHost());
    G_joueur= !m_configDialog->isGM();
    G_client= !m_configDialog->isServer();
    m_preferences->registerValue("ServerPort",m_configDialog->getPort());
    m_preferences->registerValue("isClient",!m_configDialog->isServer());
    m_preferences->registerValue("clientPort",m_configDialog->getPort());

}

bool ClientServeur::configAndConnect()
{
    m_configDialog = new ConnectionConfigDialog(
                m_preferences->value("UserName",tr("UserName")).toString(),
                m_preferences->value("UserColor",QColor(255,255,255)).value<QColor>(),
                !m_preferences->value("isPlayer",true).toBool(),
                m_preferences->value("ipaddress","").toString(),
                m_preferences->value("ServerPort",6660).toInt(),
                !m_preferences->value("isClient",true).toBool());





    m_playersList = PlayersList::instance();

    // If the user abort configDialog, we quit
    if (m_configDialog->exec() == QDialog::Rejected)
    {
        return false;
    }
    synchronizePreferences();
    m_localPlayer = new Player(
            QUuid(G_idJoueurLocal),
            m_configDialog->getName(),
            m_configDialog->getColor(),
            m_configDialog->isGM()
        );


    startConnection();


    setConnectionState(true);
    return true;
}
bool ClientServeur::startConnection()
{
    bool cont = true;
    while (cont)
    {
        // Server setup
        if (m_configDialog->isServer())
        {
            cont = !startListening();
        }
        else
        {
            m_port = m_configDialog->getPort();
            m_address = m_configDialog->getHost();
            if(startConnectionToServer())
            {
                cont = false;
            }
        }
        if(cont)
        {
            m_dialog->startTimer();
            m_dialog->exec();
        }



    }
    m_playersList = PlayersList::instance();
    if(m_disconnectAsked)
    {
         m_playersList->cleanList();
         m_playersList->sendOffLocalPlayerInformations();
         m_playersList->sendOffFeatures( m_playersList->getLocalPlayer());

    }
    else
    {
        m_playersList->setLocalPlayer(m_localPlayer);
    }
    return true;
}
bool  ClientServeur::startListening()
{
    if (m_server == NULL)
    {
        m_server = new QTcpServer(this);
        connect(m_server, SIGNAL(newConnection()), this, SLOT(nouveauClientConnecte()));
    }
    if (m_server->listen(QHostAddress::Any, m_configDialog->getPort()))
    {
        connect(this, SIGNAL(linkDeleted(Liaison *)), m_playersList, SLOT(delPlayerWithLink(Liaison *)));
        return true;
    }
    else
    {
        QMessageBox errorDialog(QMessageBox::Warning, tr("Error"), tr("Can not establish the connection."));
        errorDialog.setInformativeText(m_server->errorString());
        errorDialog.exec();
        return false;
    }

}

bool ClientServeur::startConnectionToServer()
{
    ConnectionWaitDialog waitDialog;
    QTcpSocket * socket;

    socket = waitDialog.connectTo(m_address, m_port);
    qDebug()<< "connection retry";
    //QMessageBox errorDialog(QMessageBox::Warning, tr("Error"), tr("Can not establish the connection."));
    // connect successed

        if (socket != NULL)
        {
            //G_client=true;
            if(NULL==m_liaisonToServer)
            {
                m_liaisonToServer = new Liaison(socket);
            }
            else
            {
                m_liaisonToServer->setSocket(socket);
            }

          return true;

        }
        else
        {
            return false;
        }


}

void ClientServeur::emettreDonnees(char *donnees, quint32 taille, Liaison *sauf)
{
    // Demande d'emission vers toutes les liaisons
    emit emissionDonnees(donnees, taille, sauf);
}


void ClientServeur::ajouterLiaison(Liaison *liaison)
{
    liaisons.append(liaison);
    connect(this, SIGNAL(emissionDonnees(char *, quint32, Liaison *)),liaison, SLOT(emissionDonnees(char *, quint32, Liaison *)));
    connect(liaison, SIGNAL(disconnected(Liaison *)),this, SLOT(finDeLiaison(Liaison *)));
    //liaison->initialize();
    emit linkAdded(liaison);
}


void ClientServeur::nouveauClientConnecte()
{
    // Recuperation du socket lie a la demande de connexion
    QTcpSocket *socketTcp = m_server->nextPendingConnection();


    new Liaison(socketTcp);

}


void ClientServeur::finDeLiaison(Liaison * link)
{

    if(!m_disconnectAsked)
    {
        link->deleteLater();

        emit linkDeleted(link);
    }


    if (!m_configDialog->isServer())
    {
        //On quitte l'application
        //MainWindow::notifyUser(tr("Receiving picture: %1"));
        if(link!=m_liaisonToServer)
        {
            qDebug() << "link is NOT the link to the server ";
            setConnectionState(false);
        }
        else
            qDebug() << "link is the link to the server ";


        if(!m_disconnectAsked)
        {
            m_dialog->startTimer();
            m_dialog->show();
        }
    }

    // Si l'ordinateur local est le serveur
    else
    {
        if (link == NULL)
        {
            qWarning() << tr("NULL Liaison pointer (ClientServeur::finDeLiaison).");
            return;
        }

        int i = liaisons.indexOf(link);
        if (i < 0)
        {
            qWarning()<< tr("Unknown thread joined, (finDeLiaison - ClientServeur.cpp)");
            return;
        }
        liaisons.removeAt(i);
    }
}
void ClientServeur::disconnectAndClose()
{
    if (m_configDialog->isServer())
    {
        m_server->close();
        MainWindow::notifyUser(tr("Server has been close."));
    }
    else
    {
        m_disconnectAsked = true;
        m_liaisonToServer->disconnectAndClose();
        MainWindow::notifyUser(tr("Connection to the server has been close."));
    }
    setConnectionState(false);
}
bool ClientServeur::isServer() const
{
    return m_configDialog->isServer();
}
bool ClientServeur::isConnected() const
{
    return m_connectionState;
}

void ClientServeur::setConnectionState(bool state)
{
    if(m_connectionState!=state)
    {
        m_connectionState=state;
        connectionStateChanged(m_connectionState);
    }
}
