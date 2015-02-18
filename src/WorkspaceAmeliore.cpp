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

#include "WorkspaceAmeliore.h"
#include "constantesGlobales.h"

//#include <QTextStream>
/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
WorkspaceAmeliore::WorkspaceAmeliore(QWidget *parent)
: QWorkspace(parent)
{
  /*  QTextStream out(stderr,QIODevice::WriteOnly);
    out << " Si l'utilisateur a ajoute une image de fond, on la charge " <<QString(NOM_APPLICATION);*/
        // Nom du fichier image utilisateur, qui peut etre utilise pour le fond
	#ifdef WIN32
		QString fichierImage = QString(NOM_APPLICATION) + ".bmp";
        #else
               QString fichierImage = QDir::homePath() + "/." + QString(NOM_APPLICATION) + "/" + QString(NOM_APPLICATION) + ".bmp";
	#endif

	// Si l'utilisateur a ajoute une image de fond, on la charge

	if (QFile::exists(fichierImage))
			imageFond = new QImage(fichierImage);
	// Sinon on utilise le fond par defaut
	else
	{
                #ifdef WIN32
                        imageFond = new QImage(":/resources/icones/fond workspace win32.bmp");
                #else
                        imageFond = new QImage(":/resources/icones/fond workspace macos.bmp");
                #endif
	}
}

/********************************************************************/
/* Redessine le fond                                                */
/********************************************************************/	
void WorkspaceAmeliore::paintEvent(QPaintEvent *event)
{
	// Creation du painter pour pouvoir dessiner
	QPainter painter(this);

	// On calcule l'intersection de la zone a repeindre avec la taille de l'image
	QRect zoneARecopier = event->rect().intersected(QRect(0, 0, imageFond->width(), imageFond->height()));

	// Si la zone n'est pas vide, on recopie l'image de fond dans le workspace
	if (!zoneARecopier.isEmpty())
		painter.drawImage(zoneARecopier, *imageFond, zoneARecopier);
}
