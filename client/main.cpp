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
#include <QQuickStyle>
#include <QResource>
#include <QTextCodec>
#include <QTranslator>
#include <QUuid>

#include "common/controller/theme.h"
#include "data/cleveruri.h"
#include "data/person.h"
#include "mainwindow.h"
#include "preferences/preferencesmanager.h"
#include "rolisteamapplication.h"
#include "uiwatchdog.h"

#include <exception>

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
    // QApplication app(nargs, args);
    RolisteamApplication app(argc, argv);

    Q_INIT_RESOURCE(rolisteamqml);
    Q_INIT_RESOURCE(textedit);

    auto format= QSurfaceFormat::defaultFormat();
    if(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);
    QQuickStyle::setStyle(":/rolistyle");
    QQuickStyle::setFallbackStyle("Default");

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Ressources
    QResource::registerResource("rolisteam.rcc");

    // Settings
    QSettings settings("rolisteam", QString("rolisteam_%1/preferences").arg(app.applicationVersion()));
    settings.beginGroup("rolisteam/preferences");

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

    QMap<QString, QVariant> map;

    int size= settings.beginReadArray("preferenceMap");
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString key= settings.value("key").toString();
        QVariant value= settings.value("value");
        map.insert(key, value);
    }
    settings.endArray();
    settings.endGroup();

#ifndef UNIT_TESTS
    UiWatchdog dog;
    dog.start();
#endif

    customization::Theme::setPath(":/resources/stylesheet/qml/theme.ini");

    // Create the main window
    MainWindow* mainWindow= new MainWindow(app.arguments());
    int value= 0;
    mainWindow->showAsPreferences();
    value= app.exec();
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    delete mainWindow;
    return value;
}
