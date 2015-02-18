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

#include "toolbar.h"
#include "colorselector.h"
#include "diameterselector.h"
#include "flowlayout.h"
	

/**/
ToolsBar* ToolsBar::m_sigleton=NULL;

ToolsBar* ToolsBar::getInstance(QWidget *parent)
{
    if(m_sigleton==NULL)
        m_sigleton=new ToolsBar(parent);

    return m_sigleton;
}

ToolsBar::ToolsBar(QWidget *parent)
		: QDockWidget(parent)
{

    setWindowTitle(tr("Tools"));
    setObjectName("Toolbar");
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_centralWidget = new QWidget(this);
    creerActions();
    creerOutils();
    setWidget(m_centralWidget);

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


    actionCrayon 	= new QAction(QIcon(":/resources/icons/crayon.png"), tr("Crayon"), m_toolsGroup);
    actionLigne 	= new QAction(QIcon(":/resources/icons/line.png"), tr("Ligne"), m_toolsGroup);
    actionRectVide 	= new QAction(QIcon(":/resources/icons/rectangle vide.png"), tr("Rectangle vide"), m_toolsGroup);
    actionRectPlein	= new QAction(QIcon(":/resources/icons/rectangle plein.png"), tr("Rectangle plein"), m_toolsGroup);
    actionElliVide 	= new QAction(QIcon(":/resources/icons/ellipse vide.png"), tr("Ellipse vide"), m_toolsGroup);
    actionElliPlein	= new QAction(QIcon(":/resources/icons/ellipse pleine.png"), tr("Ellipse pleine"), m_toolsGroup);
    actionTexte 	= new QAction(QIcon(":/resources/icons/text.png"), tr("Texte"), m_toolsGroup);
    actionMain		= new QAction(QIcon(":/resources/icons/main.png"), tr("Déplacer"), m_toolsGroup);
    actionAjoutPnj 	= new QAction(QIcon(":/resources/icons/ajouter PNJ.png"), tr("Ajouter un PNJ"), m_toolsGroup);
    actionSupprPnj 	= new QAction(QIcon(":/resources/icons/supprimer PNJ.png"), tr("Supprimer un PNJ"), m_toolsGroup);
    actionDeplacePnj= new QAction(QIcon(":/resources/icons/move-npc.png"), tr("Déplacer/Orienter un personnage"), m_toolsGroup);
    actionEtatPnj	= new QAction(QIcon(":/resources/icons/etat.png"), tr("Changer l'état d'un personnage"), m_toolsGroup);


    actionRazChrono	= new QAction(QIcon(":/resources/icons/chronometre.png"), tr("RAZ numéros de PNJ"), this);

    actionCrayon->setCheckable(true);
    actionLigne->setCheckable(true);
    actionRectVide->setCheckable(true);
    actionRectPlein->setCheckable(true);
    actionElliVide->setCheckable(true);
    actionElliPlein->setCheckable(true);
    actionTexte->setCheckable(true);
    actionMain->setCheckable(true);
    actionAjoutPnj->setCheckable(true);
    actionSupprPnj->setCheckable(true);
    actionDeplacePnj->setCheckable(true);
    actionEtatPnj->setCheckable(true);

    actionMain->setChecked(true);
}

	/********************************************************************/
	/* Creation des boutons et du widget qui les contient               */
	/********************************************************************/	
