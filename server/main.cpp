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
#include <QTextStream>
#include <QMessageLogContext>
enum LOG_STATE {DEBUG,WARNING,CRITICAL,FATAL,INFO};

int g_logMini = CRITICAL;

void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
      QByteArray localMsg = msg.toLocal8Bit();
      QString time = QDateTime::currentDateTime().toString("dd/mm/yyyy - hh:mm:ss");
      QTextStream stream(stderr);
      if(g_logMini != INFO)
      {
          if((type==QtDebugMsg)&&(g_logMini!=DEBUG))
          {
             return;
          }
          else if((g_logMini==DEBUG)&&(type!=QtDebugMsg))
          {
             return;
          }
          else if(type < g_logMini)
          {
              return;
          }
      }
      switch (type)
      {
      case QtDebugMsg:
          stream << QStringLiteral("Debug: [%1] - %2 (%3:%4, %5)\n").arg(time).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
          break;
      case QtInfoMsg:
          stream << QStringLiteral("Info: [%1] - %2 (%3:%4, %5)\n").arg(time).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
          break;
      case QtWarningMsg:
          stream << QStringLiteral("Warning: [%1] - %2 (%3:%4, %5)\n").arg(time).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
          break;
      case QtCriticalMsg:
          stream << QStringLiteral("Critical: [%1] - %2 (%3:%4, %5)\n").arg(time).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
          break;
      case QtFatalMsg:
          stream << QStringLiteral("Fatal: [%1] - %2 (%3:%4, %5)\n").arg(time).arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
          abort();
      }
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    qInstallMessageHandler(logOutput);
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

    g_logMini = static_cast<LOG_STATE>(deamon.getLevelOfLog());


    deamon.start();
    //m_networkManager->setValueConnection(portValue,hostnameValue,username,roleValue);






    return app.exec();
} 
