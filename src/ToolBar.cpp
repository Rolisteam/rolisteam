/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

#include "ToolBar.h"
#include "colorselector.h"
#include "SelecteurDiametre.h"

	
	

QString G_texteCourant;

QString G_nomPnjCourant;

int G_numeroPnjCourant;
	

ToolsBar::ToolsBar(QWidget *parent)
		: QDockWidget(parent)
{

    setWindowTitle(tr("Tools"));

    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    outils = new QWidget(this);

    setWidget(outils);


    creerActions();

    creerOutils();


    connect(couleur,SIGNAL(currentColorChanged(QColor&)),this,SIGNAL(currentColorChanged(QColor&)));
    connect(couleur,SIGNAL(currentModeChanged(int)),this,SIGNAL(currentModeChanged(int)));

    m_currentTool = HANDLER;


    QObject::connect(actionRazChrono, SIGNAL(triggered(bool)), this, SLOT(razNumeroPnj()));

    QObject::connect(ligneDeTexte, SIGNAL(textEdited(const QString &)), this, SLOT(texteChange(const QString &)));

    QObject::connect(nomPnj, SIGNAL(textEdited(const QString &)), this, SLOT(nomPnjChange(const QString &)));


    connect(m_toolsGroup,SIGNAL(triggered(QAction*)),this,SLOT(currentActionChanged(QAction*)));


    QObject::connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(changementTaille(bool)));

    setFloating(false);
}


void ToolsBar::autoriserOuInterdireCouleurs()
{
    couleur->allowOrForbideColors();
}


