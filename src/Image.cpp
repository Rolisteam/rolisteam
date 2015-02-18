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

#include "Image.h"
#include "variablesGlobales.h"


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
Image::Image(QString identImage, QString identJoueur, QImage *image, QAction *action, QWidget *parent)
: QScrollArea(parent)
{
	// On donne un nom a l'objet "Image" pour le differencier des autres fenetres du workspace
	setObjectName("Image");

	// On change l'icone de la fenetre
	setWindowIcon(QIcon(":/icones/vignette image.png"));

	// Creation du label qui contient l'image
	labelImage = new QLabel(this);
	QPixmap pixmap = QPixmap::fromImage(*image);
	labelImage->setPixmap(pixmap);
	labelImage->resize(image->width(), image->height());
	// Memorisation de l'action associee
	actionAssociee = action;
	// Memorisation des ID de l'image et du joueur qui l'a ouverte
	idImage = identImage;
	idJoueur = identJoueur;
	// On aligne l'image au centre de la scrollArea
setAlignment(Qt::AlignCenter);
	// Association du label contenant l'image avec le scrollArea
	setWidget(labelImage);
	// Redimentionement de la taille du scrollArea
	resize(image->width()+2, image->height()+2);
}

/********************************************************************/	
/* Destructeur                                                      */
/********************************************************************/	
Image::~Image()
{
	// Destruction de l'action associee
	actionAssociee->~QAction();
	// On enleve l'image de la liste des Images existantes
	G_mainWindow->enleverImageDeLaListe(idImage);
}

/********************************************************************/
/* Cache la fenetre au lieu de la detruire                          */
/********************************************************************/	
void Image::closeEvent(QCloseEvent *event)
{
	// Masquage de la fenetre
	hide();
	// Deselection de l'action associee
	actionAssociee->setChecked(false);
	// Arret de la procedure de fermeture		
	event->ignore();
}

/********************************************************************/
/* Associe l'action d'affichage/masquage a la carte                 */
/********************************************************************/	
void Image::associerAction(QAction *action)
{
	actionAssociee = action;
}

/********************************************************************/
/* Renvoie true si l'utilisateur local est le proprietaire de       */
/* l'image                                                          */
/********************************************************************/
bool Image::proprietaireImage()
{
	return idJoueur == G_idJoueurLocal;
}

/********************************************************************/
/* Renvoie l'identifiant de l'image                                 */
/********************************************************************/
QString Image::identifiantImage()
{
	return idImage;
}

/********************************************************************/
/* Emet l'Image vers la liaison passee en parametre                 */
/********************************************************************/
void Image::emettreImage(QString titre, int numeroLiaison)
{
	bool ok;

	// On compresse l'image dans un tableau
	QByteArray baImage;
	QBuffer bufImage(&baImage);
	ok = labelImage->pixmap()->save(&bufImage, "jpeg", 70);
	if (!ok)
		qWarning("Probleme de compression de l'image (emettreImage - Image.cpp)");
	
	// Taille des donnees
	quint32 tailleCorps =
		// Taille du titre
		sizeof(quint16) + titre.size()*sizeof(QChar) +
		// Taille de l'identifiant de l'image
		sizeof(quint8) + idImage.size()*sizeof(QChar) +
		// Taille de l'identifiant du joueur
		sizeof(quint8) + idJoueur.size()*sizeof(QChar) +
		// Taille de l'image
		sizeof(quint32) + baImage.size();
			
	// Buffer d'emission
	char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

	// Creation de l'entete du message
	enteteMessage *uneEntete;
	uneEntete = (enteteMessage *) donnees;
	uneEntete->categorie = image;
	uneEntete->action = chargerImage;
	uneEntete->tailleDonnees = tailleCorps;
	
	// Creation du corps du message
	int p = sizeof(enteteMessage);
	// Ajout du titre
	quint16 tailleTitre = titre.size();
	memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
	p+=sizeof(quint16);
	memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
	p+=tailleTitre*sizeof(QChar);
	// Ajout de l'identifiant de l'image
	quint8 tailleIdImage = idImage.size();
	memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
	p+=sizeof(quint8);
	memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
	p+=tailleIdImage*sizeof(QChar);
	// Ajout de l'identifiant du joueur
	quint8 tailleIdJoueur = idJoueur.size();
	memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
	p+=sizeof(quint8);
	memcpy(&(donnees[p]), idJoueur.data(), tailleIdJoueur*sizeof(QChar));
	p+=tailleIdJoueur*sizeof(QChar);
	// Ajout de l'image
	quint32 tailleImage = baImage.size();
	memcpy(&(donnees[p]), &tailleImage, sizeof(quint32));
	p+=sizeof(quint32);
	memcpy(&(donnees[p]), baImage.data(), tailleImage);
	p+=tailleImage;

	// Emission de l'image vers la liaison indiquee
	emettre(donnees, tailleCorps + sizeof(enteteMessage), numeroLiaison);
	// Liberation du buffer d'emission
	delete[] donnees;
}

