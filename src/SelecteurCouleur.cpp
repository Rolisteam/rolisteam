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

#include "constantesGlobales.h"
#include "SelecteurCouleur.h"
#include "variablesGlobales.h"


/********************************************************************/
/* Variables globales utilisees par tous les elements de            */
/* l'application                                                    */
/********************************************************************/	
// Definit la couleur courante
couleurSelectionee G_couleurCourante;
// Intensite du masque affichant ou masquant le plan
QColor G_couleurMasque;


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
SelecteurCouleur::SelecteurCouleur(QWidget *parent)
	: QWidget(parent)
{
	// L'utilisateur n'a pas encore clique sur le selecteur de couleur
	boutonEnfonce = false;
	
	// Initialisation des couleurs de base
	int rouge[48] = {255,255,128,0,128,0,255,255, 255,255,128,0,0,0,128,255, 128,255,0,0,0,128,128,255, 128,255,0,0,0,0,128,128, 64,128,0,0,0,0,64,64, 0,128,128,128,64,192,64,255};
	int vert[48] = {128,255,255,255,255,128,128,128, 0,255,255,255,255,128,128,0, 64,128,255,128,64,128,0,0, 0,128,128,128,0,0,0,0, 0,64,64,64,0,0,0,0, 0,128,128,128,128,192,64,255};
	int bleu[48] = {128,128,128,128,255,255,192,255, 0,0,0,64,255,192,192,255, 64,64,0,128,128,255,64,128, 0,0,0,64,255,160,128,255, 0,0,0,64,128,64,64,128, 0,0,64,128,128,192,64,255};

	// Creation du layout principale
	QVBoxLayout *selecteurLayout = new QVBoxLayout(this);
	selecteurLayout->setMargin(2);
	selecteurLayout->setSpacing(1);

	// Creation du frame contenant la couleur actuelle
	couleurActuelle = new QLabel(this);
	couleurActuelle->setFrameStyle(QFrame::Panel | QFrame::Raised);
	couleurActuelle->setLineWidth(1);
	couleurActuelle->setMidLineWidth(1);
	couleurActuelle->setFixedSize(45,40);
	couleurActuelle->setPalette(QPalette(QColor(rouge[0],vert[0],bleu[0])));
	couleurActuelle->setToolTip(tr("Couleur prédéfinie 1"));
	couleurActuelle->setAutoFillBackground(true);
	couleurActuelle->setScaledContents(true);

	// Mise a jour de la variable globale
	G_couleurCourante.type = qcolor;
	G_couleurCourante.color = QColor(rouge[0],vert[0],bleu[0]);

	// Ajout de la couleur actuelle au layout principal
	selecteurLayout->addWidget(couleurActuelle);
	selecteurLayout->setAlignment(couleurActuelle, Qt::AlignHCenter | Qt::AlignTop);
			
	// Création du layout de la grille de couleurs predefinies
	QGridLayout *grillePredef = new QGridLayout();
	grillePredef->setSpacing(1);
	grillePredef->setMargin(1);
	// Ajout de la grille de couleurs predefinies dans le layout principal
	selecteurLayout->addLayout(grillePredef);

	// Creation des widgets pour les couleurs predefinies
	int i=0, x=0, y=0;
	for (; i<48; i++)
	{
		// Initialisation de la couleur
		QColor couleur(rouge[i],vert[i],bleu[i]);
		
		// Creation d'un widget de couleur unie
		couleurPredefinie[i] = new QWidget(this);
		couleurPredefinie[i]->setPalette(QPalette(couleur));
		couleurPredefinie[i]->setAutoFillBackground(true);
		couleurPredefinie[i]->setFixedHeight(5);
		couleurPredefinie[i]->setToolTip(tr("Couleur prédéfinie ") + QString::number(i+1));

		// Mise a jour des couleurs standard de QColorDialog
		QColorDialog::setStandardColor(x*6+y, couleur.rgb());
		
		// Ajout du widget au layout
		grillePredef->addWidget(couleurPredefinie[i], y, x);

		x++;
		y = x>=8?y+1:y;
		x = x>=8?0:x;
	}

	// Ajout d'un separateur entre les couleurs predefinies et les couleurs personnelles
	separateur1 = new QWidget(this);
	separateur1->setMaximumHeight(2);
	selecteurLayout->addWidget(separateur1);

	// Création du layout de la grille de couleurs personnelles
	QGridLayout *grillePerso = new QGridLayout();
	grillePerso->setSpacing(1);
	grillePerso->setMargin(1);
	// Ajout de la grille de couleurs personnelles dans le layout principal
	selecteurLayout->addLayout(grillePerso);

	// Creation des widgets pour les couleurs personnelles
	for (i=0, x=0, y=7; i<16; i++)
	{
		// Creation d'un widget de couleur blanche
		couleurPersonnelle[i] = new QWidget(this);
		couleurPersonnelle[i]->setAutoFillBackground(true);
		couleurPersonnelle[i]->setFixedHeight(5);
		couleurPersonnelle[i]->setToolTip(tr("Couleur personnelle ") + QString::number(i+1));

		// Mise a jour des couleurs personnelles de QColorDialog dans le cas ou la variable d'initialisation est utilisable
		if (G_initialisation.initialisee == true)
			QColorDialog::setCustomColor(i, G_initialisation.couleurPersonnelle[i].rgb());
		
		// Sinon on met une couleur par defaut
		else
			QColorDialog::setCustomColor(i, QColor(i*16, i*16, i*16).rgb());

		// Ajout du widget au layout
		grillePerso->addWidget(couleurPersonnelle[i], y, x);

		x++;
		y = x>=8?y+1:y;
		x = x>=8?0:x;
	}

	// On met a jour les widgets des couleurs personnelles
	majCouleursPersonnelles();

	// Ajout d'un separateur entre les couleurs personnelles et les couleurs speciales
	separateur2 = new QWidget(this);
	separateur2->setMaximumHeight(3);
	selecteurLayout->addWidget(separateur2);

	// Création du layout des couleurs speciales
	QHBoxLayout *couleursSpeciales = new QHBoxLayout();
	couleursSpeciales->setSpacing(1);
	couleursSpeciales->setMargin(0);
	// Ajout du layout des couleurs specuales dans le layout principal
	selecteurLayout->addLayout(couleursSpeciales);

	// Ajout de la couleur speciale qui efface
	couleurEfface = new QLabel(this);
	couleurEfface->setFrameStyle(QFrame::Box | QFrame::Raised);
	couleurEfface->setLineWidth(0);
	couleurEfface->setMidLineWidth(1);
	couleurEfface->setFixedHeight(15);
        efface_pix = new QPixmap(":/resources/icones/efface.png");
	couleurEfface->setPixmap(*efface_pix);
	couleurEfface->setScaledContents(true);
	couleurEfface->setToolTip(tr("Effacer"));
	couleurEfface->setPalette(QPalette(Qt::white));
	couleurEfface->setAutoFillBackground(true);
	couleursSpeciales->addWidget(couleurEfface);
	
	// Ajout de la couleur speciale qui masque
	couleurMasque = new QLabel(this);
	couleurMasque->setFrameStyle(QFrame::Box | QFrame::Raised);
	couleurMasque->setLineWidth(0);
	couleurMasque->setMidLineWidth(1);
	couleurMasque->setFixedHeight(15);
        masque_pix = new QPixmap(":/resources/icones/masque.png");
	couleurMasque->setPixmap(*masque_pix);
	couleurMasque->setScaledContents(true);
	couleurMasque->setPalette(QPalette(Qt::white));
	couleurMasque->setAutoFillBackground(true);
	couleursSpeciales->addWidget(couleurMasque);

	// Ajout de la couleur speciale qui demasque
	couleurDemasque = new QLabel(this);
	couleurDemasque->setFrameStyle(QFrame::Box | QFrame::Raised);
	couleurDemasque->setLineWidth(0);
	couleurDemasque->setMidLineWidth(1);
	couleurDemasque->setFixedHeight(15);
        demasque_pix = new QPixmap(":/resources/icones/demasque.png");
	couleurDemasque->setPixmap(*demasque_pix);
	couleurDemasque->setScaledContents(true);
	couleurDemasque->setPalette(QPalette(Qt::white));
	couleurDemasque->setAutoFillBackground(true);
	couleursSpeciales->addWidget(couleurDemasque);

	// Taille de la palette
	setFixedHeight(126);
	//setMaximumWidth((TAILLE_ICONES+7)*2);

	// On autorise ou pas la selection des couleurs de masquage/demasquage en fonction de la nature de l'utilisateur (MJ/joueur)
	autoriserOuInterdireCouleurs();
}

