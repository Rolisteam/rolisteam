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


#include <QtWidgets>

#include "vtoolbar.h"
#include "widgets/colorselector.h"
#include "widgets/diameterselector.h"
#include "widgets/flowlayout.h"


/**/
VToolsBar* VToolsBar::m_sigleton=NULL;

VToolsBar* VToolsBar::getInstance(QWidget *parent)
{
    if(m_sigleton==NULL)
        m_sigleton=new VToolsBar(parent);
    
    return m_sigleton;
}

VToolsBar::VToolsBar(QWidget *parent)
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
    
    connect(m_colorSelector,SIGNAL(currentColorChanged(QColor&)),this,SIGNAL(currentColorChanged(QColor&)));
    connect(m_colorSelector,SIGNAL(currentModeChanged(int)),this,SIGNAL(currentModeChanged(int)));
    m_currentTool = HANDLER;

    QObject::connect(m_resetCountAct, SIGNAL(triggered(bool)), this, SLOT(resetNpcCount()));
    QObject::connect(m_textEditLine, SIGNAL(textEdited(const QString &)), this, SLOT(changeText(const QString &)));
    QObject::connect(m_npcNameTextEdit, SIGNAL(textEdited(const QString &)), this, SLOT(npcNameChange(const QString &)));
    connect(m_toolsGroup,SIGNAL(triggered(QAction*)),this,SLOT(currentActionChanged(QAction*)));

    QObject::connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(changeSize(bool)));
    
    setFloating(false);
    
}


void VToolsBar::autoriserOuInterdireCouleurs()
{
    //m_colorSelector->allowOrForbideColors();
}


void VToolsBar::creerActions()
{
    // Creation du groupe d'action
    m_toolsGroup = new QActionGroup(this);
    
    
    m_pencilAct				= new QAction(QIcon(":/resources/icons/pen.png"), tr("Pen"), m_toolsGroup);
    m_lineAct				= new QAction(QIcon(":/resources/icons/line.png"), tr("Line"), m_toolsGroup);
    m_rectAct				= new QAction(QIcon(":/resources/icons/emptyrectangle.png"), tr("Empty Rectangle"), m_toolsGroup);
    m_rectFillAct           = new QAction(QIcon(":/resources/icons/filledrectangle.png"), tr("filled Rectangle"), m_toolsGroup);
    m_elipseAct             = new QAction(QIcon(":/resources/icons/emptyellipse.png"), tr("Empty Ellipse"), m_toolsGroup);
    m_elipseFillAct         = new QAction(QIcon(":/resources/icons/filledellipse.png"), tr("Filled Ellipse"), m_toolsGroup);
    m_textAct               = new QAction(QIcon(":/resources/icons/text.png"), tr("Text"), m_toolsGroup);
    m_handAct               = new QAction(QIcon(":/resources/icons/hand.png"), tr("Mode"), m_toolsGroup);
    m_addPCAct              = new QAction(QIcon(":/resources/icons/add.png"), tr("Add NPC"), m_toolsGroup);
    m_delNPCAct             = new QAction(QIcon(":/resources/icons/remove.png"), tr("Remove NPC"), m_toolsGroup);
    m_movePCAct             = new QAction(QIcon(":/resources/icons/moveNpc.png"), tr("Move/Turn Character"), m_toolsGroup);
    m_pcStateAct            = new QAction(QIcon(":/resources/icons/state.png"), tr("Change Character's State"), m_toolsGroup);
    
    
    m_resetCountAct	= new QAction(QIcon(":/resources/icons/chronometre.png"), tr("Reset NPC counter"), this);
    
    m_pencilAct->setCheckable(true);
    m_lineAct->setCheckable(true);
    m_rectAct->setCheckable(true);
    m_rectFillAct->setCheckable(true);
    m_elipseAct->setCheckable(true);
    m_elipseFillAct->setCheckable(true);
    m_textAct->setCheckable(true);
    m_handAct->setCheckable(true);
    m_addPCAct->setCheckable(true);
    m_delNPCAct->setCheckable(true);
    m_movePCAct->setCheckable(true);
    m_pcStateAct->setCheckable(true);
    
    m_handAct->setChecked(true);
}

