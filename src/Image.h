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


/********************************************************************/
/*                                                                  */
/* QScrollArea contenant une image, qui ne peut etre fermee que par */
/* le joueur qui l'a ouverte ou par le MJ. Le but est de pouvoir    */
/* afficher des images sans passer par les cartes qui utilisent     */
/* beaucoup de ressources systeme.                                  */
/*                                                                  */
/********************************************************************/	


#ifndef IMAGE_H
#define IMAGE_H

    #include <QWidget>
    #include <QImage>
    #include <QScrollArea>
    #include <QAction>
	#include <QFile>
	#include <QString>
	#include <QLabel>


    class Image : public QScrollArea
    {
	Q_OBJECT
		
    public :
	    Image(QString identImage, QString identJoueur, QImage *image, QAction *action = 0, QWidget *parent = 0);
		~Image();
		void associerAction(QAction *action);
		void emettreImage(QString titre, int numeroLiaison);
		void sauvegarderImage(QFile &file, QString titre);
		bool proprietaireImage();
		QString identifiantImage();

	public slots :
		void pointeurMain();
		void pointeurNormal();

    private :
		QString idImage;
		QString idJoueur;
		QAction *actionAssociee;
		QLabel *labelImage;
		QPoint pointDepart;
		int horizontalDepart;
		int verticalDepart;
		bool deplacementAutorise;

	protected :
		void closeEvent(QCloseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	};

#endif
