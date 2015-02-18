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
#include "mainwindow.h"
#include "persons.h"

#include "constantesGlobales.h"
#include "variablesGlobales.h"





// Identifiant du joueur en local sur la machine
QString G_idJoueurLocal;

ClientServeur * G_clientServeur;

/**
* @mainpage Rolisteam Documentation
*
* @author Renaud Guezennec
*
* Rolisteam help you to manage role playing games with your friend all over the world.<br/>
* Rolisteam is a free software under GNU/GPL. Its purpose is to provide all features required to<br/>
* perform Role playing games with remote friends.<br/>
* It is based on Client/server architecture and it is written in C++ with Qt.<br/>
*
* <h2>Features</h2>:
* <ul>
*    <li>Chat with one, many and all players</li>
*    <li>Sharing images and many other media type</li>
*    <li>Drawing maps on the fly</li>
*    <li>Sharing environment sound</li>
*    <li>Multi-platform: Windows, Linux and Mac OS X</li>
* </ul>
* <h2>How to stay in touch ? </h2>
* Please take a look to <a href="http://www.rolisteam.org/">Rolisteam Web Site</a>
*
*
*
*/

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    // Creation de l'application
    QApplication app(argc, argv);
    Initialisation* init= Initialisation::getInstance();


    app.setApplicationName(init->getApplicationName());
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
    QResource::registerResource(init->getApplicationName() + ".rcc");

    QTranslator rolisteamTranslator;
    rolisteamTranslator.load(":/traduction/rolisteam_" + locale);
    app.installTranslator(&rolisteamTranslator);

    // Seeds random generator
    uint seed = quintptr(&app) + QDateTime::currentDateTime().toTime_t();
    //qDebug("Seed %x", seed);
    qsrand(seed);

    // We need an Uuid for the local player (do we ?)
    G_idJoueurLocal = QUuid::createUuid().toString();




    // Create the main window
    MainWindow* mainWindow =MainWindow::getInstance();

    mainWindow->setupUi();
    mainWindow->readSettings();
    int value = 0;
    if(mainWindow->showConnectionDialog())
    {

        mainWindow->setUpNetworkConnection();
        mainWindow->updateWindowTitle();
        mainWindow->checkUpdate();
        mainWindow->updateUi();
        // We have a connection, we launch the main window.
        mainWindow->showNormal();
        value = app.exec();
    }
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    delete mainWindow;
    return value;


} 
