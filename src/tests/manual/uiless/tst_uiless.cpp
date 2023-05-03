/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "utils/countdownobject.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    CountDownObject counter(4, 10);

    QObject::connect(&counter, &CountDownObject::triggered, &app, [](int remaingTries){
        qDebug() << "triggered: remaining:" << remaingTries;
    });
    QObject::connect(&counter, &CountDownObject::countDownChanged, &app, [](int c){
        qDebug() << "countDown: " << c;
    });

    QObject::connect(&counter, &CountDownObject::runningChanged, &app, [&app, &counter](){
        if(!counter.isRunning())
            app.quit();
    });

    counter.start();

    return app.exec();
}