/********************************************************************/		
/* Autorise ou pas la selection des couleurs de masquage et         */
/* demasquage selon que l'utilisateur local est MJ ou joueur        */
/********************************************************************/		
void SelecteurCouleur::autoriserOuInterdireCouleurs()
{
	// L'utilisateur est un joueur
	if (G_joueur)
	{
		// Le masquage est total
		G_couleurMasque = QColor(0,0,0);
		// Message d'interdiction pour les couleurs de masquage et de demasquage
		couleurMasque->setToolTip(tr("Masquer (MJ seulement)"));
		couleurDemasque->setToolTip(tr("Démasquer (MJ seulement)"));
		// Il est impossible de selectionner les couleurs de masquage et de demasquage
		couleurMasque->setEnabled(false);
		couleurDemasque->setEnabled(false);
	}

	// L'utilisateur est un MJ
	else
	{
		// Le masque est transparent
		G_couleurMasque = QColor(50,50,50);
		// Tooltip normaux pour les couleurs de masquage et de demasquage
		couleurMasque->setToolTip(tr("Masquer"));
		couleurDemasque->setToolTip(tr("Démasquer"));
	}
}

/********************************************************************/
/* L'utilisateur a clique dans le selecteur de couleur              */
/********************************************************************/		
void SelecteurCouleur::mousePressEvent(QMouseEvent *event)
{
	// Si le bouton gauche est enfonce...
	if (event->button() == Qt::LeftButton)
	{
		boutonEnfonce = true;
		clicUtilisateur(event->pos());
	}
}

