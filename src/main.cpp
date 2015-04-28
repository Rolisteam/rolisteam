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
        QTranslator*  cliTranslator=NULL;
        int pos = argList.indexOf(str);
        if(pos!=-1)
        {
            if(argList.size()>pos+1)
            {
            cliTranslator = new QTranslator();
            cliTranslator->load(argList.at(pos+1));
            app.installTranslator(cliTranslator);
            }
        }
    }


    // Settings
    QSettings settings("rolisteam",QString("rolisteam_%1/preferences").arg(version));
    settings.beginGroup("rolisteam/preferences");

    QMap<QString,QVariant> map;
    QVariant variant = settings.value("map",map);
    if(variant.canConvert<QMap<QString,QVariant> >())
    {
        map = variant.value<QMap<QString,QVariant> >();
    }
    settings.endGroup();

    QString file = map.value("currentTranslationFile","").toString();
    if(!file.isEmpty())
    {
        qDebug() << file<< "current translation file";
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
