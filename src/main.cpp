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
#include <QTextCodec>
#include <QTranslator>
#include <QtWebEngine/QtWebEngine>

#include "diceparser/qmltypesregister.h"

#include <QCommandLineOption>
#include <QCommandLineParser>

int main(int argc, char* argv[])
{
    QtWebEngine::initialize();
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontUseNativeMenuBar, true);
    QString appName("RCSE");
    a.setApplicationName(appName);
    QString version= QObject::tr("Unknown");
#ifdef VERSION_MINOR
#ifdef VERSION_MAJOR
#ifdef VERSION_MIDDLE
    version= QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
#endif
#endif
#endif
    a.setApplicationVersion(version);

    registerQmlTypes();

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Ressources
    // QResource::registerResource(appName+".rcc");

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