/********************************************************************/
/* L'utilisateur a clique dans le selecteur de couleur et bouge la  */
/* souris                                                           */
/********************************************************************/		
void SelecteurCouleur::mouseMoveEvent(QMouseEvent *event)
{
	// Si le bouton gauche est enfonce...
	if (boutonEnfonce)
		clicUtilisateur(event->pos(), true);
}

/********************************************************************/
/* L'utilisateur relache le bouton de la souris                     */
/********************************************************************/		
void SelecteurCouleur::mouseReleaseEvent(QMouseEvent *event)
{
	// Si le bouton gauche est relache...
	if (event->button() == Qt::LeftButton)
		boutonEnfonce = false;
}

/********************************************************************/
/* Change la couleur actuelle pour celle du widget selectionne. Le  */
/* parametre move indique si l'utilisateur vient de deplacer la     */
/* souris ou s'il s'agit d'un simple clic                           */
/********************************************************************/
void SelecteurCouleur::clicUtilisateur(QPoint positionSouris, bool move)
{
	QWidget *enfant = childAt(positionSouris);
	
	// La souris ne se trouvait pas sur une couleur : on quitte
	if (enfant == 0 || enfant == separateur1 || enfant == separateur2)
		return;
	
	// La souris se trouvait sur la couleur actuelle : on ouvre QColorDialog
	// uniquement accessible par simple clic (pas de mouvement de souris)
	if (enfant == couleurActuelle && !move)
	{
		QColor couleur = QColorDialog::getColor((couleurActuelle->palette()).color(QPalette::Window));

		// Si l'utilisateur a clique sur OK on recupere la nouvelle couleur
		if (couleur.isValid())
		{
			// Mise a jour du widget affichant la couleur
			couleurActuelle->clear();
			couleurActuelle->setPalette(QPalette(couleur));
			// Mise a jour de la variable globale
			G_couleurCourante.type = qcolor;
			G_couleurCourante.color = couleur;
			// Mise a jour de la bulle d'aide
			couleurActuelle->setToolTip(tr("Rouge") + " : " + QString::number(couleur.red()) +
				" / " + tr("Vert") + " : " + QString::number(couleur.green()) +
				" / " + tr("Bleu") + " : " + QString::number(couleur.blue()));
		}

		// Recuperation des couleurs personnelles
		#ifdef WIN32
			majCouleursPersonnelles();
		#endif
		return;
	}

	// La souris se trouvait sur la couleur speciale d'effacement
	if (enfant == couleurEfface)
	{
		// Mise a jour du widget affichant la couleur
		couleurActuelle->setPixmap(*efface_pix);
		couleurActuelle->setPalette(QPalette(Qt::white));
		// Mise a jour de la variable globale
		G_couleurCourante.type = efface;
		// Mise a jour de la bulle d'aide
		couleurActuelle->setToolTip(enfant->toolTip());
		return;
	}
	
	// La souris se trouvait sur la couleur speciale de masquage
	if (enfant == couleurMasque)
	{
		// Si l'utilisateur est un joueur (et non un MJ) il n'a pas le droit de selectionner cette couleur
		if (G_joueur)
			return;
		// Mise a jour du widget affichant la couleur
		couleurActuelle->setPixmap(*masque_pix);
		couleurActuelle->setPalette(QPalette(Qt::white));
		// Mise a jour de la variable globale
		G_couleurCourante.type = masque;
		// Mise a jour de la bulle d'aide
		couleurActuelle->setToolTip(enfant->toolTip());
		return;
	}
	
	// La souris se trouvait sur la couleur speciale de demasquage
	if (enfant == couleurDemasque)
	{
		// Si l'utilisateur est un joueur (et non un MJ) il n'a pas le droit de selectionner cette couleur
		if (G_joueur)
			return;
		// Mise a jour du widget affichant la couleur
		couleurActuelle->setPixmap(*demasque_pix);
		couleurActuelle->setPalette(QPalette(Qt::white));
		// Mise a jour de la variable globale
		G_couleurCourante.type = demasque;
		// Mise a jour de la bulle d'aide
		couleurActuelle->setToolTip(enfant->toolTip());
		return;
	}
	
	// La souris se trouvait sur une couleur : on l'utilise pour couleurActuelle
	couleurActuelle->clear();
	couleurActuelle->setPalette(enfant->palette());
	// Mise a jour de la variable globale
	G_couleurCourante.type = qcolor;
	G_couleurCourante.color = (enfant->palette()).color(QPalette::Window);
	// Mise a jour de la bulle d'aide
	couleurActuelle->setToolTip(enfant->toolTip());
}