void ToolsBar::creerActions()
{
    // Creation du groupe d'action
    m_toolsGroup = new QActionGroup(this);

    // Creation des actions
            actionCrayon 	= new QAction(QIcon(":/resources/icons/crayon.png"), tr("Crayon"), m_toolsGroup);
            actionLigne 	= new QAction(QIcon(":/resources/icons/ligne.png"), tr("Ligne"), m_toolsGroup);
            actionRectVide 	= new QAction(QIcon(":/resources/icons/rectangle vide.png"), tr("Rectangle vide"), m_toolsGroup);
            actionRectPlein	= new QAction(QIcon(":/resources/icons/rectangle plein.png"), tr("Rectangle plein"), m_toolsGroup);
            actionElliVide 	= new QAction(QIcon(":/resources/icons/ellipse vide.png"), tr("Ellipse vide"), m_toolsGroup);
            actionElliPlein	= new QAction(QIcon(":/resources/icons/ellipse pleine.png"), tr("Ellipse pleine"), m_toolsGroup);
            actionTexte 	= new QAction(QIcon(":/resources/icons/texte.png"), tr("Texte"), m_toolsGroup);
            actionMain		= new QAction(QIcon(":/resources/icons/main.png"), tr("Déplacer"), m_toolsGroup);
            actionAjoutPnj 	= new QAction(QIcon(":/resources/icons/ajouter PNJ.png"), tr("Ajouter un PNJ"), m_toolsGroup);
            actionSupprPnj 	= new QAction(QIcon(":/resources/icons/supprimer PNJ.png"), tr("Supprimer un PNJ"), m_toolsGroup);
            actionDeplacePnj= new QAction(QIcon(":/resources/icons/deplacer PNJ.png"), tr("Déplacer/Orienter un personnage"), m_toolsGroup);
            actionEtatPnj	= new QAction(QIcon(":/resources/icons/etat.png"), tr("Changer l'état d'un personnage"), m_toolsGroup);

    // Action independante : remise a 0 des numeros de PNJ
            actionRazChrono	= new QAction(QIcon(":/resources/icons/chronometre.png"), tr("RAZ numéros de PNJ"), this);

           /* m_eraseAction= new QAction(QIcon(":/resources/icons/efface.png"), tr("Erase"), m_toolsGroup);
            m_hideAction= new QAction(QIcon(":/resources/icons/masque.png"), tr("Hide"), m_toolsGroup);
            m_unveilAction= new QAction(QIcon(":/resources/icons/demasque.png"), tr("Unveil"), m_toolsGroup);*/

    // Les actions sont checkable
    actionCrayon	->setCheckable(true);
    actionLigne		->setCheckable(true);
    actionRectVide	->setCheckable(true);
    actionRectPlein	->setCheckable(true);
    actionElliVide	->setCheckable(true);
    actionElliPlein	->setCheckable(true);
    actionTexte		->setCheckable(true);
    actionMain		->setCheckable(true);
    actionAjoutPnj	->setCheckable(true);
    actionSupprPnj	->setCheckable(true);
    actionDeplacePnj->setCheckable(true);
    actionEtatPnj	->setCheckable(true);

    /*m_eraseAction->setCheckable(true);
    m_hideAction->setCheckable(true);
    m_unveilAction->setCheckable(true);*/

    // Choix d'une action selectionnee au depart
    actionMain->setChecked(true);
}

	/********************************************************************/
	/* Creation des boutons et du widget qui les contient               */
	/********************************************************************/	
    void ToolsBar::creerOutils()
	{
		// Creation des boutons du toolBar
		QToolButton *boutonCrayon     = new QToolButton(outils);
		QToolButton *boutonLigne      = new QToolButton(outils);
		QToolButton *boutonRectVide   = new QToolButton(outils);
		QToolButton *boutonRectPlein  = new QToolButton(outils);
		QToolButton *boutonElliVide   = new QToolButton(outils);
		QToolButton *boutonElliPlein  = new QToolButton(outils);
		QToolButton *boutonTexte      = new QToolButton(outils);
		QToolButton *boutonMain       = new QToolButton(outils);
		QToolButton *boutonAjoutPnj   = new QToolButton(outils);
		QToolButton *boutonSupprPnj   = new QToolButton(outils);
		QToolButton *boutonDeplacePnj = new QToolButton(outils);
		QToolButton *boutonEtatPnj    = new QToolButton(outils);
		QToolButton *boutonRazChrono  = new QToolButton(outils);

       /*QToolButton* eraseButton  = new QToolButton(outils);
        QToolButton* hideButton  = new QToolButton(outils);
        QToolButton* unveilButton  = new QToolButton(outils);*/

		// Association des boutons avec les actions
        boutonCrayon->setDefaultAction(actionCrayon);
		boutonLigne      ->setDefaultAction(actionLigne);
		boutonRectVide   ->setDefaultAction(actionRectVide);
		boutonRectPlein  ->setDefaultAction(actionRectPlein);
		boutonElliVide   ->setDefaultAction(actionElliVide);
		boutonElliPlein  ->setDefaultAction(actionElliPlein);
		boutonTexte      ->setDefaultAction(actionTexte);
		boutonMain       ->setDefaultAction(actionMain);
		boutonAjoutPnj   ->setDefaultAction(actionAjoutPnj);
		boutonSupprPnj   ->setDefaultAction(actionSupprPnj);
		boutonDeplacePnj ->setDefaultAction(actionDeplacePnj);
		boutonEtatPnj    ->setDefaultAction(actionEtatPnj);
		boutonRazChrono  ->setDefaultAction(actionRazChrono);

        /*eraseButton->setDefaultAction(m_eraseAction);
        hideButton->setDefaultAction(m_hideAction);
        unveilButton->setDefaultAction(m_unveilAction);*/

		// Boutons en mode AutoRaise, plus lisible
		boutonCrayon     ->setAutoRaise(true);
		boutonLigne      ->setAutoRaise(true);
		boutonRectVide   ->setAutoRaise(true);
		boutonRectPlein  ->setAutoRaise(true);
		boutonElliVide   ->setAutoRaise(true);
		boutonElliPlein  ->setAutoRaise(true);
		boutonTexte      ->setAutoRaise(true);
		boutonMain       ->setAutoRaise(true);
		boutonAjoutPnj   ->setAutoRaise(true);
		boutonSupprPnj   ->setAutoRaise(true);
		boutonDeplacePnj ->setAutoRaise(true);
		boutonEtatPnj    ->setAutoRaise(true);
		boutonRazChrono  ->setAutoRaise(true);

       /* eraseButton->setAutoRaise(true);
        hideButton->setAutoRaise(true);
        unveilButton->setAutoRaise(true);*/

        /*#ifdef MACOS
			// Changement du style des boutons (plus lisible)
			QPlastiqueStyle *styleBouton = new QPlastiqueStyle();
			boutonCrayon     ->setStyle(styleBouton);
			boutonLigne      ->setStyle(styleBouton);
			boutonRectVide   ->setStyle(styleBouton);
			boutonRectPlein  ->setStyle(styleBouton);
			boutonElliVide   ->setStyle(styleBouton);
			boutonElliPlein  ->setStyle(styleBouton);
			boutonTexte      ->setStyle(styleBouton);
			boutonMain       ->setStyle(styleBouton);
			boutonAjoutPnj   ->setStyle(styleBouton);
			boutonSupprPnj   ->setStyle(styleBouton);
			boutonDeplacePnj ->setStyle(styleBouton);
			boutonEtatPnj    ->setStyle(styleBouton);
			boutonRazChrono  ->setStyle(styleBouton);
        #endif*/
		
        /**
	*
	* @todo used preferencemanager to get icon Size.
	*
	*/
        QSize tailleIcones(20,20);
		boutonCrayon     ->setIconSize(tailleIcones);
		boutonLigne      ->setIconSize(tailleIcones);
		boutonRectVide   ->setIconSize(tailleIcones);
		boutonRectPlein  ->setIconSize(tailleIcones);
		boutonElliVide   ->setIconSize(tailleIcones);
		boutonElliPlein  ->setIconSize(tailleIcones);
		boutonTexte      ->setIconSize(tailleIcones);
		boutonMain       ->setIconSize(tailleIcones);
		boutonAjoutPnj   ->setIconSize(tailleIcones);
		boutonSupprPnj   ->setIconSize(tailleIcones);
		boutonDeplacePnj ->setIconSize(tailleIcones);
		boutonEtatPnj    ->setIconSize(tailleIcones);
		boutonRazChrono  ->setIconSize(tailleIcones);

        /*eraseButton->setIconSize(tailleIcones);
        hideButton->setIconSize(tailleIcones);
        unveilButton->setIconSize(tailleIcones);*/
					
		// Creation du layout vertical qui constitue la barre d'outils
		QVBoxLayout *outilsLayout = new QVBoxLayout(outils);
		outilsLayout->setSpacing(0);
		outilsLayout->setMargin(2);

		// Creation du layout qui contient les outils de dessin
		QGridLayout *layoutDessin = new QGridLayout();
		layoutDessin->setSpacing(0);
		layoutDessin->setMargin(0);
		layoutDessin->addWidget(boutonCrayon, 0, 0);
		layoutDessin->addWidget(boutonLigne, 0, 1);
		layoutDessin->addWidget(boutonRectVide, 1, 0);
		layoutDessin->addWidget(boutonRectPlein, 1, 1);
		layoutDessin->addWidget(boutonElliVide, 2, 0);
		layoutDessin->addWidget(boutonElliPlein, 2, 1);
		layoutDessin->addWidget(boutonTexte, 3, 0);
		layoutDessin->addWidget(boutonMain, 3, 1);
        /*layoutDessin->addWidget(eraseButton, 4, 0);
        layoutDessin->addWidget(hideButton, 4, 1);
        layoutDessin->addWidget(unveilButton, 5, 0);*/
		// Creation des zones de texte et de nom de PNJ
		ligneDeTexte = new QLineEdit(outils);
        ligneDeTexte->setToolTip(tr("Text"));

		nomPnj = new QLineEdit(outils);
        nomPnj->setToolTip(tr("NPC Name"));

        /*#ifdef MACOS
			ligneDeTexte->setFixedHeight(22);
			nomPnj->setFixedHeight(22);
        #endif*/
		
		// Creation de l'afficheur du numero de PNJ
		afficheNumeroPnj = new QLCDNumber(2, outils);
		afficheNumeroPnj->setSegmentStyle(QLCDNumber::Flat);
        /// @todo used preferencemanager
        afficheNumeroPnj->setMaximumSize(20 + 7, 20);
		afficheNumeroPnj->display(1);
                afficheNumeroPnj->setToolTip(tr("NPC's number"));
		// Initialisation de la variable globale indiquant le numero de PNJ courant
		G_numeroPnjCourant = 1;
		
		// Creation du selecteur de couleur
        couleur = new ColorSelector(outils);

		// Creation du layout contient les outils de deplcement des PNJ
		QHBoxLayout *layoutMouvementPnj = new QHBoxLayout();
		layoutMouvementPnj->setSpacing(0);
		layoutMouvementPnj->setMargin(0);
		layoutMouvementPnj->addWidget(boutonDeplacePnj);
		layoutMouvementPnj->addWidget(boutonEtatPnj);

		// Creation du layout contient les outils d'ajout et de suppression des PNJ
		QGridLayout *layoutAjoutPnj = new QGridLayout();
		layoutAjoutPnj->setSpacing(0);
		layoutAjoutPnj->setMargin(0);
		layoutAjoutPnj->addWidget(boutonAjoutPnj, 0, 0);
		layoutAjoutPnj->addWidget(boutonSupprPnj, 0, 1);
		layoutAjoutPnj->addWidget(boutonRazChrono, 1, 0);
		layoutAjoutPnj->addWidget(afficheNumeroPnj, 1, 1, Qt::AlignHCenter);
		
		// Creation du selecteur de diametre du trait
        diametreTrait = new DiameterSelector(outils, true, 1, 45);
        diametreTrait->setToolTip(tr("Heigth of the pen"));
        connect(diametreTrait,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPenSizeChanged(int)));

		// Creation du selecteur de diametre des PNJ
        diametrePnj = new DiameterSelector(outils, false, 12, 41);
        connect(diametrePnj,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPNCSizeChanged(int)));
        diametrePnj->setToolTip(tr("Size of NPC"));

		//Creation du separateur se trouvant entre le selecteur de couleur et les outils de dessin
		QFrame *separateur1 = new QFrame(outils);
		separateur1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
		separateur1->setMinimumHeight(15);
		separateur1->setLineWidth(1);
		separateur1->setMidLineWidth(0);
		
		// Creation du separateur se trouvant entre les outils de dessin et le selecteur de diametre du trait
		QWidget *separateur2 = new QWidget(outils);
		separateur2->setFixedHeight(3);
		
		//Creation du separateur se trouvant entre les outils de dessin et ceux de deplacement des PNJ
		QFrame *separateur3 = new QFrame(outils);
		separateur3->setFrameStyle(QFrame::HLine | QFrame::Sunken);
		separateur3->setMinimumHeight(10);
		separateur3->setLineWidth(1);
		separateur3->setMidLineWidth(0);

		//Creation du separateur se trouvant entre les outils de deplacement et ceux d'ajout des PNJ
		QFrame *separateur4 = new QFrame(outils);
		separateur4->setFrameStyle(QFrame::HLine | QFrame::Sunken);
		separateur4->setMinimumHeight(10);
		separateur4->setLineWidth(1);
		separateur4->setMidLineWidth(0);

		// Creation du separateur se trouvant au dessus du selecteur de diametre des PNJ
		QWidget *separateur5 = new QWidget(outils);
		separateur5->setFixedHeight(3);

        /*#ifdef MACOS
			// Creation des separateurs se trouvant au dessus des zones de saisie de texte
			QWidget *separateur10 = new QWidget(outils);
			QWidget *separateur11 = new QWidget(outils);
			separateur10->setFixedHeight(3);
			separateur11->setFixedHeight(3);
        #endif*/
		
		// Ajout des differents layouts et widgets dans outilsLayout
		outilsLayout->addWidget(couleur);
		outilsLayout->addWidget(separateur1);
		outilsLayout->addLayout(layoutDessin);
        /*#ifdef MACOS
			outilsLayout->addWidget(separateur10);
        #endif*/
		outilsLayout->addWidget(ligneDeTexte);
		outilsLayout->addWidget(separateur2);
		outilsLayout->addWidget(diametreTrait);
		outilsLayout->addWidget(separateur3);
		outilsLayout->addLayout(layoutMouvementPnj);
		outilsLayout->addWidget(separateur4);
		outilsLayout->addLayout(layoutAjoutPnj);
        /*#ifdef MACOS
			outilsLayout->addWidget(separateur11);
        #endif*/
		outilsLayout->addWidget(nomPnj);
		outilsLayout->addWidget(separateur5);
		outilsLayout->addWidget(diametrePnj);

		// Alignement du widget outils sur le haut du dockWidget
		layout()->setAlignment(outils, Qt::AlignTop | Qt::AlignHCenter);
		// Contraintes de taille sur la barre d'outils
        /// @todo used preferencemanager
        outils->setFixedWidth((20+8)*layoutDessin->columnCount());
        setMaximumWidth((20+8)*layoutDessin->columnCount()+10);
	}