/********************************************************************/
/* Creation des boutons et du widget qui les contient               */
/********************************************************************/
void VToolsBar::creerOutils()
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
    boutonCrayon->setDefaultAction(m_pencilAct);
    boutonLigne      ->setDefaultAction(m_lineAct);
    boutonRectVide   ->setDefaultAction(m_rectAct);
    boutonRectPlein  ->setDefaultAction(m_rectFillAct);
    boutonElliVide   ->setDefaultAction(m_elipseAct);
    boutonElliPlein  ->setDefaultAction(m_elipseFillAct);
    boutonTexte      ->setDefaultAction(m_textAct);
    boutonMain       ->setDefaultAction(m_handAct);
    boutonAjoutPnj   ->setDefaultAction(m_addPCAct);
    boutonSupprPnj   ->setDefaultAction(m_delNPCAct);
    boutonDeplacePnj ->setDefaultAction(m_movePCAct);
    boutonEtatPnj    ->setDefaultAction(m_pcStateAct);
    boutonRazChrono  ->setDefaultAction(m_resetCountAct);
    
    
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
    
    m_textEditLine = new QLineEdit();
    m_textEditLine->setToolTip(tr("Text"));
    
    m_npcNameTextEdit = new QLineEdit();
    m_npcNameTextEdit->setToolTip(tr("NPC Name"));
    
    
    m_displayNPCCounter = new QLCDNumber(2);
    m_displayNPCCounter->setSegmentStyle(QLCDNumber::Flat);
    /// @todo used preferencemanager
    //afficheNumeroPnj->setMaximumSize(20 + 7, 20);
    m_displayNPCCounter->display(1);
    m_displayNPCCounter->setToolTip(tr("NPC's number"));
    
    m_currentNPCNumber = 1;
    
    
    m_colorSelector = new VColorSelector(this);
    
    
    
    
    
    FlowLayout *characterToolsLayout = new FlowLayout();
    characterToolsLayout->addWidget(boutonDeplacePnj);
    characterToolsLayout->addWidget(boutonEtatPnj);
    characterToolsLayout->addWidget(boutonAjoutPnj);
    characterToolsLayout->addWidget(boutonSupprPnj);
    characterToolsLayout->addWidget(boutonRazChrono);
    characterToolsLayout->addWidget(m_displayNPCCounter);
    
    
    m_lineDiameter = new DiameterSelector(m_centralWidget, true, 1, 45);
    m_lineDiameter->setToolTip(tr("Heigth of the pen"));
    connect(m_lineDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPenSizeChanged(int)));
    
    
    m_NpcDiameter = new DiameterSelector(m_centralWidget, false, 12, 60);
    connect(m_NpcDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPNCSizeChanged(int)));
    m_NpcDiameter->setToolTip(tr("Size of NPC"));
    
    
    
    
    
    
    outilsLayout->addWidget(m_colorSelector);
    outilsLayout->addLayout(toolsLayout);
    outilsLayout->addWidget(m_textEditLine);
    outilsLayout->addWidget(m_lineDiameter);
    outilsLayout->addLayout(characterToolsLayout);
    outilsLayout->addWidget(m_npcNameTextEdit);
    outilsLayout->addWidget(m_NpcDiameter);
    outilsLayout->addStretch();
    //layout()->setAlignment(outils, Qt::AlignTop | Qt::AlignHCenter);
    m_centralWidget->setLayout(outilsLayout);
    
    /// @todo used preferencemanager
    // outils->setFixedWidth((20+8)*layoutDessin->columnCount());
    //setMaximumWidth((20+8)*layoutDessin->columnCount()+10);
}


void VToolsBar::incrementeNumeroPnj()
{
    
    int numeroActuel = (int) m_displayNPCCounter->value();
    numeroActuel++;
    if (m_displayNPCCounter->checkOverflow(numeroActuel))
        m_displayNPCCounter->display(1);
    else
        m_displayNPCCounter->display(numeroActuel);
    m_currentNPCNumber = (int) m_displayNPCCounter->value();
}


void VToolsBar::resetNpcCount()
{

            m_displayNPCCounter->display(1);
    m_currentNPCNumber = 1;

}

void VToolsBar::changeSize(bool floating)
{

    if (floating)
    {
        setFixedHeight(578);
    }
    else
    setMaximumHeight(0xFFFFFF);
}


void VToolsBar::changeText(const QString &texte)
{

    m_textAct->trigger();

}


void VToolsBar::npcNameChange(const QString &texte)
{ 
    m_addPCAct->trigger();
}


void VToolsBar::changeCurrentColor(QColor color)
{
    //m_colorSelector>setCurrentColor(color);
    emit currentColorChanged(color);
}
QColor& VToolsBar::currentColor()
{
   // return m_colorSelector->currentColor();
}


void VToolsBar::updateNPCSize(int diameter, QString name)
{
    m_NpcDiameter->changerDiametre(diameter);
    m_npcNameTextEdit->setText(name);
    m_currentNPCName = name;
}


void VToolsBar::majCouleursPersonnelles()
{
   // m_colorSelector->customColorUpdate();
}

QColor VToolsBar::donnerCouleurPersonnelle(int numero)
{
    //return m_colorSelector->getPersonalColor(numero);
    return QColor();
}
VToolsBar::SelectableTool VToolsBar::getCurrentTool()
{
    return m_currentTool;
}



void VToolsBar::currentActionChanged(QAction* p)
{
    //  enum SelectableTool {PEN, LINE, EMPTYRECT, FILLRECT, EMPTYELLIPSE, FILLEDELLIPSE, TEXT, HANDLER, ADDNPC, DELNPC, MOVECHARACTER, STATECHARACTER};
    if(p == m_pencilAct)
        m_currentTool = PEN;
    
    if(p ==  m_lineAct)
        m_currentTool = LINE;
    
    if(p == m_rectAct)
        m_currentTool = EMPTYRECT;
    
    if(p == m_rectFillAct)
        m_currentTool = FILLRECT;
    
    if(p ==  m_elipseAct)
        m_currentTool = EMPTYELLIPSE;
    
    if(p ==  m_elipseFillAct)
        m_currentTool = FILLEDELLIPSE;
    
    if(p ==  m_handAct)
        m_currentTool = HANDLER;
    
    if(p ==  m_textAct)
        m_currentTool = TEXT;
    
    if(p ==  m_addPCAct)
        m_currentTool = ADDNPC;
    
    if(p ==  m_delNPCAct)
        m_currentTool = DELNPC;
    
    if(p ==  m_movePCAct)
        m_currentTool = MOVECHARACTER;
    
    if(p ==  m_pcStateAct)
        m_currentTool = STATECHARACTER;
    
    emit currentToolChanged(m_currentTool);
    
    
}
