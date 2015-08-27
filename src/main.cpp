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


#include <QApplication>
#include <QTextCodec>
#include <QResource>
#include <QTranslator>
#include <QDateTime>
#include <QUuid>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <time.h>

#include "network/networkmanager.h"
#include "mainwindow.h"
#include "data/persons.h"
#include "preferences/preferencesmanager.h"

#include "variablesGlobales.h"

/**
* @mainpage Rolisteam
*
* @section @author Renaud Guezennec
*
*  @section intro_sec Introduction
* Rolisteam help you to manage role playing games with your friend all over the world.<br/>
* Rolisteam is a free software under GNU/GPL. Its purpose is to provide all features required to<br/>
* perform Role playing games with remote friends.<br/>
* It is based on Client/server architecture and it is written in C++ with Qt.<br/>
*
* @section features_sec Features:
* - Chat with one, many and all players
* - Sharing images and many other media type
* - Drawing maps on the fly
* - Sharing environment sound
* - Multi-platform: Windows, Linux and Mac OS X
* - Powerful die rolling syntax
* - Theme and skin: make your own skin, save it, share it.
* - Useful preferences systems
*
*
*
* @section install_sec Installation
* To get documentation on how to install rolisteam: http://wiki.rolisteam.org/
*
* @section How to stay in touch ?
* Please, visit: http://www.rolisteam.org/
*
* @section tools_subsec Dependencies:
* Qt5, zlib.
* @subpage DiceParser
*
* @section copyright Copyright and License
* GNU/GPLv2
*
* <BR><BR>
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

    QString appName="rolisteam";

    app.setApplicationName(appName);
    QString version = QObject::tr("Unknown");
    #ifdef VERSION_MINOR
        #ifdef VERSION_MAJOR
            #ifdef VERSION_MIDDLE
                version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
            #endif
        #endif
    #endif
    app.setApplicationVersion(version);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QString locale = QLocale::system().name();

    // Ressources
    QResource::registerResource(appName+".rcc");

    QTranslator rolisteamTranslator;
    rolisteamTranslator.load(":/translations/rolisteam_" + locale);
    app.installTranslator(&rolisteamTranslator);

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + locale);
    app.installTranslator(&qtTranslator);


    QStringList translationCLI;
    translationCLI << "-t"<<"--translation";
    QStringList argList = app.arguments();
    foreach(QString str, translationCLI)
    {
        int pos = argList.indexOf(str);
        if(pos!=-1)
        {
            if(argList.size()>pos+1)
            {
				QTranslator* cliTranslator = new QTranslator();
				cliTranslator->load(argList.at(pos+1));
				app.installTranslator(cliTranslator);
            }
        }
    }


    // Settings
    QSettings settings("rolisteam",QString("rolisteam_%1/preferences").arg(version));
    settings.beginGroup("rolisteam/preferences");

    QMap<QString,QVariant> map;

    int size = settings.beginReadArray("preferenceMap");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString key = settings.value("key").toString();
        QVariant value = settings.value("value");
        map.insert(key,value);
    }
    settings.endArray();
    settings.endGroup();

//    QVariant variant = settings.value("map",map);
//    if(variant.canConvert<QMap<QString,QVariant> >())
//    {
//        map = variant.value<QMap<QString,QVariant> >();
//    }
//    settings.endGroup();

    QString file = map.value("currentTranslationFile","").toString();
    if(!file.isEmpty())
    {
        QTranslator* currentTranslator = new QTranslator();
        currentTranslator->load(file);
        app.installTranslator(currentTranslator);
    }






    // Create the main window
    MainWindow* mainWindow =MainWindow::getInstance();
    mainWindow->parseCommandLineArguments(app.arguments());

    mainWindow->setupUi();
    mainWindow->readSettings();




    int value = 0;
    if(mainWindow->showConnectionDialog())
    {
        mainWindow->setUpNetworkConnection();
        mainWindow->updateWindowTitle();
        mainWindow->checkUpdate();
        mainWindow->updateUi();
        if(PreferencesManager::getInstance()->value("FullScreenAtStarting",true).toBool())
        {
            mainWindow->showMaximized();
        }
        else
        {
            mainWindow->show();
        }
        value = app.exec();
    }
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    delete mainWindow;
    return value;


} 
