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

#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString appName("RCSE");
    a.setApplicationName(appName);
    QString version = QObject::tr("Unknown");
    #ifdef VERSION_MINOR
        #ifdef VERSION_MAJOR
            #ifdef VERSION_MIDDLE
                version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
            #endif
        #endif
    #endif
    a.setApplicationVersion(version);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QString locale = QLocale::system().name();

    // Ressources
    //QResource::registerResource(appName+".rcc");

    QTranslator rolisteamTranslator;
    rolisteamTranslator.load(":/translations/rcse_" + locale);
    a.installTranslator(&rolisteamTranslator);

    QTranslator qtTranslator;
    qtTranslator.load(":/translations/qt_" + locale);
    a.installTranslator(&qtTranslator);


    QStringList translationCLI;
    translationCLI << "-t"<<"--translation";
    QStringList argList = a.arguments();
    foreach(QString str, translationCLI)
    {
        int pos = argList.indexOf(str);
        if(pos!=-1)
        {
            if(argList.size()>pos+1)
            {
                QTranslator* cliTranslator = new QTranslator();
                cliTranslator->load(argList.at(pos+1));
                a.installTranslator(cliTranslator);
            }
        }
    }



    MainWindow w;
    w.show();

    return a.exec();
}