void ToolsBar::incrementeNumeroPnj()
{

    int numeroActuel = (int) afficheNumeroPnj->value();


    numeroActuel++;

    if (afficheNumeroPnj->checkOverflow(numeroActuel))
        afficheNumeroPnj->display(1);
    else
        afficheNumeroPnj->display(numeroActuel);


    G_numeroPnjCourant = (int) afficheNumeroPnj->value();
}


void ToolsBar::razNumeroPnj()
{
    afficheNumeroPnj->display(1);

    G_numeroPnjCourant = 1;
}


void ToolsBar::changementTaille(bool floating)
{
    if (floating)
    {
        setFixedHeight(578);
    }
    else
        setMaximumHeight(0xFFFFFF);
}


void ToolsBar::texteChange(const QString &texte)
{
    // M.a.j automatique de la variable globale contenant le texte
    G_texteCourant = texte;
    // Selection automatique de l'action Texte
    actionTexte->trigger();
}


void ToolsBar::nomPnjChange(const QString &texte)
{
    // M.a.j de la variable globale contenant le nom du PNJ
    G_nomPnjCourant = texte;
    // Selection automatique de l'action ajout PNJ
    actionAjoutPnj->trigger();
}


void ToolsBar::changeCurrentColor(QColor color)
{
    couleur->setCurrentColor(color);
    qDebug() << "Toolbar changed color";
    emit currentColorChanged(color);
}
QColor& ToolsBar::currentColor()
{
    return couleur->currentColor();
}


