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

#include "NouveauPlanVide.h"
#include "variablesGlobales.h"


// Definition des tableaux static contenant les formats et dimensions des plans
quint16 NouveauPlanVide::largeur[3][4] = { {600, 800, 1000, 1200}, {450, 600, 750, 900}, {500, 700, 900, 1100} };
quint16 NouveauPlanVide::hauteur[3][4] = { {450, 600, 750, 900}, {600, 800, 1000, 1200}, {500, 700, 900, 1100} };


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
NouveauPlanVide::NouveauPlanVide(QWidget *parent)
	: QDialog(parent)
{
	// Initialisation des variables
	format = 0;
	dimensions = 1;
	// Connexion de la demande de creation d'un plan du widget vers le mainWindow
	QObject::connect(this, SIGNAL(creerNouveauPlanVide(QString, QString, QColor, quint16, quint16, quint8)),
		G_mainWindow, SLOT(creerNouveauPlanVide(QString, QString, QColor, quint16, quint16, quint8)));

	#ifdef WIN32
		// On supprime de bouton d'aide contextuelle et on rajoute le bouton d'iconisation
		Qt::WindowFlags flags = windowFlags();
		setWindowFlags(flags ^ Qt::WindowContextHelpButtonHint ^ Qt::WindowSystemMenuHint);
	#endif
	// Creation du layout principal
	QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
	// Creation du layout de gauche
	QHBoxLayout *layoutHaut = new QHBoxLayout();
	// Creation du layout de gauche
	QVBoxLayout *layoutGauche = new QVBoxLayout();
	// Creation du layout de droite
	QVBoxLayout *layoutDroite = new QVBoxLayout();
	// Ajout du layout du haut au layout principal
	layoutPrincipal->addLayout(layoutHaut);		
	// Ajout des layout gauche et droit dans le layout du haut
	layoutHaut->addLayout(layoutGauche);
	layoutHaut->addLayout(layoutDroite);
	
	// Selecteur de titre
	QHBoxLayout *layoutTitre = new QHBoxLayout();
	QLabel *titreLabel = new QLabel(tr("Titre"));
	titrePlan = new QLineEdit();
	titrePlan->setToolTip(tr("Entrez l'intitulé du plan"));
	layoutTitre->addWidget(titreLabel);
	layoutTitre->addWidget(titrePlan);
	
	// Selecteur de couleur
	QHBoxLayout *layoutCouleur = new QHBoxLayout();
	QLabel *couleurLabel = new QLabel(tr("Couleur du fond"));
	couleurFond = new QPushButton();
	couleurFond->setFlat(true);
	couleurFond->setDefault(false);
	couleurFond->setAutoDefault(false);
	couleurFond->setStyle(new QPlastiqueStyle());
	couleurFond->setFixedSize(25,15);
	couleurFond->setPalette(QPalette(Qt::white));
	couleurFond->setToolTip(tr("Choisissez une couleur de fond"));
	couleurFond->setAutoFillBackground(true);
	#ifdef WIN32
		couleurFond->setFixedSize(25,15);
	#elif defined (MACOS)
		couleurFond->setFixedSize(30,20);
	#endif
	layoutCouleur->addWidget(couleurLabel);
	layoutCouleur->addWidget(couleurFond);
	layoutCouleur->setAlignment(couleurFond,Qt::AlignLeft | Qt::AlignHCenter);
	layoutCouleur->setStretchFactor(couleurFond, 1);
	// Connexion du bouton de selection de la couleur a l'ouverture du selecteur de couleur
	QObject::connect(couleurFond, SIGNAL(pressed()), this, SLOT(ouvrirSelecteurCouleur()));

	// Selecteur de format
	groupeFormat = new QGroupBox(tr("Format"));
	// Creation des boutons
        QRadioButton *boutonPaysage = new QRadioButton(tr("Paysage"));
        QRadioButton *boutonPortrait = new QRadioButton(tr("Portrait"));
        QRadioButton *boutonCarre = new QRadioButton(tr("Carré"));
	// Creation du groupe de boutons
	QButtonGroup *groupeBoutonsFormat = new QButtonGroup();
	groupeBoutonsFormat->addButton(boutonPaysage);
	groupeBoutonsFormat->addButton(boutonPortrait);
	groupeBoutonsFormat->addButton(boutonCarre);
	boutonPaysage->setChecked(true);
	// On donne un id a chaque bouton
	groupeBoutonsFormat->setId(boutonPaysage, 0);
	groupeBoutonsFormat->setId(boutonPortrait, 1);
	groupeBoutonsFormat->setId(boutonCarre, 2);
	// Ajout des boutons au selecteur de format
	QVBoxLayout *layoutFormat = new QVBoxLayout();
	layoutFormat->addWidget(boutonPaysage);
	layoutFormat->addWidget(boutonPortrait);
	layoutFormat->addWidget(boutonCarre);
	groupeFormat->setLayout(layoutFormat);
	// Connexion du groupe de boutons avec le changement de format du plan
	QObject::connect(groupeBoutonsFormat, SIGNAL(buttonClicked(int)), this, SLOT(changementFormat(int)));

	// Ajout des layouts et widget au layout de gauche
	QVBoxLayout *layoutIdentite = new QVBoxLayout();
	layoutGauche->addLayout(layoutTitre);
	layoutGauche->addLayout(layoutCouleur);
	layoutGauche->addWidget(groupeFormat);

	// Creation du selecteur de taille
	groupeTaille = new QGroupBox(tr("Dimensions (en pixels)"));
	// Creation des boutons
	boutonPetitPlan = new QRadioButton();
	boutonMoyenPlan = new QRadioButton();
	boutonGrandPlan = new QRadioButton();
	boutonTresGrandPlan = new QRadioButton();
	boutonPersonnalise = new QRadioButton(tr("Personnalisé"));
	// On met a jour l'intitule des boutons de dimensions
	majIntitulesBoutons();
	// Creation du groupe de boutons
	QButtonGroup *groupeBoutonsTaille = new QButtonGroup();
	groupeBoutonsTaille->addButton(boutonPetitPlan);
	groupeBoutonsTaille->addButton(boutonMoyenPlan);
	groupeBoutonsTaille->addButton(boutonGrandPlan);
	groupeBoutonsTaille->addButton(boutonTresGrandPlan);
	groupeBoutonsTaille->addButton(boutonPersonnalise);
	// On donne un id a chaque bouton
	groupeBoutonsTaille->setId(boutonPetitPlan, 0);
	groupeBoutonsTaille->setId(boutonMoyenPlan, 1);
	groupeBoutonsTaille->setId(boutonGrandPlan, 2);
	groupeBoutonsTaille->setId(boutonTresGrandPlan, 3);
	groupeBoutonsTaille->setId(boutonPersonnalise, 4);
	boutonMoyenPlan->setChecked(true);
	// Connexion du groupe de boutons avec le changement de dimensions du plan
	QObject::connect(groupeBoutonsTaille, SIGNAL(buttonClicked(int)), this, SLOT(changementDimensions(int)));
	// Creation de la zone de personnalisation de la taille
	taillePersonnalisee = new QWidget();
	QLabel *labelLargeur = new QLabel(tr("Largeur"));
	largeurPlan = new QLineEdit();
	largeurPlan->setMaxLength(5);
	QLabel *labelHauteur = new QLabel(tr("Hauteur"));
	hauteurPlan = new QLineEdit();
	hauteurPlan->setMaxLength(5);
	#ifdef WIN32
		largeurPlan->setFixedWidth(38);
		hauteurPlan->setFixedWidth(38);
	#elif defined (MACOS)
		largeurPlan->setFixedWidth(50);
		largeurPlan->setFixedHeight(20);
		hauteurPlan->setFixedWidth(50);
		hauteurPlan->setFixedHeight(20);
	#endif
	QVBoxLayout *layoutTaillePersonnalisee = new QVBoxLayout(taillePersonnalisee);
	QHBoxLayout *layoutLargeur = new QHBoxLayout();
	QHBoxLayout *layoutHauteur = new QHBoxLayout();
	layoutLargeur->addWidget(labelLargeur);
	layoutLargeur->addWidget(largeurPlan);
	layoutHauteur->addWidget(labelHauteur);
	layoutHauteur->addWidget(hauteurPlan);
	layoutTaillePersonnalisee->addLayout(layoutLargeur);
	layoutTaillePersonnalisee->addLayout(layoutHauteur);
	// Selecteur de taille personnalise non selectionnable
	taillePersonnalisee->setEnabled(false);
	// Ajout des boutons et de la zone de personnalisation au selecteur de taille
	QVBoxLayout *layoutTaille = new QVBoxLayout();
	layoutTaille->addWidget(boutonPetitPlan);
	layoutTaille->addWidget(boutonMoyenPlan);
	layoutTaille->addWidget(boutonGrandPlan);
	layoutTaille->addWidget(boutonTresGrandPlan);
	layoutTaille->addWidget(boutonPersonnalise);
	layoutTaille->addWidget(taillePersonnalisee);		
	groupeTaille->setLayout(layoutTaille);
	
	// Ajout au layout de droite
	layoutDroite->addWidget(groupeTaille);

	// Creation des boutons de validation
	QPushButton *boutonOK = new QPushButton(tr("OK"));
	QPushButton *boutonAnnuler = new QPushButton(tr("Annuler"));
	boutonOK->setFixedWidth(80);
	boutonAnnuler->setFixedWidth(80);
	boutonOK->setDefault(true);
	boutonOK->setAutoDefault(true);
	QHBoxLayout *layoutBoutons = new QHBoxLayout();
	layoutBoutons->addWidget(boutonOK);
	layoutBoutons->addWidget(boutonAnnuler);
	layoutBoutons->setAlignment(boutonOK, Qt::AlignRight | Qt::AlignHCenter);
	layoutBoutons->setAlignment(boutonAnnuler, Qt::AlignLeft | Qt::AlignHCenter);
	// Connexion du bouton OK a la demande de connexion
	QObject::connect(boutonOK, SIGNAL(clicked()), this, SLOT(validerDimensions()));
	// Connexion du bouton Annuler a la fermeture de la fenetre
	QObject::connect(boutonAnnuler, SIGNAL(clicked()), G_mainWindow, SLOT(aucunNouveauPlanVide()));
	// Ajout au layout principal
	layoutPrincipal->addLayout(layoutBoutons);

	// M.a.j du titre de la fenetre
	setWindowTitle(tr("Nouveau plan vide"));
	// On bloque la souris et le clavier sur cette fenetre
	setModal(true);
	// On fixe les dimensions de la fenetre
	#ifdef WIN32
		setFixedSize(354, 271);
		groupeTaille->setFixedSize(176, 218);
	#elif defined (MACOS)
		setFixedSize(410, 300);
		groupeTaille->setFixedSize(200, 218);
	#endif
	groupeFormat->setFixedSize(150, 100);
	// Affichage de la fenetre
	move(G_mainWindow->width()/2 - 175, G_mainWindow->height()/2 - 130);
	show();
}

