/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *      http://www.rolisteam.org/                                        *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include <QVBoxLayout>
#include <QToolButton>
#include <QDebug>


#include "toolsbar.h"

#include "map/map.h"
#include "network/networkmessagewriter.h"
#include "widgets/colorselector.h"
#include "widgets/diameterselector.h"
#include "userlist/playersList.h"
#include "data/persons.h"

#include "variablesGlobales.h"

#define DEFAULT_ICON_SIZE 20




//QString G_texteCourant;
// Contient le texte de la zone "nom du PNJ"
//QString G_nomPnjCourant;
// Numero de PNJ courant
//int G_numeroPnjCourant;


ToolsBar::ToolsBar(QWidget *parent)
	: QDockWidget(parent), m_map(NULL)
{
	m_currentTool = Handler;
	setWindowTitle(tr("Tools"));
	setObjectName("ToolsBar");
	// Parametrage du dockWidget
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	// Creation du widget contenant les boutons
	outils = new QWidget(this);
	// Insertion du widget dans le dockWidget
	setWidget(outils);

	// Creation des actions
	creerActions();
	// Creation des outils
	creerOutils();

	// Initialisation de la variable globale indiquant l'outil courant
    //G_outilCourant = main;
	
	// Connexion de l'action RazChrono avec le slot razNumeroPnj
	connect(m_resetCountAct, SIGNAL(triggered(bool)), this, SLOT(razNumeroPnj()));
	connect(m_textEdit, SIGNAL(textEdited(const QString &)), this, SIGNAL(currentTextChanged(QString)));
	connect(m_npcNameEdit, SIGNAL(textEdited(const QString &)), this, SIGNAL(currentNpcNameChanged(QString)));

	// Connection du changement d'etat (floating / no floating) avec le changement de taille
	connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(changeSize(bool)));

	setFloating(false);
}
ToolsBar::~ToolsBar()
{
	delete m_textEdit;
	delete m_npcNameEdit;
}

void ToolsBar::updateUi()
{
    m_color->autoriserOuInterdireCouleurs();
        if(!PlayersList::instance()->localPlayer()->isGM())
        {
            m_npcDiameter->setVisible(false);
        }
}

void ToolsBar::creerActions()
{
	// Creation du groupe d'action
	QActionGroup *groupOutils = new QActionGroup(this);

	// Creation des actions
	m_pencilAct				= new QAction(QIcon(":/resources/icones/crayon.png"), tr("Pen"), groupOutils);
	m_lineAct				= new QAction(QIcon(":/resources/icones/ligne.png"), tr("Line"), groupOutils);
	m_rectAct				= new QAction(QIcon(":/resources/icones/rectangle vide.png"), tr("Empty Rectangle"), groupOutils);
	m_filledRectAct			= new QAction(QIcon(":/resources/icones/rectangle plein.png"), tr("filled Rectangle"), groupOutils);
	m_ellipseAct			= new QAction(QIcon(":/resources/icones/ellipse vide.png"), tr("Empty Ellipse"), groupOutils);
	m_filledEllipseAct		= new QAction(QIcon(":/resources/icones/ellipse pleine.png"), tr("Filled Ellipse"), groupOutils);
	m_textAct				= new QAction(QIcon(":/textevignette"), tr("Text"), groupOutils);
	m_handAct				= new QAction(QIcon(":/resources/icones/main.png"), tr("Move"), groupOutils);
	m_addNpcAct				= new QAction(QIcon("://resources/icones/add.png"), tr("Add NPC"), groupOutils);
	m_delNpcAct				= new QAction(QIcon("://resources/icones/remove.png"), tr("Remove NPC"), groupOutils);
	m_moveCharacterAct		= new QAction(QIcon(":/resources/icones/deplacer PNJ.png"), tr("Move/Turn Character"), groupOutils);
	m_changeCharacterState	= new QAction(QIcon(":/resources/icones/etat.png"), tr("Change Character's State"), groupOutils);

	// Action independante : remise a 0 des numeros de PNJ
	m_resetCountAct	= new QAction(QIcon(":/resources/icones/chronometre.png"), tr("Reset NPC counter"), this);

	// Les actions sont checkable
	m_pencilAct	->setCheckable(true);
	m_lineAct->setCheckable(true);
	m_rectAct->setCheckable(true);
	m_filledRectAct->setCheckable(true);
	m_ellipseAct->setCheckable(true);
	m_filledEllipseAct->setCheckable(true);
	m_textAct->setCheckable(true);
	m_handAct->setCheckable(true);
	m_addNpcAct->setCheckable(true);
	m_delNpcAct->setCheckable(true);
	m_moveCharacterAct->setCheckable(true);
	m_changeCharacterState->setCheckable(true);

	// Choix d'une action selectionnee au depart
	m_handAct->setChecked(true);
}	

