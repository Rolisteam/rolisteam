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


#include <QCoreApplication>
#include <QTextCodec>
#include <QResource>
#include <QTranslator>
#include <QDateTime>
#include <QUuid>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <time.h>

#include "network/rolisteamdaemon.h"
#include "preferences/preferencesmanager.h"

/**
* @page Roliserver
* @tableofcontents
* @author Renaud Guezennec
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
* Qt5, zlib, QML,
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
    // Application creation
    QCoreApplication app(argc, argv);

    QString appName("roliserver");

    app.setOrganizationName("Rolisteam");
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
    //QString locale = QLocale::system().name();




    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption configuration(QStringList() << "c"<< "config", QObject::tr("Set the path to configuration file [mandatory]"),"config");
    QCommandLineOption print(QStringList() << "p"<< "print", QObject::tr("Print a default configuration file into Standard output"),"output");


    parser.addOption(configuration);
    parser.addOption(print);

    parser.parse(app.arguments());

    bool hasConfig = parser.isSet(configuration);
    bool askPrint = parser.isSet(print);


    QString configPath;

    if(hasConfig)
    {
        configPath = parser.value(configuration);
    }


    RolisteamDaemon deamon;
    if(askPrint)
    {
        QString configPathOut = parser.value(print);
        deamon.createEmptyConfigFile(configPathOut);
        return 0;
    }
    if(configPath.isEmpty())
    {
        parser.showHelp();
    }

    deamon.readConfigFile(configPath);


    //m_networkManager->setValueConnection(portValue,hostnameValue,username,roleValue);






    return app.exec();
} 
