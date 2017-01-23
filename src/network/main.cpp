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

//#include "network/networkmanager.h"
//#include "preferences/preferencesmanager.h"
#include "rolisteamdaemon.h"

/**
* @page Rolisteamd Rolisteamd
* @tableofcontents
* @author Renaud Guezennec
*
*  @section intro_sec Introduction
* Stand alone server for rolisteam.
*
* @section features_sec Features:
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


    QString appName("rolisteamd");

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


    // Settings
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("RolisteamD is a stand alone server to host games from rolisteam clients."));

    QCommandLineOption configFile(QStringList() << "f" << "configFile","Read all parameters from <file>.","file");
    QCommandLineOption generateFile(QStringList() << "g" << "generate","Generate basic config <output>.","output");

    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(configFile);
    parser.addOption(generateFile);

    RolisteamDaemon daemon;
    parser.process(app.arguments());
    if(parser.isSet(configFile))
    {
        QString value = parser.value(configFile);
        daemon.readConfigFile(value);
    }
    if(parser.isSet(generateFile))
    {
        QString value = parser.value(generateFile);
        daemon.readConfigFile(value);
    }




    return app.exec();




}
