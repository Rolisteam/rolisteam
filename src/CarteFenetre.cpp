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

	#include "CarteFenetre.h"
	#include "variablesGlobales.h"



CarteFenetre::CarteFenetre(Carte *uneCarte, QWidget *parent)
    : QScrollArea(parent)
{

    setObjectName("CarteFenetre");

            setWindowIcon(QIcon(":/resources/icones/vignette plan.png"));

    carteAssociee = uneCarte;

    setAlignment(Qt::AlignCenter);

    setWidget(carteAssociee);

    #ifdef WIN32
        resize(carteAssociee->width()+2, carteAssociee->height()+2);
    #elif defined (MACOS)
        resize(carteAssociee->width()+4, carteAssociee->height()+4);
            #elif defined Q_WS_X11
                    resize(carteAssociee->width()+2, carteAssociee->height()+2);
    #endif

    QObject::connect(carteAssociee, SIGNAL(commencerDeplacementCarteFenetre(QPoint)), this, SLOT(commencerDeplacement(QPoint)));
    QObject::connect(carteAssociee, SIGNAL(deplacerCarteFenetre(QPoint)), this, SLOT(deplacer(QPoint)));
}


CarteFenetre::~CarteFenetre()
{

    actionAssociee->~QAction();
    G_mainWindow->enleverCarteDeLaListe(carteAssociee->identifiantCarte());
}

void CarteFenetre::closeEvent(QCloseEvent *event)
{
    hide();
    actionAssociee->setChecked(false);
    event->ignore();
}


void CarteFenetre::associerAction(QAction *action)
{
    actionAssociee = action;
}


Carte * CarteFenetre::carte()
{
    return carteAssociee;
}


void CarteFenetre::commencerDeplacement(QPoint position)
{
    pointDepart = position;
    horizontalDepart = horizontalScrollBar()->value();
    verticalDepart = verticalScrollBar()->value();
}

void CarteFenetre::deplacer(QPoint position)
{

    QPoint diff = pointDepart - position;
    horizontalScrollBar()->setValue(horizontalDepart + diff.x());
    verticalScrollBar()->setValue(verticalDepart + diff.y());
}
