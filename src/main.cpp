/*************************************************************************
 *	 Copyright (C) 2007 by Romain Campioni                               *
 *	 Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   http://www.rolisteam.org/                                           *
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


#include <QtGui>

#include <time.h>

#include "ClientServeur.h"
#include "initialisation.h"
#include "MainWindow.h"
#include "persons.h"

#include "constantesGlobales.h"
#include "variablesGlobales.h"

// Inclusion de la librairie FMOD (librairie audio)
#ifdef WIN32
        #define DLL_EXPORTS
        #include "fmod.h"
#endif


/*************************************/
/* Variables globales (we love them) */
/*************************************/

// Contient tous les parametres extraits du fichier d'initialisation
Initialisation  G_initialisation;

// Identifiant du joueur en local sur la machine
QString G_idJoueurLocal;

ClientServeur * G_clientServeur;
MainWindow    * G_mainWindow;


/********/
/* main */
/********/

int main(int argc, char *argv[])
{
    // Creation de l'application
    QApplication app(argc, argv);
    app.setApplicationName(NOM_APPLICATION);
    QString version = QObject::tr("Unknown");
    #ifdef VERSION_MINOR
        #ifdef VERSION_MAJOR
            #ifdef VERSION_MIDDLE
                version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
            #endif
        #endif
    #endif
    app.setApplicationVersion(version);

    // Internationalization
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QString locale = QLocale::system().name();
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // Ressources
    QResource::registerResource(QString(NOM_APPLICATION) + ".rcc");
    #ifdef WIN32
        app.setWindowIcon(QIcon(":/resources/icons/" + QString(NOM_APPLICATION) + ".png"));
    #endif
    QTranslator rolisteamTranslator;
    rolisteamTranslator.load(":/traduction/rolisteam_" + locale);
    app.installTranslator(&rolisteamTranslator);

    // Seeds random generator
    uint seed = quintptr(&app) + QDateTime::currentDateTime().toTime_t();
    qDebug("Seed %x", seed);
    qsrand(seed);

    // We need an Uuid for the local player (do we ?)
    G_idJoueurLocal = QUuid::createUuid().toString();

    // Get a connection
    G_clientServeur = new ClientServeur;
    if (!G_clientServeur->configAndConnect())
        return 0;

    // Create the main window
    G_mainWindow = new MainWindow;

    G_mainWindow->setWindowTitle(NOM_APPLICATION);
    G_mainWindow->checkUpdate();
    // We have a connection, we launch the main window.
    G_mainWindow->showNormal();
    return app.exec();
} 