/********************************************************************/	
/* Affiche le selecteur de couleur et m.a.j la couleur du bouton de */
/* selection de couleur du fond                                     */
/********************************************************************/	
void NouveauPlanVide::ouvrirSelecteurCouleur()
{
	// Ouverture du selecteur de couleur
	QColor couleur = QColorDialog::getColor((couleurFond->palette()).color(QPalette::Window));

	// Si l'utilisateur a clique sur OK on m.a.j la couleur du bouton
	if (couleur.isValid())
			couleurFond->setPalette(QPalette(couleur));

	// On recopie les couleurs personnelles
	#ifdef WIN32
		G_mainWindow->majCouleursPersonnelles();
	#endif
}

/********************************************************************/	
/* Le format du plan vient de changer                               */
/********************************************************************/	
void NouveauPlanVide::changementFormat(int bouton)
{
	// On met a jour d'indice du format
	format = bouton;
	// On met a jour l'intitule des boutons de dimensions
	majIntitulesBoutons();
}

/********************************************************************/	
/* M.a.j les intitules des boutons de dimension en fonction du      */
/* format selectionne                                               */
/********************************************************************/	
void NouveauPlanVide::majIntitulesBoutons()
{
	boutonPetitPlan->setText(tr("Petit (") + QString::number(largeur[format][0]) + " x " + QString::number(hauteur[format][0]) + ")");
	boutonMoyenPlan->setText(tr("Moyen (") + QString::number(largeur[format][1]) + " x " + QString::number(hauteur[format][1]) + ")");
	boutonGrandPlan->setText(tr("Grand (") + QString::number(largeur[format][2]) + " x " + QString::number(hauteur[format][2]) + ")");
	boutonTresGrandPlan->setText(tr("Très grand (") + QString::number(largeur[format][3]) + " x " + QString::number(hauteur[format][3]) + ")");		
}

