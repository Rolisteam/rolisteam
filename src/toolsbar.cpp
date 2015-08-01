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
#include "data/person.h"
#include "data/player.h"
#include "variablesGlobales.h"

#define DEFAULT_ICON_SIZE 20

ToolsBar::ToolsBar(QWidget *parent)
    : QWidget(parent)
{
	m_currentTool = Handler;
	setWindowTitle(tr("Tools"));
	setObjectName("ToolsBar");




    m_mainWidget = new QWidget(this);

    createActions();
    createTools();

    connect(m_resetCountAct, SIGNAL(triggered(bool)), this, SLOT(resetNpcNumber()));
	connect(m_textEdit, SIGNAL(textEdited(const QString &)), this, SIGNAL(currentTextChanged(QString)));
	connect(m_npcNameEdit, SIGNAL(textEdited(const QString &)), this, SIGNAL(currentNpcNameChanged(QString)));
}
ToolsBar::~ToolsBar()
{
	delete m_textEdit;
	delete m_npcNameEdit;
}

void ToolsBar::updateUi()
{
    m_color->checkPermissionColor();
        if(!PlayersList::instance()->localPlayer()->isGM())
        {
            m_npcDiameter->setVisible(false);
        }
}

void ToolsBar::createActions()
{
	// Creation du groupe d'action
	QActionGroup *groupOutils = new QActionGroup(this);

	// Creation des actions
    m_pencilAct				= new QAction(QIcon(":/resources/icons/pen.png"), tr("Pen"), groupOutils);
    m_lineAct				= new QAction(QIcon(":/resources/icons/line.png"), tr("Line"), groupOutils);
    m_rectAct				= new QAction(QIcon(":/resources/icons/emptyrectangle.png"), tr("Empty Rectangle"), groupOutils);
    m_filledRectAct			= new QAction(QIcon(":/resources/icons/filledrectangle.png"), tr("filled Rectangle"), groupOutils);
    m_ellipseAct			= new QAction(QIcon(":/resources/icons/emptyellipse.png"), tr("Empty Ellipse"), groupOutils);
    m_filledEllipseAct		= new QAction(QIcon(":/resources/icons/filledellipse.png"), tr("Filled Ellipse"), groupOutils);
    m_textAct				= new QAction(QIcon(":/resources/icons/text.png"), tr("Text"), groupOutils);
    m_handAct				= new QAction(QIcon(":/resources/icons/hand.png"), tr("Move"), groupOutils);
    m_addNpcAct				= new QAction(QIcon(":/resources/icons/add.png"), tr("Add NPC"), groupOutils);
    m_delNpcAct				= new QAction(QIcon(":/resources/icons/remove.png"), tr("Remove NPC"), groupOutils);
    m_moveCharacterAct		= new QAction(QIcon(":/resources/icons/moveNpc.png"), tr("Move/Turn Character"), groupOutils);
    m_changeCharacterState	= new QAction(QIcon(":/resources/icons/state.png"), tr("Change Character's State"), groupOutils);

	// Action independante : remise a 0 des numeros de PNJ
	m_resetCountAct	= new QAction(QIcon(":/resources/icons/chronometre.png"), tr("Reset NPC counter"), this);

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

void ToolsBar::createTools()
{
    // Creationm_actionGroup des boutons du toolBar
    m_actionGroup = new QActionGroup(this);
    QToolButton *penButton     = new QToolButton(this);
    QToolButton *LineButton      = new QToolButton(this);
    QToolButton *emptyRectButton   = new QToolButton(this);
    QToolButton *filledRectButton  = new QToolButton(this);
    QToolButton *emptyEllipseButton   = new QToolButton(this);
    QToolButton *FilledEllipseButton  = new QToolButton(this);
    QToolButton *textButton      = new QToolButton(this);
    QToolButton *handButton       = new QToolButton(this);
    QToolButton *addNpcButton   = new QToolButton(this);
    QToolButton *delNpcButton   = new QToolButton(this);
    QToolButton *moveNpcButton = new QToolButton(this);
    QToolButton *stateNpcButton    = new QToolButton(this);
    QToolButton *resetNumberButton  = new QToolButton(this);

	// Association des boutons avec les actions
    penButton     ->setDefaultAction(m_pencilAct);
    LineButton      ->setDefaultAction(m_lineAct);
    emptyRectButton   ->setDefaultAction(m_rectAct);
    filledRectButton  ->setDefaultAction(m_filledRectAct);
    emptyEllipseButton   ->setDefaultAction(m_ellipseAct);
    FilledEllipseButton  ->setDefaultAction(m_filledEllipseAct);
    textButton      ->setDefaultAction(m_textAct);
    handButton       ->setDefaultAction(m_handAct);
    addNpcButton   ->setDefaultAction(m_addNpcAct);
    delNpcButton   ->setDefaultAction(m_delNpcAct);
    moveNpcButton ->setDefaultAction(m_moveCharacterAct);
    stateNpcButton    ->setDefaultAction(m_changeCharacterState);
    resetNumberButton  ->setDefaultAction(m_resetCountAct);


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
    penButton->setAutoRaise(true);
    LineButton->setAutoRaise(true);
    emptyRectButton->setAutoRaise(true);
    filledRectButton->setAutoRaise(true);
    emptyEllipseButton->setAutoRaise(true);
    FilledEllipseButton->setAutoRaise(true);
    textButton->setAutoRaise(true);
    handButton->setAutoRaise(true);
    addNpcButton->setAutoRaise(true);
    delNpcButton->setAutoRaise(true);
    moveNpcButton->setAutoRaise(true);
    stateNpcButton->setAutoRaise(true);
    resetNumberButton->setAutoRaise(true);
	
	// Changement de la taille des icones
	QSize tailleIcones(DEFAULT_ICON_SIZE,DEFAULT_ICON_SIZE);
    penButton     ->setIconSize(tailleIcones);
    LineButton      ->setIconSize(tailleIcones);
    emptyRectButton   ->setIconSize(tailleIcones);
    filledRectButton  ->setIconSize(tailleIcones);
    emptyEllipseButton   ->setIconSize(tailleIcones);
    FilledEllipseButton  ->setIconSize(tailleIcones);
    textButton      ->setIconSize(tailleIcones);
    handButton       ->setIconSize(tailleIcones);
    addNpcButton   ->setIconSize(tailleIcones);
    delNpcButton   ->setIconSize(tailleIcones);
    moveNpcButton ->setIconSize(tailleIcones);
    stateNpcButton    ->setIconSize(tailleIcones);
    resetNumberButton  ->setIconSize(tailleIcones);
				
	// Creation du layout vertical qui constitue la barre d'outils
    QVBoxLayout *outilsLayout = new QVBoxLayout(this);
	outilsLayout->setSpacing(0);
	outilsLayout->setMargin(2);

	// Creation du layout qui contient les outils de dessin
    /*QGridLayout *layoutDessin = new QGridLayout();
	layoutDessin->setSpacing(0);
	layoutDessin->setMargin(0);
    layoutDessin->addWidget(penButton, 0, 0);
    layoutDessin->addWidget(LineButton, 0, 1);
    layoutDessin->addWidget(emptyRectButton, 1, 0);
    layoutDessin->addWidget(filledRectButton, 1, 1);
    layoutDessin->addWidget(emptyEllipseButton, 2, 0);
    layoutDessin->addWidget(FilledEllipseButton, 2, 1);
    layoutDessin->addWidget(textButton, 3, 0);
    layoutDessin->addWidget(handButton, 3, 1);*/

    FlowLayout *toolsLayout = new FlowLayout();
    toolsLayout->setSpacing(0);
    toolsLayout->setMargin(0);
    toolsLayout->addWidget(penButton);
    toolsLayout->addWidget(LineButton);
    toolsLayout->addWidget(emptyRectButton);
    toolsLayout->addWidget(filledRectButton);
    toolsLayout->addWidget(emptyEllipseButton);
    toolsLayout->addWidget(FilledEllipseButton);
    toolsLayout->addWidget(textButton);
    toolsLayout->addWidget(handButton);

	// Creation des zones de texte et de nom de PNJ
    m_textEdit = new QLineEdit(this);
    m_textEdit->setToolTip(tr("Text"));

    m_npcNameEdit = new QLineEdit(this);
    m_npcNameEdit->setToolTip(tr("NPC name"));
	
	// Creation de l'afficheur du numero de PNJ
    m_showPnjNumber = new QLCDNumber(2, this);
    m_showPnjNumber->setSegmentStyle(QLCDNumber::Flat);
    m_showPnjNumber->setMaximumSize(DEFAULT_ICON_SIZE + 7, DEFAULT_ICON_SIZE);
    m_showPnjNumber->display(1);
    m_showPnjNumber->setToolTip(tr("NPC Number"));

	m_currentNpcNumber = 1;
	
    // Creation du selecteur de m_color
    m_color = new ColorSelector(this);

	// Creation du layout contient les outils de deplcement des PNJ
	QHBoxLayout *layoutMouvementPnj = new QHBoxLayout();
	layoutMouvementPnj->setSpacing(0);
	layoutMouvementPnj->setMargin(0);
    layoutMouvementPnj->addWidget(moveNpcButton);
	layoutMouvementPnj->addWidget(stateNpcButton);

	// Creation du layout contient les outils d'ajout et de suppression des PNJ
	//QGridLayout *layoutAjoutPnj = new QGridLayout();
	QHBoxLayout *layoutAjoutPnj = new QHBoxLayout();
	layoutAjoutPnj->setSpacing(0);
	layoutAjoutPnj->setMargin(0);
	layoutAjoutPnj->addWidget(addNpcButton/*, 0, 0*/);
	layoutAjoutPnj->addWidget(delNpcButton/*, 0, 1*/);

	QHBoxLayout *layoutAjoutPnj2 = new QHBoxLayout();
	layoutAjoutPnj2->setSpacing(0);
	layoutAjoutPnj2->setMargin(0);
	layoutAjoutPnj2->addWidget(resetNumberButton/*, 1, 0*/);
	layoutAjoutPnj2->addWidget(m_showPnjNumber/*, 1, 1, Qt::AlignHCenter*/);

	m_showPnjNumber->setVisible(false);


	
	// Creation du selecteur de diametre du trait
    m_lineDiameter = new DiameterSelector(this, true, 1, 45);
	m_lineDiameter->setToolTip(tr("Line's Width"));
	connect(m_lineDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPenSizeChanged(int)));


    // Creation du selecteur de diametre des PNJ
    m_npcDiameter = new DiameterSelector(this, false, 12, 200);
    m_npcDiameter->setToolTip(tr("NPC Size"));
	connect(m_npcDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentNpcSizeChanged(int)));

	//Creation du separateur se trouvant entre le selecteur de couleur et les outils de dessin
    QFrame *separateur1 = new QFrame(this);
	separateur1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	separateur1->setMinimumHeight(15);
	separateur1->setLineWidth(1);
	separateur1->setMidLineWidth(0);
	
	// Creation du separateur se trouvant entre les outils de dessin et le selecteur de diametre du trait
    QWidget *separateur2 = new QWidget(this);
	separateur2->setFixedHeight(3);
	
	//Creation du separateur se trouvant entre les outils de dessin et ceux de deplacement des PNJ
    QFrame *separateur3 = new QFrame(this);
	separateur3->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	separateur3->setMinimumHeight(10);
	separateur3->setLineWidth(1);
	separateur3->setMidLineWidth(0);

	//Creation du separateur se trouvant entre les outils de deplacement et ceux d'ajout des PNJ
    QFrame *separateur4 = new QFrame(this);
	separateur4->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	separateur4->setMinimumHeight(10);
	separateur4->setLineWidth(1);
	separateur4->setMidLineWidth(0);

	// Creation du separateur se trouvant au dessus du selecteur de diametre des PNJ
	QWidget *separateur5 = new QWidget(this);
	separateur5->setFixedHeight(3);
	
	// Ajout des differents layouts et widgets dans outilsLayout
    outilsLayout->addWidget(m_color);
	outilsLayout->addWidget(separateur1);
    outilsLayout->addLayout(toolsLayout);

    outilsLayout->addWidget(m_textEdit);

	outilsLayout->addWidget(separateur2);
	outilsLayout->addWidget(m_lineDiameter);
	m_lineDiameter->setVisible(false);
	outilsLayout->addWidget(separateur3);
	outilsLayout->addLayout(layoutMouvementPnj);
	outilsLayout->addWidget(separateur4);
	outilsLayout->addLayout(layoutAjoutPnj);
	outilsLayout->addLayout(layoutAjoutPnj2);

    outilsLayout->addWidget(m_npcNameEdit);

	outilsLayout->addWidget(separateur5);

        //if(PlayersList::instance().localPlayer()->isGM())
    outilsLayout->addWidget(m_npcDiameter);

    outilsLayout->addStretch();

    setLayout(outilsLayout);
        //outilsLayout->addWidget(m_pcDiameter);
	// Alignement du widget outils sur le haut du dockWidget
    //layout()->setAlignment(m_mainWidget, Qt::AlignTop | Qt::AlignHCenter);
	// Contraintes de taille sur la barre d'outils

    //m_mainWidget->setFixedWidth((DEFAULT_ICON_SIZE+8)*layoutDessin->columnCount());
    //setMaximumWidth((DEFAULT_ICON_SIZE+8)*layoutDessin->columnCount()+10);
}

