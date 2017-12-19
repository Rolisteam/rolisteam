/***************************************************************************
    *	Copyright (C) 2007 by Romain Campioni                                  *
    *	Copyright (C) 2009 by Renaud Guezennec                                 *
    *   http://www.rolisteam.org/contact                   *
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


VToolsBar* VToolsBar::m_sigleton=nullptr;

VToolsBar* VToolsBar::getInstance(QWidget *parent)
{
    if(m_sigleton==nullptr)
    {
        m_sigleton=new VToolsBar(parent);
    }
    
    return m_sigleton;
}

VToolsBar::VToolsBar(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Tools"));
    setObjectName("Toolbar");
    m_centralWidget = new QWidget(this);
    createActions();
    makeTools();
    QHBoxLayout* lay = new QHBoxLayout();
    lay->setMargin(0);
    lay->addWidget(m_centralWidget);
    setLayout(lay);
    connect(m_colorSelector,SIGNAL(currentColorChanged(QColor&)),this,SIGNAL(currentColorChanged(QColor&)));
    connect(m_colorSelector,SIGNAL(currentModeChanged(int)),this,SIGNAL(currentModeChanged(int)));
    connect(m_resetCountAct, SIGNAL(triggered(bool)), this, SLOT(resetNpcCount()));
    connect(m_npcNameTextEdit, SIGNAL(textEdited(const QString &)), this, SLOT(npcNameChange(const QString &)));
    connect(m_toolsGroup,SIGNAL(triggered(QAction*)),this,SLOT(currentActionChanged(QAction*)));


}

void VToolsBar::createActions()
{

    m_toolsGroup = new QActionGroup(this);
    
    m_pencilAct	= new QAction(QIcon(":/resources/icons/pen.png"), tr("Pen"), m_toolsGroup);
    m_pencilAct->setData(PEN);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_pencilAct->setChecked(tool==VToolsBar::PEN);
    });

    m_lineAct	= new QAction(QIcon(":/resources/icons/line.png"), tr("Line"), m_toolsGroup);
    m_lineAct->setData(LINE);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_lineAct->setChecked(tool==VToolsBar::LINE);
    });

    m_rectAct	= new QAction(QIcon(":/resources/icons/emptyrectangle.png"), tr("Empty Rectangle"), m_toolsGroup);
    m_rectAct->setData(EMPTYRECT);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_rectAct->setChecked(tool==VToolsBar::EMPTYRECT);
    });

    m_rectFillAct = new QAction(QIcon(":/resources/icons/filledrectangle.png"), tr("filled Rectangle"), m_toolsGroup);
    m_rectFillAct->setData(FILLRECT);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_rectFillAct->setChecked(tool==VToolsBar::FILLRECT);
    });

    m_elipseAct = new QAction(QIcon(":/resources/icons/emptyellipse.png"), tr("Empty Ellipse"), m_toolsGroup);
    m_elipseAct->setData(EMPTYELLIPSE);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_elipseAct->setChecked(tool==VToolsBar::EMPTYELLIPSE);
    });

    m_elipseFillAct  = new QAction(QIcon(":/resources/icons/filledellipse.png"), tr("Filled Ellipse"), m_toolsGroup);
    m_elipseFillAct->setData(FILLEDELLIPSE);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_elipseFillAct->setChecked(tool==VToolsBar::FILLEDELLIPSE);
    });

    m_textAct  = new QAction(QIcon(":/resources/icons/text.png"), tr("Text"), m_toolsGroup);
    m_textAct->setData(TEXT);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_textAct->setChecked(tool==VToolsBar::TEXT);
    });

    m_handAct = new QAction(QIcon(":/resources/icons/hand.png"), tr("Hand"), m_toolsGroup);
    m_handAct->setData(HANDLER);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_handAct->setChecked(tool==VToolsBar::HANDLER);
    });

    m_addPCAct= new QAction(QIcon(":/resources/icons/add.png"), tr("Add NPC"), m_toolsGroup);
    m_addPCAct->setData(ADDNPC);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_addPCAct->setChecked(tool==VToolsBar::ADDNPC);
    });

    m_ruleAct  = new QAction(QIcon(":/resources/icons/rule.png"),tr("Rule"),m_toolsGroup);
    m_ruleAct->setData(RULE);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_ruleAct->setChecked(tool==VToolsBar::RULE);
    });

    m_pathAct    = new QAction(QIcon(":/resources/icons/path.png"),tr("Path"),m_toolsGroup);
    m_pathAct->setData(PATH);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_pathAct->setChecked(tool==VToolsBar::PATH);
    });

    m_anchorAct  = new QAction(QIcon(":/resources/icons/insert-link-2.png"),tr("Anchor"),m_toolsGroup);
    m_anchorAct->setData(ANCHOR);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_anchorAct->setChecked(tool==VToolsBar::ANCHOR);
    });

    m_pipette = new QAction(QIcon(":/resources/icons/pipettecursor.png"),tr("Pipette"),m_toolsGroup);
    m_pipette->setData(PIPETTE);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_pipette->setChecked(tool==VToolsBar::PIPETTE);
    });

    m_highlighterAct = new QAction(QIcon(":/resources/icons/pipettecursor.png"),tr("Highlighter"),m_toolsGroup);
    m_highlighterAct->setData(HIGHLIGHTER);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
           m_highlighterAct->setChecked(tool==VToolsBar::HIGHLIGHTER);
    }); 


  /*  m_unmaskPathAct = new QAction(QIcon(":/resources/icons/maskpath.png"),tr("Unmask Path"),m_toolsGroup);
    m_unmaskPathAct->setData(PATHFOG);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_unmaskPathAct->setChecked(tool==VToolsBar::PATHFOG);
    });*/

    m_textWithBorderAct = new QAction(QIcon(":/resources/icons/textwithBorder.png"),tr("Text With Border"),m_toolsGroup);
    m_textWithBorderAct->setData(TEXTBORDER);
    connect(this,&VToolsBar::currentToolChanged,[=](VToolsBar::SelectableTool tool){
            m_textWithBorderAct->setChecked(tool==VToolsBar::TEXTBORDER);
    });

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
    m_pipette->setCheckable(true);
    m_highlighterAct->setCheckable(true);

    m_anchorAct->setCheckable(true);
    //m_unmaskPathAct->setCheckable(true);
  //  m_unmaskRectAct->setCheckable(true);
    
    m_handAct->setChecked(true);
}
void VToolsBar::makeTools()
{

    m_opacitySlider = new RealSlider(this);
    m_opacitySlider->setOrientation(Qt::Horizontal);
    m_opacitySlider->setRealValue(1.0);

    connect(m_opacitySlider,SIGNAL(valueChanged(qreal)),this, SIGNAL(opacityChanged(qreal)));

    QToolButton* penButton     = new QToolButton();
    QToolButton* lineButton      = new QToolButton();
    QToolButton* emptyRectButton   = new QToolButton();
    QToolButton* filledRectButton  = new QToolButton();
    QToolButton* emptyEllipseButton   = new QToolButton();
    QToolButton* filledEllipseButton  = new QToolButton();
    QToolButton* textButton      = new QToolButton();
    QToolButton* handleButton       = new QToolButton();
    QToolButton* addNpcButton   = new QToolButton();
    QToolButton* resetNpcNumberButton  = new QToolButton();
    QToolButton* ruleButton  = new QToolButton();
    QToolButton* pathButton  = new QToolButton();
    QToolButton* anchorButton  = new QToolButton();
    QToolButton* pipetteButton  = new QToolButton();
    QToolButton* highlighterButton  = new QToolButton();


    penButton->setDefaultAction(m_pencilAct);
    lineButton->setDefaultAction(m_lineAct);
    emptyRectButton->setDefaultAction(m_rectAct);
    filledRectButton->setDefaultAction(m_rectFillAct);
    emptyEllipseButton->setDefaultAction(m_elipseAct);
    filledEllipseButton->setDefaultAction(m_elipseFillAct);
    textButton->setDefaultAction(m_textAct);
    handleButton->setDefaultAction(m_handAct);
    addNpcButton->setDefaultAction(m_addPCAct);
    resetNpcNumberButton->setDefaultAction(m_resetCountAct);
    ruleButton->setDefaultAction(m_ruleAct);
    pathButton->setDefaultAction(m_pathAct);
    pipetteButton->setDefaultAction(m_pipette);
    anchorButton->setDefaultAction(m_anchorAct);
    highlighterButton->setDefaultAction(m_highlighterAct);
    textButton->addAction(m_textWithBorderAct);
  //  unveilRect->setDefaultAction(m_unmaskRectAct);

    connect(ruleButton,SIGNAL(triggered(QAction*)),ruleButton,SLOT(setDefaultAction(QAction*)));
    connect(textButton,SIGNAL(triggered(QAction*)),textButton,SLOT(setDefaultAction(QAction*)));
    connect(filledRectButton,SIGNAL(triggered(QAction*)),filledRectButton,SLOT(setDefaultAction(QAction*)));
    connect(pathButton,SIGNAL(triggered(QAction*)),pathButton,SLOT(setDefaultAction(QAction*)));
    
    penButton->setAutoRaise(true);
    lineButton->setAutoRaise(true);
    emptyRectButton->setAutoRaise(true);
    filledRectButton->setAutoRaise(true);
    emptyEllipseButton->setAutoRaise(true);
    filledEllipseButton->setAutoRaise(true);
    textButton->setAutoRaise(true);
    handleButton->setAutoRaise(true);
    addNpcButton->setAutoRaise(true);
    resetNpcNumberButton->setAutoRaise(true);
    ruleButton->setAutoRaise(true);
    pathButton->setAutoRaise(true);
    anchorButton->setAutoRaise(true);
    pipetteButton->setAutoRaise(true);
    highlighterButton->setAutoRaise(true);
 //   unveilRect->setAutoRaise(true);

    /**
    *
    * @todo used preferencemanager to get icon Size.
    *
    */
    QSize iconSize(20,20);
    penButton->setIconSize(iconSize);
    lineButton->setIconSize(iconSize);
    emptyRectButton->setIconSize(iconSize);
    filledRectButton->setIconSize(iconSize);
    emptyEllipseButton->setIconSize(iconSize);
    filledEllipseButton->setIconSize(iconSize);
    textButton->setIconSize(iconSize);
    handleButton->setIconSize(iconSize);
    addNpcButton->setIconSize(iconSize);
    resetNpcNumberButton->setIconSize(iconSize);
    ruleButton->setIconSize(iconSize);
    pathButton->setIconSize(iconSize);
    pipetteButton->setIconSize(iconSize);
    highlighterButton->setIconSize(iconSize);

    //maskModeButton->setIconSize(iconSize);
   // paintingModeButton->setIconSize(iconSize);

 //   QSize minSize(40,40);
 //   paintingModeButton->setFixedSize(minSize);
   // maskModeButton->setFixedSize(minSize);
  //  unmaskButton->setFixedSize(minSize);
    
    QVBoxLayout* toolsVerticalLayout = new QVBoxLayout();
    toolsVerticalLayout->setMargin(0);
    toolsVerticalLayout->setSpacing(0);

    FlowLayout *toolsLayout = new FlowLayout();
    toolsLayout->setSpacing(0);
    toolsLayout->setMargin(0);
    toolsLayout->addWidget(penButton);
    toolsLayout->addWidget(lineButton);
    toolsLayout->addWidget(emptyRectButton);
    toolsLayout->addWidget(filledRectButton);
    toolsLayout->addWidget(emptyEllipseButton);
    toolsLayout->addWidget(filledEllipseButton);
    toolsLayout->addWidget(textButton);
    toolsLayout->addWidget(handleButton);
    toolsLayout->addWidget(ruleButton);
    toolsLayout->addWidget(pathButton);
    toolsLayout->addWidget(anchorButton);
    toolsLayout->addWidget(pipetteButton);
    toolsLayout->addWidget(highlighterButton);
    //toolsLayout->addWidget(unveilRect);

    m_npcNameTextEdit = new QLineEdit();
    m_npcNameTextEdit->setToolTip(tr("NPC Name"));

    m_displayNPCCounter = new QLCDNumber(2);
    m_displayNPCCounter->setSegmentStyle(QLCDNumber::Flat);
    m_displayNPCCounter->display(1);
    m_displayNPCCounter->setToolTip(tr("NPC's number"));
    
    m_currentNPCNumber = 1;

    m_colorSelector = new VColorSelector(this);
    m_editionModeCombo = new QComboBox();

    m_editionModeCombo->setFrame(false);
    m_editionModeCombo->addItem(QIcon(":/resources/icons/pen.png"),tr("Normal"),Painting);
    m_editionModeCombo->addItem(QIcon(":/resources/icons/mask.png"),tr("Mask"),Mask);
    m_editionModeCombo->addItem(QIcon(":/resources/icons/eye.png"),tr("Unmask"),Unmask);

    FlowLayout* characterToolsLayout = new FlowLayout();
    characterToolsLayout->setMargin(0);
    characterToolsLayout->setSpacing(0);
    characterToolsLayout->addWidget(addNpcButton);
    characterToolsLayout->addWidget(resetNpcNumberButton);
    characterToolsLayout->addWidget(m_displayNPCCounter);


    m_lineDiameter = new DiameterSelector(m_centralWidget, true, 1, 45);
    m_lineDiameter->setDiameter(15);
    m_lineDiameter->setToolTip(tr("height of the pen"));
    connect(m_lineDiameter,SIGNAL(diameterChanged(int)),this,SIGNAL(currentPenSizeChanged(int)));

    toolsVerticalLayout->addWidget(m_colorSelector);
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    toolsVerticalLayout->addWidget(m_editionModeCombo);
    toolsVerticalLayout->addWidget(line);
    toolsVerticalLayout->addLayout(toolsLayout);
    toolsVerticalLayout->addWidget(m_lineDiameter);
    toolsVerticalLayout->addLayout(characterToolsLayout);
    toolsVerticalLayout->addWidget(m_npcNameTextEdit);
    toolsVerticalLayout->addWidget(new QLabel(tr("Opacity:"),this));
    toolsVerticalLayout->addWidget(m_opacitySlider);
    toolsVerticalLayout->addStretch(1);
    m_centralWidget->setLayout(toolsVerticalLayout);


    connect(m_editionModeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(currentEditionModeChange()));

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
void VToolsBar::currentEditionModeChange()
{
    EditionMode newtool = (EditionMode)m_editionModeCombo->currentData().toInt();
    if(newtool!=m_currentEditionMode)
    {
        m_currentEditionMode = newtool;
        emit currentEditionModeChanged(m_currentEditionMode);
    }
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
void VToolsBar::updateUi(Map::PermissionMode mode)
{
    if(!m_isGM)
    {
        switch(mode)
        {
        case Map::GM_ONLY:
            m_pencilAct->setVisible(false);
            m_lineAct->setVisible(false);
            m_rectAct->setVisible(false);
            m_rectFillAct->setVisible(false);
            m_elipseAct->setVisible(false);
            m_elipseFillAct->setVisible(false);
            m_textAct->setVisible(false);
            m_handAct->setVisible(false);
            m_addPCAct->setVisible(false);
            m_resetCountAct->setVisible(false);
            m_ruleAct->setVisible(false);
            m_pathAct->setVisible(false);
            m_anchorAct->setVisible(false);
   //         m_unmaskRectAct->setVisible(false);
    //        m_unmaskPathAct->setVisible(false);
            m_textWithBorderAct->setVisible(false);
            break;
        case Map::PC_ALL:
            m_pencilAct->setVisible(true);
            m_lineAct->setVisible(true);
            m_rectAct->setVisible(true);
            m_rectFillAct->setVisible(true);
            m_elipseAct->setVisible(true);
            m_elipseFillAct->setVisible(true);
            m_textAct->setVisible(true);
            m_handAct->setVisible(true);
            m_addPCAct->setVisible(true);
            m_resetCountAct->setVisible(true);
            m_ruleAct->setVisible(true);
            m_pathAct->setVisible(true);
            m_anchorAct->setVisible(true);
        //    m_unmaskRectAct->setVisible(true);
      //      m_unmaskPathAct->setVisible(true);
            m_textWithBorderAct->setVisible(true);
            break;
        case Map::PC_MOVE:
            m_pencilAct->setVisible(false);
            m_lineAct->setVisible(false);
            m_rectAct->setVisible(false);
            m_rectFillAct->setVisible(false);
            m_elipseAct->setVisible(false);
            m_elipseFillAct->setVisible(false);
            m_textAct->setVisible(false);
            m_addPCAct->setVisible(false);
            m_resetCountAct->setVisible(false);
            m_pathAct->setVisible(false);
            m_anchorAct->setVisible(false);
          //  m_unmaskRectAct->setVisible(false);
         //   m_unmaskPathAct->setVisible(false);
            m_textWithBorderAct->setVisible(false);
            m_handAct->setVisible(true);
            m_ruleAct->setVisible(true);
            break;

        }
    }
}

void VToolsBar::setGM(bool b)
{
    m_isGM = b;
}

void VToolsBar::setCurrentOpacity(qreal a)
{
    m_opacitySlider->setRealValue(a);
}

void VToolsBar::setCurrentColor(QColor color)
{
    m_colorSelector->setCurrentColor(color);
}