void ToolsBar::mettreAJourPnj(int diametre, QString nom)
{

    diametrePnj->changerDiametre(diametre);

    nomPnj->setText(nom);

    G_nomPnjCourant = nom;
}


void ToolsBar::majCouleursPersonnelles()
{
    couleur->customColorUpdate();
}

QColor ToolsBar::donnerCouleurPersonnelle(int numero)
{
    return couleur->getPersonalColor(numero);
}




void ToolsBar::currentActionChanged(QAction* p)
{
    //  enum SelectableTool {PEN, LINE, EMPTYRECT, FILLRECT, EMPTYELLIPSE, FILLEDELLIPSE, TEXT, HANDLER, ADDNPC, DELNPC, MOVECHARACTER, STATECHARACTER};


        if(p == actionCrayon)
            m_currentTool = PEN;

        if(p ==  actionLigne)
            m_currentTool = LINE;

        if(p == actionRectVide)
                m_currentTool = EMPTYRECT;

       if(p == actionRectPlein)
            m_currentTool = FILLRECT;

        if(p ==  actionElliVide)
            m_currentTool = EMPTYELLIPSE;

        if(p ==  actionElliPlein)
            m_currentTool = FILLEDELLIPSE;

        if(p ==  actionMain)
            m_currentTool = HANDLER;

        if(p ==  actionTexte)
            m_currentTool = TEXT;

        if(p ==  actionAjoutPnj)
            m_currentTool = ADDNPC;

        if(p ==  actionSupprPnj)
            m_currentTool = DELNPC;

        if(p ==  actionDeplacePnj)
            m_currentTool = MOVECHARACTER;

        if(p ==  actionEtatPnj)
            m_currentTool = STATECHARACTER;

        emit currentToolChanged(m_currentTool);


}
