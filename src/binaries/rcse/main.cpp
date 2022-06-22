/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "mainwindow.h"
#include <QApplication>

#include <QTranslator>

#include "qmltypesregister.h"
#include "version.h"

#include <QCommandLineOption>
#include <QCommandLineParser>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Q_INIT_RESOURCE(charactersheet);

    a.setAttribute(Qt::AA_DontUseNativeMenuBar, true);
    QString appName("RCSE");
    a.setApplicationName(appName);

    a.setApplicationVersion(version::version);

    registerQmlTypes();

    QString locale= QLocale::system().name();

    // INIT STYLE
    QIcon::setThemeSearchPaths({":/rcstyle"});

    QTranslator rolisteamTranslator;
    rolisteamTranslator.load(QLocale(), ":/translations/rcse", "_");
    a.installTranslator(&rolisteamTranslator);

    QTranslator qtTranslator;
    qtTranslator.load(QLocale(), ":/translations/qt", "_");
    a.installTranslator(&qtTranslator);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption translation(QStringList() << "t"
                                                 << "translation",
                                   QObject::tr("define path to current <translation>"), "translation");
    QCommandLineOption file(QStringList() << "f"
                                          << "file",
                            QObject::tr("open <file>."), "file");

    parser.addOption(translation);
    parser.addOption(file);
    parser.process(a.arguments());

    if(parser.isSet(translation))
    {
        QTranslator* cliTranslator= new QTranslator();
        cliTranslator->load(parser.value(translation));
        a.installTranslator(cliTranslator);
    }

    MainWindow w;
    if(parser.isSet(file))
    {
        w.loadFile(parser.value(file));
    }

    w.show();

    return a.exec();
}
