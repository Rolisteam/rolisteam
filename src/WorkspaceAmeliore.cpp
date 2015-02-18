/*
	Rolistik - logiciel collaboratif d'aide aux jeux de roles en ligne
	Copyright (C) 2007 - Romain Campioni  Tous droits rservs.

	Ce programme est un logiciel libre ; vous pouvez le redistribuer ou le
	modifier suivant les termes de la GNU General Public License telle que
	publie par la Free Software Foundation : soit la version 2 de cette
	licence, soit ( votre gr) toute version ultrieure.

	Ce programme est distribu dans lespoir quil vous sera utile, mais SANS
	AUCUNE GARANTIE : sans mme la garantie implicite de COMMERCIALISABILIT
	ni dADQUATION  UN OBJECTIF PARTICULIER. Consultez la Licence Gnrale
	Publique GNU pour plus de dtails.

	Vous devriez avoir reu une copie de la Licence Gnrale Publique GNU avec
	ce programme ; si ce nest pas le cas, consultez :
	<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>

	Par ailleurs ce logiciel est gratuit et ne peut en aucun cas tre
	commercialis, conformment  la "FMOD Non-Commercial License".
*/


	#include <QtGui>

	#include "WorkspaceAmeliore.h"
	#include "constantesGlobales.h"


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
    WorkspaceAmeliore::WorkspaceAmeliore(QWidget *parent)
        : QWorkspace(parent)
    {
                // Nom du fichier image utilisateur, qui peut etre utilise pour le fond
		#ifdef WIN32
			QString fichierImage = QString(NOM_APPLICATION) + ".bmp";
		#elif defined (MACOS)
                        QString fichierImage = QDir::homePath() + "/." + QString(NOM_APPLICATION) + "/" + QString(NOM_APPLICATION) + ".bmp";
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
			#elif defined (MACOS)
                                imageFond = new QImage(":/resources/icones/fond workspace macos.bmp");
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