/********************************************************************/
/* Creation des boutons et du widget qui les contient               */
/********************************************************************/	
void ToolsBar::creerOutils()
{
    // Creationm_actionGroup des boutons du toolBar
    m_actionGroup = new QActionGroup(this);
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












	// Association des boutons avec les actions
	boutonCrayon     ->setDefaultAction(m_pencilAct);
	boutonLigne      ->setDefaultAction(m_lineAct);
	boutonRectVide   ->setDefaultAction(m_rectAct);
	boutonRectPlein  ->setDefaultAction(m_filledRectAct);
	boutonElliVide   ->setDefaultAction(m_ellipseAct);
	boutonElliPlein  ->setDefaultAction(m_filledEllipseAct);
	boutonTexte      ->setDefaultAction(m_textAct);
	boutonMain       ->setDefaultAction(m_handAct);
	boutonAjoutPnj   ->setDefaultAction(m_addNpcAct);
	boutonSupprPnj   ->setDefaultAction(m_delNpcAct);
	boutonDeplacePnj ->setDefaultAction(m_moveCharacterAct);
	boutonEtatPnj    ->setDefaultAction(m_changeCharacterState);
	boutonRazChrono  ->setDefaultAction(m_resetCountAct);


	m_actionGroup->addAction(m_pencilAct);
	m_actionGroup->addAction(m_lineAct);
	m_actionGroup->addAction(m_rectAct);
	m_actionGroup->addAction(m_filledRectAct);
	m_actionGroup->addAction(m_ellipseAct);
	m_actionGroup->addAction(m_filledEllipseAct);
	m_actionGroup->addAction(m_textAct);

	m_actionGroup->addAction(m_handAct);
	m_actionGroup->addAction(m_addNpcAct);
	m_actionGroup->addAction(m_delNpcAct);
	m_actionGroup->addAction(m_moveCharacterAct);
	m_actionGroup->addAction(m_changeCharacterState);
    //m_actionGroup->addAction(actionRazChrono);

    connect(m_actionGroup,SIGNAL(triggered(QAction*)),this,SLOT(currentToolHasChanged(QAction*)));

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
	
	// Changement de la taille des icones
	QSize tailleIcones(DEFAULT_ICON_SIZE,DEFAULT_ICON_SIZE);
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

	// Creation des zones de texte et de nom de PNJ
	m_textEdit = new QLineEdit(outils);
		m_textEdit->setToolTip(tr("Text"));

	m_npcNameEdit = new QLineEdit(outils);
		m_npcNameEdit->setToolTip(tr("NPC name"));
	
	// Creation de l'afficheur du numero de PNJ
	afficheNumeroPnj = new QLCDNumber(2, outils);
	afficheNumeroPnj->setSegmentStyle(QLCDNumber::Flat);
	afficheNumeroPnj->setMaximumSize(DEFAULT_ICON_SIZE + 7, DEFAULT_ICON_SIZE);
	afficheNumeroPnj->display(1);
                afficheNumeroPnj->setToolTip(tr("NPC Number"));
	// Initialisation de la variable globale indiquant le numero de PNJ courant
	m_currentNpcNumber = 1;
	
    // Creation du selecteur de m_color
    m_color = new ColorSelector(outils);

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
	m_lineDiameter = new DiameterSelector(outils, true, 1, 45);
	m_lineDiameter->setToolTip(tr("Line's Width"));
	connect(m_lineDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPenSizeChanged(int)));


    // Creation du selecteur de diametre des PNJ
    m_npcDiameter = new DiameterSelector(outils, false, 12, 200);
    m_npcDiameter->setToolTip(tr("NPC Size"));
	connect(m_npcDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentNpcSizeChanged(int)));

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
	
	// Ajout des differents layouts et widgets dans outilsLayout
    outilsLayout->addWidget(m_color);
	outilsLayout->addWidget(separateur1);
	outilsLayout->addLayout(layoutDessin);

	outilsLayout->addWidget(m_textEdit);
	outilsLayout->addWidget(separateur2);
	outilsLayout->addWidget(m_lineDiameter);
	outilsLayout->addWidget(separateur3);
	outilsLayout->addLayout(layoutMouvementPnj);
	outilsLayout->addWidget(separateur4);
	outilsLayout->addLayout(layoutAjoutPnj);

	outilsLayout->addWidget(m_npcNameEdit);
	outilsLayout->addWidget(separateur5);
        //if(PlayersList::instance().localPlayer()->isGM())
        outilsLayout->addWidget(m_npcDiameter);
        //outilsLayout->addWidget(m_pcDiameter);
	// Alignement du widget outils sur le haut du dockWidget
	layout()->setAlignment(outils, Qt::AlignTop | Qt::AlignHCenter);
	// Contraintes de taille sur la barre d'outils
	outils->setFixedWidth((DEFAULT_ICON_SIZE+8)*layoutDessin->columnCount());
	setMaximumWidth((DEFAULT_ICON_SIZE+8)*layoutDessin->columnCount()+10);
}

