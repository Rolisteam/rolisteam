/*
	Rolistik - logiciel collaboratif d'aide aux jeux de roles en ligne
	Copyright (C) 2007 - Romain Campioni  Tous droits réservés.

	Ce programme est un logiciel libre ; vous pouvez le redistribuer ou le
	modifier suivant les termes de la GNU General Public License telle que
	publiée par la Free Software Foundation : soit la version 2 de cette
	licence, soit (à votre gré) toute version ultérieure.

	Ce programme est distribué dans lespoir quil vous sera utile, mais SANS
	AUCUNE GARANTIE : sans même la garantie implicite de COMMERCIALISABILITÉ
	ni dADÉQUATION À UN OBJECTIF PARTICULIER. Consultez la Licence Générale
	Publique GNU pour plus de détails.

	Vous devriez avoir reçu une copie de la Licence Générale Publique GNU avec
	ce programme ; si ce nest pas le cas, consultez :
	<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>

	Par ailleurs ce logiciel est gratuit et ne peut en aucun cas être
	commercialisé, conformément à la "FMOD Non-Commercial License".
*/


	#include <QtGui>

	#include "CarteFenetre.h"
	#include "variablesGlobales.h"


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
    CarteFenetre::CarteFenetre(Carte *uneCarte, QWidget *parent)
        : QScrollArea(parent)
    {
		// On donne un nom a l'objet "CarteFenetre" pour le differencier des autres fenetres du workspace
		setObjectName("CarteFenetre");
		// On change l'icone de la fenetre
                setWindowIcon(QIcon(":/resources/icones/vignette plan.png"));
		// Sauvegarde de la reference de la carte
        carteAssociee = uneCarte;
		// On aligne la carte au centre de la CarteFenetre
        setAlignment(Qt::AlignCenter);
		// Association de la carte avec le scrollArea
		setWidget(carteAssociee);
		// Redimentionement de la taille du scrollArea
		#ifdef WIN32
			resize(carteAssociee->width()+2, carteAssociee->height()+2);
		#elif defined (MACOS)
			resize(carteAssociee->width()+4, carteAssociee->height()+4);
                #elif defined Q_WS_X11
                        resize(carteAssociee->width()+2, carteAssociee->height()+2);
		#endif
		// Connexion des signaux de deplacement de la Carte
		QObject::connect(carteAssociee, SIGNAL(commencerDeplacementCarteFenetre(QPoint)), this, SLOT(commencerDeplacement(QPoint)));
		QObject::connect(carteAssociee, SIGNAL(deplacerCarteFenetre(QPoint)), this, SLOT(deplacer(QPoint)));
	}

	/********************************************************************/	
	/* Destructeur                                                      */
	/********************************************************************/	
	CarteFenetre::~CarteFenetre()
	{
		// Destruction de l'action associee
		actionAssociee->~QAction();
		// On enleve la carte de la liste des cartes existantes
		G_mainWindow->enleverCarteDeLaListe(carteAssociee->identifiantCarte());
	}

	/********************************************************************/
	/* Cache la fenetre au lieu de la detruire                          */
	/********************************************************************/
	void CarteFenetre::closeEvent(QCloseEvent *event)
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
	void CarteFenetre::associerAction(QAction *action)
	{
		actionAssociee = action;
	}

	/********************************************************************/
	/* Renvoie la carte contenue dans la scrollArea                     */
	/********************************************************************/
	Carte * CarteFenetre::carte()
	{
		return carteAssociee;
	}

	/********************************************************************/
	/* Initialise le point de depart du deplacement dans la fenetre     */
	/********************************************************************/
	void CarteFenetre::commencerDeplacement(QPoint position)
	{
		// On memorise la position du point de depart
		pointDepart = position;
		// On releve les valeurs des barres de defilement
		horizontalDepart = horizontalScrollBar()->value();
		verticalDepart = verticalScrollBar()->value();
	}
	
	/********************************************************************/
	/* Deplace le contenu de la fenetre en comparant le point de depart */
	/* du deplacement a la position passee en parametre                 */
	/********************************************************************/
	void CarteFenetre::deplacer(QPoint position)
	{
		// On calcule la cifference de position entre le depart et maintenant
		QPoint diff = pointDepart - position;
		// On change la position des barres de defilement
		horizontalScrollBar()->setValue(horizontalDepart + diff.x());
		verticalScrollBar()->setValue(verticalDepart + diff.y());
	}
