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
    // Initialisation du generateur de nombre aleatoire
    qsrand(QDateTime::currentDateTime ().toTime_t ());

    // Creation de l'application
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    // Creation de la fenetre de log
    Log = new QTextEdit();
    Log->setWindowTitle("Log");
    Log->setReadOnly(true);
//	Log->show();

    // Installation du handler de debugage
    qInstallMsgHandler(handlerAffichageMsg);

    // Chargement du traducteur de Qt
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name());
    app.installTranslator(&qtTranslator);

    // On charge le fichier de ressources qui contient ttes les images
    QResource::registerResource(QString(NOM_APPLICATION) + ".rcc");

    #ifdef WIN32
        // M.a.j de l'icone de l'application
        app.setWindowIcon(QIcon(":/resources/icones/" + QString(NOM_APPLICATION) + ".png"));
    #endif

    // Initialisation de la librairie FMOD
/*		if (!FSOUND_Init(FREQUENCE_AUDIO, 32, 0))
        qWarning("Probleme d'initialisation de la librairie FMOD (main - main.cpp)");*/

    // We need an Uuid for the local player
    G_idJoueurLocal = QUuid::createUuid().toString();
    // and a list to store it into
    new ListeUtilisateurs;

    // Initialise features database with local features
    g_featuresList.addLocal(G_idJoueurLocal);

    // Creation du client/serveur : si la connexion reussie alors
    // le programme principal est lance
    G_clientServeur = new ClientServeur;
    if (!G_clientServeur->configAndConnect())
        return 0;
    
    // We have a connection, we create the main window.
    G_mainWindow = new MainWindow;
    G_mainWindow->setWindowTitle(NOM_APPLICATION);
    G_mainWindow->showNormal();

    // Lancement de la boucle d'application
    return app.exec();
} 