/********************************************************************/
/* Sauvegarde l'image dans le fichier passe en parametre            */
/********************************************************************/
/*void Image::sauvegarderImage(QFile &file, QString titre)
{
	bool ok;

	// On commence par compresser l'image (format jpeg) dans un tableau
	QByteArray baImage;
	QBuffer bufImage(&baImage);
	ok = labelImage->pixmap()->save(&bufImage, "jpeg", 100);
	if (!ok)
		qWarning("Probleme de compression de l'image (sauvegarderImage - Image.cpp)");
	
	// Ecriture de l'image dans le fichier

	// Ecriture du titre
	quint16 tailleTitre = titre.size();
	file.write((char *)&tailleTitre, sizeof(quint16));
	file.write((char *)titre.data(), tailleTitre*sizeof(QChar));
	// Ajout de l'image
	quint32 tailleImage = baImage.size();
	file.write((char *)&tailleImage, sizeof(quint32));
	file.write(baImage.data(), tailleImage);
}*/
/********************************************************************/
/* Sauvegarde l'image dans le fichier passe en parametre            */
/********************************************************************/
void Image::sauvegarderImage(QDataStream &out, QString titre)
{
    bool ok;

    // On commence par compresser l'image (format jpeg) dans un tableau
    QByteArray baImage;
    QBuffer bufImage(&baImage);
    ok = labelImage->pixmap()->save(&bufImage, "jpeg", 100);
    if (!ok)
        qWarning("Probleme de compression de l'image (sauvegarderImage - Image.cpp)");

    // Ecriture de l'image dans le fichier

    // Ecriture du titre
    //quint16 tailleTitre = titre.size();
    out<< titre;
    //file.write((char *)&tailleTitre, sizeof(quint16));
    //file.write((char *)titre.data(), tailleTitre*sizeof(QChar));
    // Ajout de l'image
    out << baImage;
    //quint32 tailleImage = baImage.size();
    //file.write((char *)&tailleImage, sizeof(quint32));
   // file.write(baImage.data(), tailleImage);
}
/********************************************************************/
/* Un bouton de la souris vient d'etre enfonce                      */
/********************************************************************/	
void Image::mousePressEvent(QMouseEvent *event)
{
	// Si l'utilisateur a clique avec la bouton gauche et que l'outil main est selectionne
	if (event->button() == Qt::LeftButton && G_outilCourant == BarreOutils::main)
	{
		// Le deplacement est autorise
		deplacementAutorise = true;
		// On memorise la position du point de depart
		pointDepart = event->pos();
		// On releve les valeurs des barres de defilement
		horizontalDepart = horizontalScrollBar()->value();
		verticalDepart = verticalScrollBar()->value();
	}
}

/********************************************************************/
/* Relache d'un bouton de la souris                                 */
/********************************************************************/	
void Image::mouseReleaseEvent(QMouseEvent *event)
{
	// Si le bouton gauche est relache on interdit le deplacement de la carte
	if (event->button() == Qt::LeftButton)
		deplacementAutorise = false;
}

/********************************************************************/
/* Deplacement de la souris                                         */
/********************************************************************/	
void Image::mouseMoveEvent(QMouseEvent *event)
{
	// Si le deplacement est autorise
	if (deplacementAutorise)
	{
		// On calcule la cifference de position entre le depart et maintenant
		QPoint diff = pointDepart - event->pos();
		// On change la position des barres de defilement
		horizontalScrollBar()->setValue(horizontalDepart + diff.x());
		verticalScrollBar()->setValue(verticalDepart + diff.y());
	}
}
		
/********************************************************************/
/* Changement du pointeur de souris pour l'outil main               */
/********************************************************************/	
void Image::pointeurMain()
{
	labelImage->setCursor(Qt::OpenHandCursor);
}

/********************************************************************/
/* Changement du pointeur de souris pour les autres outils          */
/********************************************************************/	
void Image::pointeurNormal()
{
	labelImage->setCursor(Qt::ForbiddenCursor);
}

