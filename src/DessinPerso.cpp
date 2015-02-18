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
	#include <math.h>

	#include "DessinPerso.h"
	#include "variablesGlobales.h"


	/********************************************************************/
	/* Variables globales utilisees par tous les elements de            */
	/* l'application                                                    */
	/********************************************************************/
	// Liste des etats de sante des PJ/PNJ (initialisee dans MainWindow.cpp)
	QList<DessinPerso::etatDeSante> G_etatsDeSante;


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
	DessinPerso::DessinPerso(QWidget *parent, QString persoId, QString nom, QColor couleurPerso, int taille, QPoint position, typePersonnage leType, int numero)
        : QWidget(parent)
	{
		// Initiatialisation des variables
		nomPerso = nom;
		numeroPnj = (uchar) numero;
		couleur = couleurPerso;
		identifiant = persoId;
		orientation = QPoint(45, 0);
		visible = false;
		orientationAffichee = false;
		numeroEtat = 0;
		diametre = taille;
		etat = G_etatsDeSante[numeroEtat];
		type = leType;

		if (type == pj)
		{
			numeroAffiche = false;
			nomAffiche = G_affichageNomPj;
		}
		else
		{
			numeroAffiche = G_affichageNumeroPnj;
			nomAffiche = G_affichageNomPnj;
		}
		
		// Creation du label contenant le dessin du personnage
		disquePerso = new QLabel(this);
		// On donne un nom au label (pour pouvoir ensuite differencier les enfants de la carte)
		disquePerso->setObjectName("disquePerso");
		// M.a.j de la taille du label
		disquePerso->resize(QSize(diametre+4, diametre+4));
		// Ajout du tooltip au label
		if (type == pj)
			disquePerso->setToolTip(nomPerso + " (" + etat.nomEtat + ")");
		else
			disquePerso->setToolTip(nomPerso + (nomPerso.isEmpty()?"":" - ") + QString::number(numeroPnj) + " (" + etat.nomEtat + ")");

		// Dessin du personnage et ajout de l'image au label
		dessinerPersonnage();

		// Initialisation du tableau de points formant le contour du disque
		QImage image = disquePerso->pixmap()->toImage();
		initialiserContour(image);

		// Creation du label contenant le nom et le numero du personnage
		intitulePerso = new QLabel(this);
		// On donne un nom au label (pour pouvoir ensuite differencier les enfants de la carte)
		intitulePerso->setObjectName("intitulePerso");
		// Met l'intitule a jour et dimensionne DessinPerso en fonction
		mettreIntituleAJour();
		
		// Creation du layout
		QVBoxLayout *layout = new QVBoxLayout;
		layout->setMargin(0);
		layout->setSpacing(0);
		layout->addWidget(disquePerso);
		layout->addWidget(intitulePerso);
		layout->setAlignment(disquePerso, Qt::AlignCenter);
		layout->setAlignment(intitulePerso, Qt::AlignCenter);
		setLayout(layout);

		// Connexion des signaux
		if (type == pj)
		{
			// Connexion des demandes d'affichage/masquage des noms de PJ
			QObject::connect(parent, SIGNAL(afficherNomsPj(bool)), this, SLOT(afficherNomsPj(bool)));
			// Connexion de la demande de changement de taille du PJ
			QObject::connect(parent, SIGNAL(changerTaillePj(int)), this, SLOT(changerTaillePj(int)));
		}
		else
		{
			// Connexion des demandes d'affichage/masquage des noms et numeros de PNJ
			QObject::connect(parent, SIGNAL(afficherNomsPnj(bool)), this, SLOT(afficherNomsPnj(bool)));
			QObject::connect(parent, SIGNAL(afficherNumerosPnj(bool)), this, SLOT(afficherNumerosPnj(bool)));
		}
		
		// On donne un nom au widget (pour pouvoir ensuite differencier les enfants de la carte)
		setObjectName("DessinPerso");
		// Deplacement du widget
		move(position.x()-width()/2, position.y()-disquePerso->height()/2);
		// On memorise la position du centre
		centre = QPoint(x()+width()/2, y()+disquePerso->height()/2);
		// A la fin du constructeur le widget n'est pas affiche
		hide();
	}

	/********************************************************************/
	/* Deplace le dessin du personnage                                  */
	/********************************************************************/	
    void DessinPerso::deplacePerso(QPoint position)
    {
		move(position);
	}

	/********************************************************************/
	/* Deplace le dessin du personnage                                  */
	/********************************************************************/	
    void DessinPerso::deplacePerso(int x, int y)
    {
		move(x, y);
	}
	
	/********************************************************************/
	/* Deplace le dessin du personnage, les coordonnees sont celles du  */
	/* centre du disque reprensentant le perso                          */
	/********************************************************************/	
    void DessinPerso::deplaceCentrePerso(QPoint position)
    {
		// On calcule la nouvelle position de disquePerso par rapport a DessinPerso
		QPoint nouvellePosition = mapFromParent( position - QPoint(disquePerso->width()/2, disquePerso->height()/2) );
		// Puis on regarde de combien il a bouge par rapport a son ancienne position dans DessinPerso
		QPoint delta = disquePerso->pos() - nouvellePosition;
		// On deplace DessinPerso du delta
		move (pos()-delta);
	}

	/********************************************************************/
	/* Renvoie les coordonnees du centre du disque representant le      */
	/* personnage                                                       */
	/********************************************************************/	
    QPoint DessinPerso::positionCentrePerso()
    {
		// On recupere la position de disquePerso par rapport aux coordonnees de la carte
		// et on y ajoute la moitie de la largeur et de la hauteur de disquePerso
		return mapTo(parentWidget(), disquePerso->pos()) + QPoint(disquePerso->width()/2, disquePerso->height()/2);
	}

	/********************************************************************/
	/* Renvoie le diametre et le nom du personnage                      */
	/********************************************************************/	
	void DessinPerso::diametreCouleurNom(int *diam, QColor *coul, QString *nom)
	{
		*diam = diametre;
		*coul = couleur;
		*nom = nomPerso;
	}
	
	/********************************************************************/
	/* Renvoie true si le point se trouve dans la partie transparente   */
	/* de la pixmap de disquePerso. Le point se trouve dans l'espace de   */
	/* coordonnees de la carte (le parent direct de DessinPerso)          */
	/********************************************************************/
    bool DessinPerso::dansPartieTransparente(QPoint position)
    {
		// On translate le point dans l'espace des coordonnees de disquePerso
		QPoint positionDansDisquePerso = disquePerso->mapFrom(parentWidget(), position);
		// Recuperation de la pixmap de disquePerso et conversion en image
		QImage image = (disquePerso->pixmap())->toImage();

		return qAlpha(image.pixel(positionDansDisquePerso.x(), positionDansDisquePerso.y())) == 0;
	}

	/********************************************************************/
	/* Affiche/masque le nom du PJ                                      */
	/********************************************************************/	
	void DessinPerso::afficherNomsPj(bool afficher)
	{
		nomAffiche = afficher;
		mettreIntituleAJour();
	}

	/********************************************************************/
	/* Affiche/masque le nom du PNJ                                     */
	/********************************************************************/	
	void DessinPerso::afficherNomsPnj(bool afficher)
	{
		nomAffiche = afficher;
		mettreIntituleAJour();
	}
	
	/********************************************************************/
	/* Affiche/masque le numero du PNJ                                  */
	/********************************************************************/	
	void DessinPerso::afficherNumerosPnj(bool afficher)
	{
		numeroAffiche = afficher;
		mettreIntituleAJour();
	}

	/********************************************************************/
	/* Renvoie true si le personnage est actuellement affiche. La       */
	/* fonction isVisible() du widget n'est pas utilisee car son        */
	/* resultat depend de la visibilite de ses ancetres; hors la carte  */
	/* est parfois cachee (lorsque l'utilisateur la ferme)              */
	/********************************************************************/	
	bool DessinPerso::estVisible()
	{
		return visible;
	}

	/********************************************************************/
	/* Affiche le personnage                                            */
	/********************************************************************/	
	void DessinPerso::afficherPerso()
	{
		visible = true;
		show();
		// On remet le personnage a sa position d'avant le masquage
		deplaceCentrePerso(centre);
	}

	/********************************************************************/
	/* Cache le personnage                                              */
	/********************************************************************/	
	void DessinPerso::cacherPerso()
	{
		// On memorise la position du centre avant de cacher le personnage
		centre = positionCentrePerso();
		visible = false;
		hide();
	}

	/********************************************************************/
	/* Met a jour l'intitule du personnage, et met le DessinPerso a la  */
	/* taille adequate                                                  */
	/********************************************************************/	
	void DessinPerso::mettreIntituleAJour()
	{
		// Recuperation de la position centrale du perso
		QPoint position = positionCentrePerso();
		// recuperation de l'ancien texte du label
		QString ancienTexte = intitulePerso->text();
		
		// Creation du texte du label
		QString texteNom = nomAffiche?nomPerso:"";
		QString texteNumero = numeroAffiche?QString::number(numeroPnj):"";
		
		// Ajout du texte au label
		intitulePerso->setText(texteNom + (texteNom.isEmpty() || texteNumero.isEmpty()?"":" - ") + texteNumero);

		// Parametrage de la fonte (taille et couleur)
		QFont font;
		font.setPixelSize(12);
		intitulePerso->setFont(font);
		intitulePerso->setPalette(QPalette(couleur));
		intitulePerso->setForegroundRole(QPalette::Window);
				
		// Calcul de l'espace occupe par le texte
        QFontMetrics fm(font);
        QSize tailleTexte = fm.size(Qt::TextSingleLine, intitulePerso->text());
		int largeurWidget = tailleTexte.width()<disquePerso->width()?disquePerso->width():tailleTexte.width();

		// Redimensionnement du widget
		if ( (intitulePerso->text()).isEmpty() )
		{
			// On masque l'intitule
			intitulePerso->hide();
			// redimensionnement du widget
			resize(largeurWidget, disquePerso->height());
			// Repositionne le perso au meme endroit
			deplaceCentrePerso(position);
		}
		else
		{
			resize(largeurWidget, disquePerso->height() + tailleTexte.height());
			// Le repositionnement ne fonctionne correctement qui si on le place apres le show()
			// mais on a un clignotement a l'affichage. Si on le place avant le show() le widget s'affiche
			// plus haut qu'a la normale (il se decale progressivement). Cela est certainement du
			// au fait que l'evenement resizeEvent n'a pas lieu tant que le widget intitulePerso est cache.
			// Qt ne doit donc pas etre en mesure de calculer correctement les changements de
			// referentiel pour les coordonnees dans deplaceCentrePerso.
			// Le pb n'arrive que lorsque l'on reaffiche le label apres l'avoir masque (donc quand
			// l'acien texte est vide)
			// Pour corriger le pb on rajoute la moitie de la hauteur du texte (empirique).
			if (ancienTexte.isEmpty())
				deplaceCentrePerso( QPoint(position.x(), position.y()+tailleTexte.height()/2) );
			// Sinon on repositionne le perso au meme endroit
			else
				deplaceCentrePerso(position);
			// On affiche l'intitule
			intitulePerso->show();
		}
	}

	/********************************************************************/
	/* Initialise le tableau contenant les coordonnees (dans l'espace   */
	/* du disque) de 8 points repartis le long du contour du disque     */
	/********************************************************************/	
	void DessinPerso::initialiserContour(QImage &disque)
	{
		// Initialisation des 4 points haut, bas, gauche et droite
		contour[0] = QPoint (disque.width()/2, 0);
		contour[1] = QPoint (disque.width()/2, disquePerso->height());
		contour[2] = QPoint (0, disque.height()/2);
		contour[3] = QPoint (disquePerso->width(), disquePerso->height()/2);
				
		int x, y;
		// Initialisation du point haut gauche
                for (x=0, y=0; qAlpha(disque.pixel(x,y))!=255; x++, y++) ;
		contour[4] = QPoint(x, y);
		
		// Initialisation du point haut droite
                for (x=disque.width()-1, y=0; qAlpha(disque.pixel(x,y))!=255; x--, y++) ;
		contour[5] = QPoint(x, y);

		// Initialisation du point bas gauche
                for (x=0, y=disque.height()-1; qAlpha(disque.pixel(x,y))!=255; x++, y--) ;
		contour[6] = QPoint(x, y);
		
		// Initialisation du point bas gauche
                for (x=disque.width()-1, y=disque.height()-1; qAlpha(disque.pixel(x,y))!=255; x--, y--) ;
		contour[7] = QPoint(x, y);
	}

	/********************************************************************/
	/* Renvoie les points du contour dans l'espace de coordonnees de la */
	/* carte                                                            */
	/********************************************************************/	
	void DessinPerso::contourDisque(QPoint *coordonnees)
	{
		for (int i=0; i<8; i++)
			coordonnees[i] = disquePerso->mapTo(parentWidget(), contour[i]);
	}

	/********************************************************************/
	/* Dessine le personnage. Si positionSouris est differente de (0,0) */
	/* l'orientation du personnage est mise a jour                      */
	/********************************************************************/
	void DessinPerso::dessinerPersonnage(QPoint positionSouris)
	{
		// Si la position de la souris est passee en parametre
		if (positionSouris != QPoint(0,0))
		{
			// On met a jour l'orientation du personnage
			QPoint centre = positionCentrePerso();
			orientation =  positionSouris - centre;
		}

		// Creation de l'image contenant le dessin du PJ/PNJ
		QImage disqueImage(QSize(diametre+4, diametre+4), QImage::Format_ARGB32_Premultiplied);
		// Image transparente
		disqueImage.fill(qRgba(0, 0, 0, 0));
		// Creation du painter servant a dessiner sur l'image
		QPainter painterDisque(&disqueImage);
		painterDisque.setRenderHint(QPainter::Antialiasing);

		// Dessin d'un disque
		QPen pen;
		pen.setColor(etat.couleurEtat);
		pen.setWidth(4);
		painterDisque.setPen(pen);
		painterDisque.setBrush(couleur);
		painterDisque.drawEllipse(2,2,disqueImage.width()-4, disqueImage.height()-4);
		// Delimitation de la zone de dessin
		QRegion clip(1, 1, disqueImage.width()-2, disqueImage.height()-2, QRegion::Ellipse);
		painterDisque.setClipRegion(clip);

		// Si l'orientation doit etre affichee...
		if (orientationAffichee)
		{
			// Les 2 points de la ligne
			QPoint p1 = QPoint(diametre/2+2, diametre/2+2);
			QPoint p2 = p1 + orientation;
			// Dessin de la ligne d'orientation
			if (etat.couleurEtat.value() >= 128)
				pen.setColor(Qt::black);
			else
				pen.setColor(Qt::white);
			pen.setWidth(3);
			painterDisque.setPen(pen);
			painterDisque.drawLine(p1, p2);
			// Dessin d'un cercle au milieu du disque
			pen.setColor(couleur);
			pen.setWidth(1);
			painterDisque.setPen(pen);
			painterDisque.setBrush(couleur);
			QRectF ellipse(0, 0, (double)diametre/2, (double)diametre/2);
			ellipse.moveCenter(QPointF((double)diametre/2+2, (double)diametre/2+2));
			painterDisque.drawEllipse(ellipse);
		}

		// Si le personnage est un PJ on dessine son centre en noir
		if (type == pj)
		{
			pen.setColor(Qt::black);
			pen.setWidth(1);
			painterDisque.setPen(pen);
			painterDisque.setBrush(Qt::black);
			QRectF ellipse(0, 0, (double)diametre/3, (double)diametre/3);
			ellipse.moveCenter(QPointF((double)diametre/2+2, (double)diametre/2+2));
			painterDisque.drawEllipse(ellipse);
		}

		// Ajout de l'image au label
		disquePerso->setPixmap(QPixmap::fromImage(disqueImage));
		// Ajout du tooltip au label
		if (type == pj)
			disquePerso->setToolTip(nomPerso + " (" + etat.nomEtat + ")");
		else if (type == pnj)
			disquePerso->setToolTip(nomPerso + (nomPerso.isEmpty()?"":" - ") + QString::number(numeroPnj) + " (" + etat.nomEtat + ")");
		else
			qWarning ("Type de personnage inconnu (dessinerPersonnage - DessinPerso.cpp)");

	}

	/********************************************************************/
	/* Change l'etat de orientationAffichee                             */
	/********************************************************************/
	void DessinPerso::afficheOuMasqueOrientation()
	{
		orientationAffichee = !orientationAffichee;
		dessinerPersonnage();
	}
	
	/********************************************************************/
	/* Affiche l'orientation du personnage si afficher = true, la       */
	/* masque si afficher = false                                       */
	/********************************************************************/
	void DessinPerso::afficherOrientation(bool afficher)
	{
		orientationAffichee = afficher;
		dessinerPersonnage();
	}
	
	/********************************************************************/
	/* Change l'etat du perso (permet d'indiquer si le personnage est   */
	/* blesse, malade, mort, etc...). Dans les autres fonctions on      */
	/* utilise l'etat de sante et non le numero dans la liste, ainsi si */
	/* la liste change on evite les plantages                           */
	/********************************************************************/
	void DessinPerso::changerEtat()
	{
		numeroEtat = numeroEtat<G_etatsDeSante.size()-1?numeroEtat+1:0;
		etat = G_etatsDeSante[numeroEtat];
		dessinerPersonnage();
	}

	/********************************************************************/
	/* Renvoie true si le personnage est un PJ                          */
	/********************************************************************/
	bool DessinPerso::estUnPj()
	{
		return type == pj;
	}

	/********************************************************************/
	/* Renvoie true si l'orientation est affichee, false sinon          */
	/********************************************************************/
	bool DessinPerso::orientationEstAffichee()
	{
		return orientationAffichee;
	}

	/********************************************************************/
	/* Renvoie l'identifiant du personnage                              */
	/********************************************************************/
	QString DessinPerso::idPersonnage()
	{
		return identifiant;
	}

	/********************************************************************/
	/* Renvoie l'orientation du personnage                              */
	/********************************************************************/
    QPoint DessinPerso::orientationPersonnage()
    {
		return orientation;
	}

	/********************************************************************/
	/* Changement de la taille du PJ                                    */
	/********************************************************************/
    void DessinPerso::changerTaillePj(int nouvelleTaille)
	{
		// Recuperation de la position centrale du PJ
		QPoint position = positionCentrePerso();
		// M.a.j du diametre
		diametre = nouvelleTaille + 11;
		// M.a.j de la taille du label contenant l'image
		disquePerso->resize(QSize(diametre+4, diametre+4));
		// M.a.j du dessin du PJ
		dessinerPersonnage();
		// M.a.j de l'intitule (redimensionne egalement le widget)
		mettreIntituleAJour();
		// M.a.j des contours du disque
		QImage image = disquePerso->pixmap()->toImage();
		initialiserContour(image);
		// Deplacement du PJ en gardant le meme centre
		deplaceCentrePerso(position);
	}

	/********************************************************************/
	/* Renomme le personnage                                            */
	/********************************************************************/
	void DessinPerso::renommerPerso(QString nouveauNom)
	{
		// M.a.j du nom du perso
		nomPerso = nouveauNom;
		// M.a.j de l'intitule (redimensionne egalement le widget)
		mettreIntituleAJour();
		// M.a.j du tooltip
		if (type == pj)
			disquePerso->setToolTip(nomPerso + " (" + etat.nomEtat + ")");
		else if (type == pnj)
			disquePerso->setToolTip(nomPerso + (nomPerso.isEmpty()?"":" - ") + QString::number(numeroPnj) + " (" + etat.nomEtat + ")");
		else
			qWarning ("Type de personnage inconnu (renommerPerso - DessinPerso.cpp)");
	}

	/********************************************************************/
	/* Change la couleur du personnage                                  */
	/********************************************************************/
	void DessinPerso::changerCouleurPerso(QColor coul)
	{
		couleur = coul;
		// M.a.j du dessin du PJ
		dessinerPersonnage();
		// M.a.j de l'intitule
		mettreIntituleAJour();
	}

	/********************************************************************/
	/* L'orientation du personnage est m.a.j avec le parametre de la    */
	/* fonction (il s'agit de la difference entre le point vers lequel  */
	/* regarde le personnage, et la position de ce dernier)             */
	/********************************************************************/
	void DessinPerso::nouvelleOrientation(QPoint uneOrientation)
	{
		orientation = uneOrientation;
		// M.a.j du dessin du personnage
		dessinerPersonnage();
	}

	/********************************************************************/
	/* L'etat de sante du personnage est m.a.j avec les parametres de   */
	/* la fonction (utilisee lors du chargement d'une carte)            */
	/********************************************************************/
	void DessinPerso::nouvelEtatDeSante(etatDeSante sante, int numeroSante)
	{
		numeroEtat = numeroSante;
		etat = sante;
		dessinerPersonnage();
	}

	/********************************************************************/
	/* M.a.j l'etat de sante du personnage a partir du numero d'etat    */
	/* passe en parametre                                               */
	/********************************************************************/
	void DessinPerso::changerEtatDeSante(int numEtat)
	{
		numeroEtat = numEtat<=G_etatsDeSante.size()-1?numEtat:0;
		etat = G_etatsDeSante[numeroEtat];
		dessinerPersonnage();
	}

	/********************************************************************/
	/* Renvoie le numero de l'etat de sante dans lequel se trouve le    */
	/* personnage                                                       */
	/********************************************************************/
	int DessinPerso::numeroEtatSante()
	{
		return numeroEtat;
	}

	/********************************************************************/
	/* Emet un pnj vers les clients ou le serveur                       */
	/********************************************************************/
	void DessinPerso::emettrePnj(QString idCarte)
	{
		// Taille des donnees a emettre (sans l'entete)
		quint32 tailleCorps = sizeof(quint8) + idCarte.size()*sizeof(QChar) + tailleDonneesAEmettre();

		// Buffer d'emission
		char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

		// Creation de l'entete du message
		enteteMessage *uneEntete;
		uneEntete = (enteteMessage *) donnees;
		uneEntete->categorie = persoNonJoueur;
		uneEntete->action = ajouterPersoNonJoueur;
		uneEntete->tailleDonnees = tailleCorps;
		
		int p = sizeof(enteteMessage);
		// Ajout de l'identifiant de la carte
		quint8 tailleId = idCarte.size();
		memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
		p+=sizeof(quint8);
		memcpy(&(donnees[p]), idCarte.data(), tailleId*sizeof(QChar));
		p+=tailleId*sizeof(QChar);
		// Ajout des donnees du PNJ
		preparerPourEmission(&(donnees[p]));
		// Emission des donnees
		emettre(donnees, tailleCorps + sizeof(enteteMessage));
		delete[] donnees;
	}

	/********************************************************************/
	/* Renvoie la taille du tampon a reserver pour stocker les info     */
	/* relativent au personnage (avant l'appel a preparerPourEmission)  */
	/********************************************************************/
	int DessinPerso::tailleDonneesAEmettre()
	{
		// Taille des donnees
		quint32 tailleDonnees =
			// Taille du nom
			sizeof(quint16) + nomPerso.size()*sizeof(QChar) +
			// Taille de l'identifiant
			sizeof(quint8) + identifiant.size()*sizeof(QChar) +
			// Taille du type de personnage (PJ ou PNJ)
			sizeof(quint8) +
			// Taille du numero de PNJ
			sizeof(quint8) +
			// Taille du diametre
			sizeof(quint8) +
			// Taille de la couleur
			sizeof(QRgb) +
			// Taille de la position du centre du personnage
			sizeof(qint16) + sizeof(qint16) +
			// Taille de l'orientation du personnage
			sizeof(qint16) + sizeof(qint16) +
			// Taille de l'etat de sante
			sizeof(QRgb) + sizeof(quint16) + etat.nomEtat.size()*sizeof(QChar) +
			// Taille du numero d'etat de sante
			sizeof(quint16) +
			// Taille de l'information visible/cache
			sizeof(quint8) +
			// Taille de l'information orientation affichee/masquee
			sizeof(quint8);
		
		return tailleDonnees;
	}

	/********************************************************************/
	/* Ecrit a l'adresse passee en parametre les elements necessaires a */
	/* l'emission d'un personnage (corps du message uniquement) et      */
	/* renvoie la taille des donnees ecritent. S'il est a true le       */
	/* parametre "convertirEnPnj" entraine la convertion en PNJ de tous */
	/* les PJ presents sur la carte                                     */
	/********************************************************************/
	int DessinPerso::preparerPourEmission(char *tampon, bool convertirEnPnj)
	{							
		int p = 0;
		// Ajout du nom
		quint16 tailleNom = nomPerso.size();
		memcpy(&(tampon[p]), &tailleNom, sizeof(quint16));
		p+=sizeof(quint16);
		memcpy(&(tampon[p]), nomPerso.data(), tailleNom*sizeof(QChar));
		p+=tailleNom*sizeof(QChar);
		// Ajout de l'identifiant
		quint8 tailleId = identifiant.size();
		QString ident;
		// Si on convertit le PJ en PNJ, on change son ID pour eviter les erreurs en cas de sauvegarde/fermture/ouverture
		// de la carte (1 PJ et 1 PNJ se retrouvent avec le meme ID)
		if (convertirEnPnj)
			ident = QUuid::createUuid().toString();
		else
			ident = identifiant;
		memcpy(&(tampon[p]), &tailleId, sizeof(quint8));
		p+=sizeof(quint8);
		memcpy(&(tampon[p]), ident.data(), tailleId*sizeof(QChar));
		p+=tailleId*sizeof(QChar);
		// Ajout du type
		quint8 typeDuPerso;
		if (convertirEnPnj)
			typeDuPerso = pnj;
		else
			typeDuPerso = type;
		memcpy(&(tampon[p]), &typeDuPerso, sizeof(quint8));
		p+=sizeof(quint8);
		// Ajout du numero de PNJ
		quint8 numeroDuPnj;
		if (type==pj && convertirEnPnj)
			numeroDuPnj = 0;
		else
			numeroDuPnj = numeroPnj;
		memcpy(&(tampon[p]), &numeroDuPnj, sizeof(quint8));
		p+=sizeof(quint8);
		// Ajout du diametre
		memcpy(&(tampon[p]), &diametre, sizeof(quint8));
		p+=sizeof(quint8);
		// Ajout de la couleur
		QRgb couleurPersoRgb = couleur.rgb();
		memcpy(&(tampon[p]), &couleurPersoRgb, sizeof(QRgb));
		p+=sizeof(QRgb);
		// Ajout de la position du centre du personnage
		QPoint positionDuCentre = positionCentrePerso();
		qint16 centreX = positionDuCentre.x();
		qint16 centreY = positionDuCentre.y();
		memcpy(&(tampon[p]), &centreX, sizeof(qint16));
		p+=sizeof(qint16);
		memcpy(&(tampon[p]), &centreY, sizeof(qint16));
		p+=sizeof(qint16);
		// Ajout de l'orientation du personnage
		qint16 orientationX = orientation.x();
		qint16 orientationY = orientation.y();
		memcpy(&(tampon[p]), &orientationX, sizeof(qint16));
		p+=sizeof(qint16);
		memcpy(&(tampon[p]), &orientationY, sizeof(qint16));
		p+=sizeof(qint16);
		// Ajout de l'etat de sante (couleur)
		QRgb couleurEtatRgb = etat.couleurEtat.rgb();
		memcpy(&(tampon[p]), &couleurEtatRgb, sizeof(QRgb));
		p+=sizeof(QRgb);
		// Ajout de l'etat de sante (nom)
		quint16 tailleEtatSante = etat.nomEtat.size();
		memcpy(&(tampon[p]), &tailleEtatSante, sizeof(quint16));
		p+=sizeof(quint16);
		memcpy(&(tampon[p]), etat.nomEtat.data(), tailleEtatSante*sizeof(QChar));
		p+=tailleEtatSante*sizeof(QChar);
		// Ajout du numero d'etat de sante
		quint16 numEtatDeSante = numeroEtat;
		memcpy(&(tampon[p]), &numEtatDeSante, sizeof(quint16));
		p+=sizeof(quint16);
		// Ajout de l'information visible/cache
		quint8 persoVisible;
		persoVisible = visible;
		memcpy(&(tampon[p]), &persoVisible, sizeof(quint8));
		p+=sizeof(quint8);
		// Ajout de l'information orientation affichee/masquee
		quint8 orientationVisible = orientationAffichee;
		memcpy(&(tampon[p]), &orientationVisible, sizeof(quint8));
		p+=sizeof(quint8);
		
		// On renvoie le nbr d'octets ecrits
		return p;		
	}