void ToolsBar::creerOutils()
{
        // Creation des boutons du toolBar
        QToolButton *boutonCrayon     = new QToolButton();
        QToolButton *boutonLigne      = new QToolButton();
        QToolButton *boutonRectVide   = new QToolButton();
        QToolButton *boutonRectPlein  = new QToolButton();
        QToolButton *boutonElliVide   = new QToolButton();
        QToolButton *boutonElliPlein  = new QToolButton();
        QToolButton *boutonTexte      = new QToolButton();
        QToolButton *boutonMain       = new QToolButton();
        QToolButton *boutonAjoutPnj   = new QToolButton();
        QToolButton *boutonSupprPnj   = new QToolButton();
        QToolButton *boutonDeplacePnj = new QToolButton();
        QToolButton *boutonEtatPnj    = new QToolButton();
        QToolButton *boutonRazChrono  = new QToolButton();


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


        QVBoxLayout* outilsLayout = new QVBoxLayout();



        FlowLayout *toolsLayout = new FlowLayout();
        toolsLayout->setSpacing(0);
        toolsLayout->setMargin(0);
        toolsLayout->addWidget(boutonCrayon);
        toolsLayout->addWidget(boutonLigne);
        toolsLayout->addWidget(boutonRectVide);
        toolsLayout->addWidget(boutonRectPlein);
        toolsLayout->addWidget(boutonElliVide);
        toolsLayout->addWidget(boutonElliPlein);
        toolsLayout->addWidget(boutonTexte);
        toolsLayout->addWidget(boutonMain);

        ligneDeTexte = new QLineEdit();
        ligneDeTexte->setToolTip(tr("Text"));

        nomPnj = new QLineEdit();
        nomPnj->setToolTip(tr("NPC Name"));


        afficheNumeroPnj = new QLCDNumber(2);
        afficheNumeroPnj->setSegmentStyle(QLCDNumber::Flat);
        /// @todo used preferencemanager
        //afficheNumeroPnj->setMaximumSize(20 + 7, 20);
        afficheNumeroPnj->display(1);
        afficheNumeroPnj->setToolTip(tr("NPC's number"));

        m_currentNPCNumber = 1;


        couleur = new ColorSelector(this);





        FlowLayout *characterToolsLayout = new FlowLayout();
        characterToolsLayout->addWidget(boutonDeplacePnj);
        characterToolsLayout->addWidget(boutonEtatPnj);
        characterToolsLayout->addWidget(boutonAjoutPnj);
        characterToolsLayout->addWidget(boutonSupprPnj);
        characterToolsLayout->addWidget(boutonRazChrono);
        characterToolsLayout->addWidget(afficheNumeroPnj);


        m_lineDiameter = new DiameterSelector(m_centralWidget, true, 1, 45);
        m_lineDiameter->setToolTip(tr("Heigth of the pen"));
        connect(m_lineDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPenSizeChanged(int)));


        m_NpcDiameter = new DiameterSelector(m_centralWidget, false, 12, 41);
        connect(m_NpcDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPNCSizeChanged(int)));
        m_NpcDiameter->setToolTip(tr("Size of NPC"));






        outilsLayout->addWidget(couleur);
        outilsLayout->addLayout(toolsLayout);
        outilsLayout->addWidget(ligneDeTexte);
        outilsLayout->addWidget(m_lineDiameter);
        outilsLayout->addLayout(characterToolsLayout);
        outilsLayout->addWidget(nomPnj);
        outilsLayout->addWidget(m_NpcDiameter);
        //layout()->setAlignment(outils, Qt::AlignTop | Qt::AlignHCenter);
        m_centralWidget->setLayout(outilsLayout);

/// @todo used preferencemanager
// outils->setFixedWidth((20+8)*layoutDessin->columnCount());
//setMaximumWidth((20+8)*layoutDessin->columnCount()+10);
}


void ToolsBar::incrementeNumeroPnj()
{

    int numeroActuel = (int) afficheNumeroPnj->value();
    numeroActuel++;
    if (afficheNumeroPnj->checkOverflow(numeroActuel))
        afficheNumeroPnj->display(1);
    else
        afficheNumeroPnj->display(numeroActuel);
    m_currentNPCNumber = (int) afficheNumeroPnj->value();
}


void ToolsBar::razNumeroPnj()
{
    afficheNumeroPnj->display(1);
    m_currentNPCNumber = 1;
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
    actionTexte->trigger();
}


void ToolsBar::nomPnjChange(const QString &texte)
{
    actionAjoutPnj->trigger();
}


void ToolsBar::changeCurrentColor(QColor color)
{
    couleur->setCurrentColor(color);
    emit currentColorChanged(color);
}
QColor& ToolsBar::currentColor()
{
    return couleur->currentColor();
}


void ToolsBar::mettreAJourPnj(int diametre, QString nom)
{
    m_NpcDiameter->changerDiametre(diametre);
    nomPnj->setText(nom);
    m_currentNPCName = nom;
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