/********************************************************************/	
/* Les dimensions du plan viennent de changer                       */
/********************************************************************/	
void NouveauPlanVide::changementDimensions(int bouton)
{
	// S'il ne s'agit pas du bouton "personnalise"
	if (bouton != 4)
	{
		// On met a jour l'indice des dimensions
		dimensions = bouton;
		// On masque la zone de personnalisation
		taillePersonnalisee->setEnabled(false);
		// On affiche les choix de format
		groupeFormat->setEnabled(true);
	}
	else
	{
		// On affiche la zone de personnalisation
		taillePersonnalisee->setEnabled(true);
		// On masque les choix de format
		groupeFormat->setEnabled(false);
	}			
}

/********************************************************************/	
/* L'utilisateur vient de cliquer sur OK, la fonction emet un       */
/* signal avec les dimensions du nouveau plan                       */
/********************************************************************/	
void NouveauPlanVide::validerDimensions()
{
	quint16 larg, haut;

	// Si la taille est personnalisee
	if (boutonPersonnalise->isChecked())
	{
		bool ok1, ok2;
		quint32 larg32 = largeurPlan->text().toInt(&ok1);
		quint32 haut32 = hauteurPlan->text().toInt(&ok2);
		// Impossible de convertir les champs largeur et hauteur en int
		if (!ok1 || !ok2 || larg32<1 || larg32>65535 || haut32<1 || haut32>65535)
		{
			// Creation de la boite d'alerte
			QMessageBox msgBox(this);
			msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setWindowTitle(tr("Paramètre invalide"));
			msgBox.move(pos() + QPoint(50,100));
			// On supprime l'icone de la barre de titre
			Qt::WindowFlags flags = msgBox.windowFlags();
			msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
			msgBox.setText(tr("La hauteur et la largeur doivent\nêtre compris entre 1 et 65535"));
			msgBox.exec();

			return;
		}
		larg = larg32;
		haut = haut32;
	}
	
	// Si la taille est predefinie
	else
	{
		larg = largeur[format][dimensions];
		haut = hauteur[format][dimensions];
	}

	// Recuperation du titre
	QString titre = titrePlan->text();
	// Creation de l'identifiant
	QString idCarte = QUuid::createUuid().toString();
	// On recupere la taille des PJ
	quint8 taillePj = G_listeUtilisateurs->taillePj();
	// On recupere la couleur du fond
	QColor couleur = (couleurFond->palette()).color(QPalette::Window);

	// Emission de la demande de creation d'un plan vide
	// Taille des donnees
	quint32 tailleCorps =
		// Taille du nom
		sizeof(quint16) + titre.size()*sizeof(QChar) +
		// Taille de l'identifiant
		sizeof(quint8) + idCarte.size()*sizeof(QChar) +
		// Taille de la couleur
		sizeof(QRgb) +
		// Taille des dimensions de la carte
		sizeof(quint16) + sizeof(quint16) +
		// Taille des PJ
		sizeof(quint8);

	// Buffer d'emission
	char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

	// Creation de l'entete du message
	enteteMessage *uneEntete;
	uneEntete = (enteteMessage *) donnees;
	uneEntete->categorie = plan;
	uneEntete->action = nouveauPlanVide;
	uneEntete->tailleDonnees = tailleCorps;
	
	// Creation du corps du message
	int p = sizeof(enteteMessage);
	// Ajout du titre
	quint16 tailleTitre = titre.size();
	memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
	p+=sizeof(quint16);
	memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
	p+=tailleTitre*sizeof(QChar);
	// Ajout de l'identifiant
	quint8 tailleId = idCarte.size();
	memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
	p+=sizeof(quint8);
	memcpy(&(donnees[p]), idCarte.data(), tailleId*sizeof(QChar));
	p+=tailleId*sizeof(QChar);
	// Ajout de la couleur
	QRgb rgb = couleur.rgb();
	memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
	p+=sizeof(QRgb);
	// Ajout de la largeur et de la hauteur de la carte
	memcpy(&(donnees[p]), &larg, sizeof(quint16));
	p+=sizeof(quint16);
	memcpy(&(donnees[p]), &haut, sizeof(quint16));
	p+=sizeof(quint16);
	// Ajout de la taille des PJ
	memcpy(&(donnees[p]), &taillePj, sizeof(quint8));
	p+=sizeof(quint8);		

	// On emet vers les clients ou le serveur
	emettre(donnees, tailleCorps + sizeof(enteteMessage));
	// Liberation du buffer d'emission
	delete[] donnees;

	// On cree le plan en local (ce qui a pour consequence de detruire la fenetre de nouveau plan)
	emit creerNouveauPlanVide(titre, idCarte, couleur, larg, haut, taillePj);

}

