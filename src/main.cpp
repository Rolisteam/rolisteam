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
static QTextEdit *Log;
#define APPLICATION_NAME "rolisteam"
initialisation G_initialisation;

void handlerAffichageMsg(QtMsgType type, const char *msg)
{
            QString titre;
            Qt::GlobalColor couleur;

            // Parametrage du titre et de la couleur
    switch (type) {
    case QtDebugMsg:
        titre = "Debug : ";
        couleur = Qt::green;
        break;
    case QtWarningMsg:
        titre = "Warning : ";
        couleur = Qt::blue;
        break;
    case QtCriticalMsg:
        titre = "Critical : ";
        couleur = Qt::red;
        break;
    case QtFatalMsg:
        titre = "Fatal : ";
        couleur = Qt::darkGray;
        break;
    default :
                    titre = "Type de message inconnu : ";
                    couleur = Qt::magenta;
                    break;
    }

            // Affichage du texte
            Log->setFontWeight(QFont::Bold);
    Log->setTextColor(couleur);
            Log->append(titre);
            Log->setFontWeight(QFont::Normal);
    Log->setTextColor(Qt::black);
            Log->insertPlainText(msg);

            // Erreur fatale : on quitte
            if (type == QtFatalMsg)
                    abort();
}

int main(int argc, char *argv[])
{
    srand(clock());


    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QCoreApplication::tr(APPLICATION_NAME));
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

    // Lancement de la boucle d'application
    return app.exec();
}