void ToolsBar::incrementNpcNumber()
{
	// Recuperation de la valeur actuelle
	int numeroActuel = (int) afficheNumeroPnj->value();
	
	// Incrementation
	numeroActuel++;
	// MAJ de la valeur; si la nouvelle valeur ne rentre pas, on la met a 1
	if (afficheNumeroPnj->checkOverflow(numeroActuel))
		afficheNumeroPnj->display(1);
	else
		afficheNumeroPnj->display(numeroActuel);
		

	m_currentNpcNumber = (int) afficheNumeroPnj->value();
}

void ToolsBar::razNumeroPnj()
{
	afficheNumeroPnj->display(1);
	// Mise a jour de la variable globale indiquant le numero de PNJ
	m_currentNpcNumber = 1;
}

void ToolsBar::changeSize(bool floating)
{
	if (floating)
	{

		setFixedHeight(578);

	}
	else
		setMaximumHeight(0xFFFFFF);
}
void ToolsBar::changeCurrentColor(QColor col)
{
	m_color->changeCouleurActuelle(col);
	emit currentColorChanged(col);
}

void ToolsBar::mettreAJourPnj(int diametre, QString nom)
{

    m_npcDiameter->changerDiametre(diametre);

	m_npcNameEdit->setText(nom);

	m_currentNPCName = nom;
}

void ToolsBar::changeMap(Map * map)
{
    m_map = map;
    if (m_map != NULL)
    {
        m_npcDiameter->changerDiametre(map->tailleDesPj());
    }
}

void ToolsBar::majCouleursPersonnelles()
{
    m_color->majCouleursPersonnelles();
}

QColor ToolsBar::donnerCouleurPersonnelle(int numero)
{
    return m_color->donnerCouleurPersonnelle(numero);
}

//void ToolsBar::changeCharacterSize(int size)
//{
//    if (m_map != NULL)
//		m_map->changerTaillePjCarte(size, m_currentTool != AddNpc);
//}

//void ToolsBar::sendNewCharacterSize(int size)
//{
//    if (m_map == NULL)
//        return;

//    changeCharacterSize(size);
//	if(m_currentTool != AddNpc)
//    {
//        NetworkMessageWriter message (NetMsg::CharacterPlayerCategory, NetMsg::ChangePlayerCharacterSizeAction);
//        message.string8(m_map->identifiantCarte());
//        message.string8(m_map->getLastSelectedCharacterId());
//        message.uint8(size - 11);
//        message.sendAll();
//    }
//}


void ToolsBar::currentToolHasChanged(QAction* bt)
{
	SelectableTool previous = m_currentTool;
	if(bt==m_pencilAct)
    {
		m_currentTool = Pen;
    }
	if(bt==m_lineAct)
    {
		m_currentTool = Line;
    }
	if(bt==m_rectAct)
    {
		m_currentTool = EmptyRect;
    }
	if(bt==m_filledRectAct)
    {
		m_currentTool = FilledRect;
    }
	if(bt==m_ellipseAct)
    {
		m_currentTool = EmptyEllipse;
    }
	if(bt==m_filledEllipseAct)
    {
		m_currentTool = FilledEllipse;
    }
	if(bt==m_textAct)
    {
		if (!(m_textEdit->hasFocus()))
        {
			m_textEdit->setFocus(Qt::OtherFocusReason);
			m_textEdit->setSelection(0, m_textEdit->text().length());
        }
		m_currentTool = Text;
    }
	if(bt==m_handAct)
    {
		m_currentTool = Handler;
    }
	if(bt==m_addNpcAct)
    {
		if (!(m_npcNameEdit->hasFocus()))
        {
			m_npcNameEdit->setFocus(Qt::OtherFocusReason);
			m_npcNameEdit->setSelection(0, m_npcNameEdit->text().length());
        }
		m_currentTool = AddNpc;
    }
	if(bt==m_delNpcAct)
    {
		m_currentTool = DelNpc;
    }
	if(bt==m_moveCharacterAct)
    {
		m_currentTool = MoveCharacterToken;
    }
	if(bt==m_changeCharacterState)
    {
		m_currentTool = ChangeCharacterState;
    }
    if(previous!=m_currentTool)
    {
        emit currentToolChanged(m_currentTool);
    }

}
ToolsBar::SelectableTool ToolsBar::getCurrentTool() const
{
    return m_currentTool;
}
