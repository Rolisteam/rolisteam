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

VToolsBar* VToolsBar::m_sigleton=NULL;

VToolsBar* VToolsBar::getInstance(QWidget *parent)
{
    if(m_sigleton==NULL)
        m_sigleton=new VToolsBar(parent);
    
    return m_sigleton;
}

VToolsBar::VToolsBar(QWidget *parent)
    : QWidget(parent)
{
    
    setWindowTitle(tr("Tools"));
    setObjectName("Toolbar");
    //setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_centralWidget = new QWidget(this);
    creerActions();
    creerOutils();
    QHBoxLayout* lay = new QHBoxLayout();
    lay->addWidget(m_centralWidget);
    setLayout(lay);
    
    connect(m_colorSelector,SIGNAL(currentColorChanged(QColor&)),this,SIGNAL(currentColorChanged(QColor&)));
    connect(m_colorSelector,SIGNAL(currentModeChanged(int)),this,SIGNAL(currentModeChanged(int)));
    m_currentTool = HANDLER;

    QObject::connect(m_resetCountAct, SIGNAL(triggered(bool)), this, SLOT(resetNpcCount()));
    QObject::connect(m_npcNameTextEdit, SIGNAL(textEdited(const QString &)), this, SLOT(npcNameChange(const QString &)));
    connect(m_toolsGroup,SIGNAL(triggered(QAction*)),this,SLOT(currentActionChanged(QAction*)));
}