void ToolsBar::incrementNpcNumber()
{
	// Recuperation de la valeur actuelle
    int numeroActuel = (int) m_showPnjNumber->value();
	
	// Incrementation
	numeroActuel++;
	// MAJ de la valeur; si la nouvelle valeur ne rentre pas, on la met a 1
    if (m_showPnjNumber->checkOverflow(numeroActuel))
        m_showPnjNumber->display(1);
	else
        m_showPnjNumber->display(numeroActuel);
		

    m_currentNpcNumber = (int) m_showPnjNumber->value();
    emit currentNpcNumberChanged(m_currentNpcNumber);
}

void ToolsBar::resetNpcNumber()
{
    m_showPnjNumber->display(1);
	m_currentNpcNumber = 1;
    emit currentNpcNumberChanged(m_currentNpcNumber);
}

void ToolsBar::changeSize(bool floating)
{
    /*if (floating)
	{
		setFixedHeight(578);
	}
	else
        setMaximumHeight(0xFFFFFF);*/
}
void ToolsBar::changeCurrentColor(QColor col)
{
    m_color->changeCurrentColor(col);
	emit currentColorChanged(col);
}

void ToolsBar::updateNpc(int diametre, QString nom)
{
    m_npcDiameter->changerDiametre(diametre);
	m_npcNameEdit->setText(nom);
	m_currentNPCName = nom;
}

void ToolsBar::changeMap(Map* map)
{
    if (map != NULL)
    {
        m_npcDiameter->changerDiametre(map->tailleDesPj());
    }
}

void ToolsBar::updatePersonalColor()
{
    m_color->updatePersonalColor();
}

QColor ToolsBar::getPersonalColor(int numero)
{
    return m_color->getPersonColor(numero);
}

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
int ToolsBar::getCurrentNpcNumber() const
{
    return m_currentNpcNumber;
}
int ToolsBar::getCurrentPenSize()const
{
    return m_lineDiameter->getCurrentValue();
}