/********************************************************************/
/* Change la couleur actuelle                                       */
/********************************************************************/		
void SelecteurCouleur::changeCouleurActuelle(QColor couleur)
{
	// M.a.j du widget affichant la couleur actuelle
	couleurActuelle->clear();
	couleurActuelle->setPalette(QPalette(couleur));
	// M.a.j de la bulle d'aide
	couleurActuelle->setToolTip(tr("Rouge") + " : " + QString::number(couleur.red()) +
		" / " + tr("Vert") + " : " + QString::number(couleur.green()) +
		" / " + tr("Bleu") + " : " + QString::number(couleur.blue()));
	// M.a.j de la variable globale
	G_couleurCourante.type = qcolor;
	G_couleurCourante.color = couleur;
}

/********************************************************************/	
/* M.a.j des couleurs personnelles                                  */
/********************************************************************/	
void SelecteurCouleur::majCouleursPersonnelles()
{
	for (int i=0, j=0; i<16; i++)
	{
		couleurPersonnelle[i]->setPalette(QPalette(QColor(QColorDialog::customColor(j))));
		j+=2;
		j = j<=15?j:1;
	}
}

/********************************************************************/	
/* Renvoie la couleur personnelle dont le numero est passe en       */
/* parametre                                                        */
/********************************************************************/	
QColor SelecteurCouleur::donnerCouleurPersonnelle(int numero)
{
	int numCouleur;

	// On fait la conversion
	if (numero%2)
		numCouleur = (numero-1)/2+8;
	else
		numCouleur = numero/2;

	return (couleurPersonnelle[numCouleur]->palette()).color(QPalette::Window);
}


#ifdef MACOS
	/********************************************************************/		
	/* L'utilisateur vient d'effectuer un double clic (Mac seulement)   */
	/********************************************************************/		
	void SelecteurCouleur::mouseDoubleClickEvent (QMouseEvent *event)
	{
		// On recupere le widget present sous la souris
		QPoint position = event->pos();
		QWidget *enfant = childAt(position);
		
		// La souris ne se trouve pas sur les couleurs personnelles : on quitte
		if (position.y() < separateur1->pos().y() || position.y() > separateur2->pos().y())
			return;

		// La souris ne se trouve pas sur une couleur : on quitte
		if (enfant == 0 || enfant == separateur1 || enfant == separateur2)
			return;
		
		// Dans le cas contraire on se trouve sur une couleur personnelle

		// On demande a l'utilisateur de choisir une couleur
		QColor couleur = QColorDialog::getColor((enfant->palette()).color(QPalette::Window));

		// Si l'utilisateur a clique sur OK on recupere la nouvelle couleur
		if (couleur.isValid())
		{
			// Mise a jour du widget affichant la couleur personnelle concernee
			enfant->setPalette(QPalette(couleur));
			// Mise a jour du widget affichant la couleur actuelle
			couleurActuelle->clear();
			couleurActuelle->setPalette(QPalette(couleur));
			// Mise a jour de la variable globale
			G_couleurCourante.type = qcolor;
			G_couleurCourante.color = couleur;
			// Mise a jour de la bulle d'aide
			couleurActuelle->setToolTip(tr("Rouge") + " : " + QString::number(couleur.red()) +
				" / " + tr("Vert") + " : " + QString::number(couleur.green()) +
				" / " + tr("Bleu") + " : " + QString::number(couleur.blue()));
		}
	}
#endif