void VToolsBar::creerActions()
{
    // Creation du groupe d'action
    m_toolsGroup = new QActionGroup(this);
    
    m_pencilAct	= new QAction(QIcon(":/resources/icons/pen.png"), tr("Pen"), m_toolsGroup);
    m_pencilAct->setData(PEN);

    m_lineAct	= new QAction(QIcon(":/resources/icons/line.png"), tr("Line"), m_toolsGroup);
    m_lineAct->setData(LINE);

    m_rectAct	= new QAction(QIcon(":/resources/icons/emptyrectangle.png"), tr("Empty Rectangle"), m_toolsGroup);
    m_rectAct->setData(EMPTYRECT);

    m_rectFillAct = new QAction(QIcon(":/resources/icons/filledrectangle.png"), tr("filled Rectangle"), m_toolsGroup);
    m_rectFillAct->setData(FILLRECT);

    m_elipseAct = new QAction(QIcon(":/resources/icons/emptyellipse.png"), tr("Empty Ellipse"), m_toolsGroup);
    m_elipseAct->setData(EMPTYELLIPSE);

    m_elipseFillAct  = new QAction(QIcon(":/resources/icons/filledellipse.png"), tr("Filled Ellipse"), m_toolsGroup);
    m_elipseFillAct->setData(FILLEDELLIPSE);

    m_textAct  = new QAction(QIcon(":/resources/icons/text.png"), tr("Text"), m_toolsGroup);
    m_textAct->setData(TEXT);

    m_handAct = new QAction(QIcon(":/resources/icons/hand.png"), tr("Mode"), m_toolsGroup);
    m_handAct->setData(HANDLER);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_handAct->setChecked(tool==VToolsBar::HANDLER);
    });

    m_addPCAct= new QAction(QIcon(":/resources/icons/add.png"), tr("Add NPC"), m_toolsGroup);
    m_addPCAct->setData(ADDNPC);

    m_ruleAct  = new QAction(QIcon(":/resources/icons/rule.png"),tr("Rule"),m_toolsGroup);
    m_ruleAct->setData(RULE);

    m_pathAct    = new QAction(QIcon(":/resources/icons/path.png"),tr("Path"),m_toolsGroup);
    m_pathAct->setData(PATH);

    m_anchorAct  = new QAction(QIcon(":/resources/icons/insert-link-2.png"),tr("Anchor"),m_toolsGroup);
    m_anchorAct->setData(ANCHOR);

    m_unmaskRectAct = new QAction(QIcon(":/resources/icons/draw-rectangle-2.png"),tr("Unmask Rect"),m_toolsGroup);
    m_unmaskRectAct->setData(RECTFOG);

    m_unmaskPathAct = new QAction(QIcon(":/resources/icons/maskpath.png"),tr("Unmask Path"),m_toolsGroup);
    m_unmaskPathAct->setData(PATHFOG);

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
    m_ruleAct->setCheckable(true);
    m_pathAct->setCheckable(true);

    m_anchorAct->setCheckable(true);
    m_unmaskPathAct->setCheckable(true);
    m_unmaskRectAct->setCheckable(true);
    
    m_handAct->setChecked(true);
}
void VToolsBar::creerOutils()
{
    QToolButton* boutonCrayon     = new QToolButton();
    QToolButton* boutonLigne      = new QToolButton();
    QToolButton* boutonRectVide   = new QToolButton();
    QToolButton* boutonRectPlein  = new QToolButton();
    QToolButton* boutonElliVide   = new QToolButton();
    QToolButton* boutonElliPlein  = new QToolButton();
    QToolButton* boutonTexte      = new QToolButton();
    QToolButton* boutonMain       = new QToolButton();
    QToolButton* boutonAjoutPnj   = new QToolButton();
    QToolButton* boutonRazChrono  = new QToolButton();
    QToolButton* ruleButton  = new QToolButton();
    QToolButton* pathButton  = new QToolButton();
  /*  QToolButton* unmaskRectButton  = new QToolButton();
    QToolButton* unmaskPathButton  = new QToolButton();
    QToolButton* anchorButton  = new QToolButton();*/
    
    boutonCrayon->setDefaultAction(m_pencilAct);
    boutonLigne      ->setDefaultAction(m_lineAct);
    boutonRectVide   ->setDefaultAction(m_rectAct);
    boutonRectPlein  ->setDefaultAction(m_rectFillAct);
    boutonElliVide   ->setDefaultAction(m_elipseAct);
    boutonElliPlein  ->setDefaultAction(m_elipseFillAct);
    boutonTexte      ->setDefaultAction(m_textAct);
    boutonMain       ->setDefaultAction(m_handAct);
    boutonAjoutPnj   ->setDefaultAction(m_addPCAct);
    boutonRazChrono  ->setDefaultAction(m_resetCountAct);
    ruleButton->setDefaultAction(m_ruleAct);
    pathButton->setDefaultAction(m_pathAct);
    boutonRectPlein->addAction(m_unmaskRectAct);
    pathButton->addAction(m_unmaskPathAct);
    ruleButton->addAction(m_anchorAct);
    
    boutonCrayon     ->setAutoRaise(true);
    boutonLigne      ->setAutoRaise(true);
    boutonRectVide   ->setAutoRaise(true);
    boutonRectPlein  ->setAutoRaise(true);
    boutonElliVide   ->setAutoRaise(true);
    boutonElliPlein  ->setAutoRaise(true);
    boutonTexte      ->setAutoRaise(true);
    boutonMain       ->setAutoRaise(true);
    boutonAjoutPnj   ->setAutoRaise(true);
    boutonRazChrono  ->setAutoRaise(true);
    ruleButton->setAutoRaise(true);
    pathButton->setAutoRaise(true);

    /**
    *
    * @todo used preferencemanager to get icon Size.
    *
    */
    QSize iconSize(20,20);
    boutonCrayon     ->setIconSize(iconSize);
    boutonLigne      ->setIconSize(iconSize);
    boutonRectVide   ->setIconSize(iconSize);
    boutonRectPlein  ->setIconSize(iconSize);
    boutonElliVide   ->setIconSize(iconSize);
    boutonElliPlein  ->setIconSize(iconSize);
    boutonTexte      ->setIconSize(iconSize);
    boutonMain       ->setIconSize(iconSize);
    boutonAjoutPnj   ->setIconSize(iconSize);
    boutonRazChrono  ->setIconSize(iconSize);
    ruleButton->setIconSize(iconSize);
    pathButton->setIconSize(iconSize);

    
    
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
    toolsLayout->addWidget(ruleButton);
    toolsLayout->addWidget(pathButton);




    m_npcNameTextEdit = new QLineEdit();
    m_npcNameTextEdit->setToolTip(tr("NPC Name"));

    m_displayNPCCounter = new QLCDNumber(2);
    m_displayNPCCounter->setSegmentStyle(QLCDNumber::Flat);
    m_displayNPCCounter->display(1);
    m_displayNPCCounter->setToolTip(tr("NPC's number"));
    
    m_currentNPCNumber = 1;

    m_colorSelector = new VColorSelector(this);

    FlowLayout *characterToolsLayout = new FlowLayout();
    characterToolsLayout->addWidget(boutonAjoutPnj);
    characterToolsLayout->addWidget(boutonRazChrono);
    characterToolsLayout->addWidget(m_displayNPCCounter);

    m_lineDiameter = new DiameterSelector(m_centralWidget, true, 1, 45);
    m_lineDiameter->setDiameter(15);
    m_lineDiameter->setToolTip(tr("height of the pen"));
    connect(m_lineDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPenSizeChanged(int)));

    outilsLayout->addWidget(m_colorSelector);
    outilsLayout->addLayout(toolsLayout);
    outilsLayout->addWidget(m_lineDiameter);
    outilsLayout->addLayout(characterToolsLayout);
    outilsLayout->addWidget(m_npcNameTextEdit);
    outilsLayout->addStretch(1);
    m_centralWidget->setLayout(outilsLayout);

}
void VToolsBar::increaseNpcNumber()
{
    int currentNumber = (int) m_displayNPCCounter->value();
    ++currentNumber;
    if (m_displayNPCCounter->checkOverflow(currentNumber))
        m_displayNPCCounter->display(1);
    else
        m_displayNPCCounter->display(currentNumber);
    m_currentNPCNumber = (int) m_displayNPCCounter->value();
    emit currentNpcNumberChanged(m_currentNPCNumber);
}

void VToolsBar::resetNpcCount()
{
    m_displayNPCCounter->display(1);
    m_currentNPCNumber = 1;
    emit currentNpcNumberChanged(m_currentNPCNumber);
}

void VToolsBar::npcNameChange(const QString &text)
{ 
    m_addPCAct->trigger();
    emit currentNpcNameChanged(text);
}

void VToolsBar::changeCurrentColor(QColor color)
{
    //m_colorSelector>setCurrentColor(color);
    emit currentColorChanged(color);
}

VToolsBar::SelectableTool VToolsBar::getCurrentTool()
{
    return m_currentTool;
}
void VToolsBar::currentActionChanged(QAction* p)
{
    SelectableTool newtool = (SelectableTool)p->data().toInt();
    if(newtool!=m_currentTool)
    {
        m_currentTool = newtool;
        emit currentToolChanged(m_currentTool);
    }
}
int VToolsBar::getCurrentPenSize()
{
    return m_lineDiameter->getCurrentValue();
}
void VToolsBar::setCurrentTool(VToolsBar::SelectableTool tool)
{
    if(m_currentTool!=tool)
    {
        m_currentTool=tool;
        emit currentToolChanged(m_currentTool);
    }
}
