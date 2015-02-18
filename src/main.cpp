/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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


#include <QtGui>

#include <time.h>

#include "ClientServeur.h"
#include "initialisation.h"
#include "MainWindow.h"
#include "constantesGlobales.h"
#include "variablesGlobales.h"


// Inclusion de la librairie FMOD (librairie audio)
#ifdef WIN32
        #define DLL_EXPORTS

        #include "fmod.h"
#endif

// Fenetre de log (utilise seulement dans ce fichier)
static QTextEdit *Log;


/********************************************************************/
/* Variables globales utilisees par tous les elements de            */
/* l'application                                                    */
/********************************************************************/

// Contient tous les parametres extraits du fichier d'initialisation
Initialisation  G_initialisation;

// Identifiant du joueur en local sur la machine
QString G_idJoueurLocal;

ClientServeur * G_clientServeur;
MainWindow    * G_mainWindow;


/********************************************************************/
/* handler d'affichage des messages d'erreur et de debug            */
/********************************************************************/
void handlerAffichageMsg(QtMsgType type, const char *msg)
{
    QString titre;
    Qt::GlobalColor couleur;

    // Parametrage du titre et de la couleur
    switch (type) {
    case QtDebugMsg:
        titre = "Debug : ";
        couleur = Qt::green;
        break;
    case QtWarningMsg:
        titre = "Warning : ";
        couleur = Qt::blue;
        break;
    case QtCriticalMsg:
        titre = "Critical : ";
        couleur = Qt::red;
        break;
    case QtFatalMsg:
        titre = "Fatal : ";
        couleur = Qt::darkGray;
        break;
    default :
        titre = "Type de message inconnu : ";
        couleur = Qt::magenta;
        break;
    }

    // Affichage du texte
    Log->setFontWeight(QFont::Bold);
    Log->setTextColor(couleur);
    Log->append(titre);
    Log->setFontWeight(QFont::Normal);
    Log->setTextColor(Qt::black);
    Log->insertPlainText(msg);

    // Erreur fatale : on quitte
    /* if (type == QtFatalMsg)
            abort();*/
}

/********************************************************************/
/* main                                                             */
/********************************************************************/	
int main(int argc, char *argv[])
{


    // Creation de l'application
    QApplication app(argc, argv);
    app.setApplicationName(NOM_APPLICATION);
    app.setApplicationVersion(VERSION_APPLICATION);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    // Creation de la fenetre de log
    Log = new QTextEdit();
    Log->setWindowTitle("Log");
    Log->setReadOnly(true);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name());
    app.installTranslator(&qtTranslator);

    QResource::registerResource(QString(NOM_APPLICATION) + ".rcc");

    #ifdef WIN32
        app.setWindowIcon(QIcon(":/resources/icons/" + QString(NOM_APPLICATION) + ".png"));
    #endif

    // Seeds random generator
    uint seed = quintptr(&app) + QDateTime::currentDateTime().toTime_t();
    qDebug("Seed %x", seed);
    qsrand(seed);

    // We need an Uuid for the local player
    G_idJoueurLocal = QUuid::createUuid().toString();
    g_featuresList.addLocal(G_idJoueurLocal);

    // Get a connection
    G_clientServeur = new ClientServeur;
    if (!G_clientServeur->configAndConnect())
        return 0;

    // Create the main window
    G_mainWindow = new MainWindow;
    if (!G_client)
        G_listeUtilisateurs->ajouterJoueur(G_clientServeur->currentUser()->id(), G_clientServeur->currentUser()->name(), G_clientServeur->currentUser()->color(), true, G_clientServeur->currentUser()->isGM());

    G_mainWindow->setWindowTitle(NOM_APPLICATION);
    // We have a connection, we launch the main window.
    G_mainWindow->showNormal();
    return app.exec();
} 
