/*************************************************************************
 *	 Copyright (C) 2007 by Romain Campioni                               *
 *	 Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   https://rolisteam.org/                                           *
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
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>
#include <QQuickStyle>
#include <QResource>
#include <QTranslator>
#include <QUuid>
#include <exception>

#include "common_qml/theme.h"
#include "mainwindow.h"
#include "rolisteamapplication.h"
#include "uiwatchdog.h"
#include "version.h"

Q_DECLARE_LOGGING_CATEGORY(rNetwork);
Q_LOGGING_CATEGORY(rNetwork, "rolisteam.network");
Q_DECLARE_LOGGING_CATEGORY(rDice);
Q_LOGGING_CATEGORY(rDice, "rolisteam.dice");

/**
 * @mainpage Rolisteam
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
 * Please, visit: https://rolisteam.org/
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
int main(int argc, char* argv[])
{
    // Application creation
    RolisteamApplication app(QString("rolisteam"), version::version, argc, argv);

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(charactersheet);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    // INIT STYLE
    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");

    // Ressources
    QResource::registerResource("rolisteam.rcc");
    customization::Theme::setPath(":/resources/stylesheet/qml/theme.ini");
    /*if(true)
        QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/rolistheme");
    else
        QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/resources/rolistheme-dark");*/

#ifdef Q_OS_WIN
    {


        QString cmdLine("\"%2\rolisteam.exe\" \"-l %1\"");
        QSettings fooKey("HKEY_CLASSES_ROOT\\rolisteam", QSettings::NativeFormat);
        fooKey.setValue(".", "URL:rolisteam Protocol");
        fooKey.setValue("URL Protocol", "");
        fooKey.sync();
        // qDebug() << fooKey.status();
        QSettings fooOpenKey("HKEY_CLASSES_ROOT\\rolisteam\\shell\\open\\command", QSettings::NativeFormat);
        QFileInfo info(QCoreApplication::applicationFilePath());
        fooOpenKey.setValue(".", cmdLine.arg("%1").arg(info.absoluteFilePath()));
        fooOpenKey.sync();
        // qDebug() << fooOpenKey.status();
    }
#endif

#ifndef UNIT_TESTS
    // UiWatchdog dog;
    // dog.start();
#endif

    SelectConnectionProfileDialog connectionDialog(app.gameCtrl());
    MainWindow mainWindow(app.gameCtrl(), app.arguments());

    QObject::connect(&app, &RolisteamApplication::stateChanged, &app,
                     [&app, &connectionDialog, &mainWindow]()
                     {
                         auto state= app.state();
                         using RA= RolisteamApplication::ApplicationState;
                         switch(state)
                         {
                         case RA::SelectProfile:
                             connectionDialog.setVisible(true);
                             mainWindow.makeVisible(false);
                             break;
                         case RA::Playing:
                             connectionDialog.accept();
                             mainWindow.makeVisible(true);
                             connectionDialog.setVisible(false);
                             break;
                         case RA::Exit:
                             mainWindow.makeVisible(false);
                             connectionDialog.setVisible(false);
                             QMetaObject::invokeMethod(&app, &RolisteamApplication::quit, Qt::QueuedConnection);
                             break;
                         }
                     });

    connectionDialog.setVisible(true);
    return app.exec();
}
