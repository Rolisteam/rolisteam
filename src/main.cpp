/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui>
#include <time.h>
#include "MainWindow.h"
#include "connection.h"

/**
* @mainpage Rolisteam Documentation 
*
* @author Renaud Guezennec
*
* Rolisteam help you to manage role playing games with your friend all over the world.
* Rolisteam is a free software under GNU/GPL. Its purpose is to provide all features required to
* perform Role playing games with remote friends.
* It is based on Client/server architecture and it is written in C++ with Qt.
* 
*
* <h2>How to stay in touch ? </h2>
* Please take a look to <a href="http://www.rolisteam.org/">Rolisteam Web Site</a>
*
*
*
*/

int main(int argc, char *argv[])
{
    qsrand(QDateTime::currentDateTime ().toTime_t ());
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QCoreApplication::tr("rolisteam"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name());
    app.installTranslator(&qtTranslator);

    qRegisterMetaType<Connection>("Connection");
    qRegisterMetaTypeStreamOperators<Connection>("Connection");

    MainWindow* mw =new MainWindow();
    mw->show();
    return app.exec();
}
