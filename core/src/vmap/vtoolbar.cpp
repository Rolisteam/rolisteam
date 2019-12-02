/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
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

#include "controller/media_controller/vectorialmapmediacontroller.h"

VToolsBar::VToolsBar(VectorialMapMediaController* ctrl, QWidget* parent) : QWidget(parent), m_ctrl(ctrl)
{
    setWindowTitle(tr("Tools"));
    setObjectName("Toolbar");
    setupUi();

    // UI to Ctrl
    connect(m_colorSelector, &VColorSelector::currentColorChanged, m_ctrl, &VectorialMapMediaController::setColor);
    connect(m_lineDiameter, &DiameterSelector::diameterChanged, m_ctrl, &VectorialMapMediaController::setPenSize);
    connect(m_editionModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int b) { m_ctrl->setEditionMode(static_cast<Core::EditionMode>(b)); });
    connect(m_resetCountAct, &QAction::triggered, this, [this]() {
        m_displayNPCCounter->display(1);
        m_ctrl->setNpcNumber(1);
    });
    connect(m_npcNameTextEdit, &QLineEdit::textEdited, m_ctrl, &VectorialMapMediaController::setNpcName);
    connect(m_toolsGroup, &QActionGroup::triggered, this, [this](QAction* act) {
        auto tool= static_cast<Core::SelectableTool>(act->data().toInt());
        m_ctrl->setTool(tool);
        m_ruleAct->setChecked(tool == Core::RULE);
        m_anchorAct->setChecked(tool == Core::ANCHOR);
        m_pathAct->setChecked(tool == Core::PATH);
        m_pipette->setChecked(tool == Core::PIPETTE);
        m_highlighterAct->setChecked(tool == Core::HIGHLIGHTER);
        m_pencilAct->setChecked(tool == Core::PEN);
        m_lineAct->setChecked(tool == Core::LINE);
        m_rectAct->setChecked(tool == Core::EMPTYRECT);
        m_rectFillAct->setChecked(tool == Core::FILLRECT);
        m_elipseAct->setChecked(tool == Core::EMPTYELLIPSE);
        m_elipseFillAct->setChecked(tool == Core::FILLEDELLIPSE);
        m_textAct->setChecked(tool == Core::TEXT);
        m_handAct->setChecked(tool == Core::HANDLER);
        m_addPCAct->setChecked(tool == Core::ADDNPC);
        m_bucketAct->setChecked(tool == Core::BUCKET);
        m_textWithBorderAct->setChecked(tool == Core::TEXTBORDER);
    });
    connect(m_opacitySlider, &QSlider::valueChanged, m_ctrl, &VectorialMapMediaController::setOpacity);

    // Ctrl to UI
    connect(m_ctrl, &VectorialMapMediaController::npcNumberChanged, m_displayNPCCounter,
            QOverload<int>::of(&QLCDNumber::display));
    connect(m_ctrl, &VectorialMapMediaController::colorChanged, m_colorSelector, &VColorSelector::setCurrentColor);
    connect(m_ctrl, &VectorialMapMediaController::opacityChanged, m_opacitySlider, &RealSlider::setRealValue);
    connect(m_ctrl, &VectorialMapMediaController::editionModeChanged, this,
            [this](Core::EditionMode mode) { m_editionModeCombo->setCurrentIndex(static_cast<int>(mode)); });
}

void VToolsBar::setupUi()
{
    m_centralWidget= new QWidget(this);
    createActions();
    makeTools();
    QHBoxLayout* lay= new QHBoxLayout();
    lay->setMargin(0);
    lay->addWidget(m_centralWidget);
    setLayout(lay);
}

void VToolsBar::createActions()
{
    m_toolsGroup= new QActionGroup(this);

    m_pencilAct= new QAction(QIcon(":/resources/icons/pen.png"), tr("Pen"), m_toolsGroup);
    m_pencilAct->setData(Core::PEN);

    m_lineAct= new QAction(QIcon(":/resources/icons/line.png"), tr("Line"), m_toolsGroup);
    m_lineAct->setData(Core::LINE);

    m_rectAct= new QAction(QIcon(":/resources/icons/emptyrectangle.png"), tr("Empty Rectangle"), m_toolsGroup);
    m_rectAct->setData(Core::EMPTYRECT);

    m_rectFillAct= new QAction(QIcon(":/resources/icons/filledrectangle.png"), tr("filled Rectangle"), m_toolsGroup);
    m_rectFillAct->setData(Core::FILLRECT);

    m_elipseAct= new QAction(QIcon(":/resources/icons/emptyellipse.png"), tr("Empty Ellipse"), m_toolsGroup);
    m_elipseAct->setData(Core::EMPTYELLIPSE);

    m_elipseFillAct= new QAction(QIcon(":/resources/icons/filledellipse.png"), tr("Filled Ellipse"), m_toolsGroup);
    m_elipseFillAct->setData(Core::FILLEDELLIPSE);

    m_textAct= new QAction(QIcon(":/resources/icons/text.png"), tr("Text"), m_toolsGroup);
    m_textAct->setData(Core::TEXT);

    m_handAct= new QAction(QIcon(":/resources/icons/hand.png"), tr("Hand"), m_toolsGroup);
    m_handAct->setData(Core::HANDLER);

    m_addPCAct= new QAction(QIcon(":/resources/icons/add.png"), tr("Add NPC"), m_toolsGroup);
    m_addPCAct->setData(Core::ADDNPC);

    m_ruleAct= new QAction(QIcon(":/resources/icons/rule.png"), tr("Rule"), m_toolsGroup);
    m_ruleAct->setData(Core::RULE);

    m_pathAct= new QAction(QIcon(":/resources/icons/path.png"), tr("Path"), m_toolsGroup);
    m_pathAct->setData(Core::PATH);
    m_anchorAct= new QAction(QIcon(":/resources/icons/insert-link-2.png"), tr("Anchor"), m_toolsGroup);
    m_anchorAct->setData(Core::ANCHOR);

    m_pipette= new QAction(QIcon(":/resources/icons/pipettecursor.png"), tr("Pipette"), m_toolsGroup);
    m_pipette->setData(Core::PIPETTE);

    m_highlighterAct= new QAction(QIcon(":/resources/icons/marker-512.png"), tr("Highlighter"), m_toolsGroup);
    m_highlighterAct->setData(Core::HIGHLIGHTER);

    m_bucketAct= new QAction(QIcon(":/resources/icons/000000-paint-bucket-512.png"), tr("Paint Bucket"), m_toolsGroup);
    m_bucketAct->setData(Core::BUCKET);

    m_textWithBorderAct
        = new QAction(QIcon(":/resources/icons/textwithBorder.png"), tr("Text With Border"), m_toolsGroup);
    m_textWithBorderAct->setData(Core::TEXTBORDER);

    m_resetCountAct= new QAction(QIcon(":/resources/icons/chronometre.png"), tr("Reset NPC counter"), this);

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
    m_bucketAct->setCheckable(true);

    m_anchorAct->setCheckable(true);

    m_handAct->setChecked(true);
}
void VToolsBar::makeTools()
{
    m_opacitySlider= new RealSlider(this);
    m_opacitySlider->setOrientation(Qt::Horizontal);
    m_opacitySlider->setRealValue(1.0);

    QToolButton* penButton= new QToolButton();
    QToolButton* lineButton= new QToolButton();
    QToolButton* emptyRectButton= new QToolButton();
    QToolButton* filledRectButton= new QToolButton();
    QToolButton* emptyEllipseButton= new QToolButton();
    QToolButton* filledEllipseButton= new QToolButton();
    QToolButton* textButton= new QToolButton();
    QToolButton* handleButton= new QToolButton();
    QToolButton* addNpcButton= new QToolButton();
    QToolButton* resetNpcNumberButton= new QToolButton();
    QToolButton* ruleButton= new QToolButton();
    QToolButton* pathButton= new QToolButton();
    QToolButton* anchorButton= new QToolButton();
    QToolButton* pipetteButton= new QToolButton();
    QToolButton* bucketButton= new QToolButton();
    QToolButton* highlighterButton= new QToolButton();

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
    bucketButton->setDefaultAction(m_bucketAct);
    //  unveilRect->setDefaultAction(m_unmaskRectAct);

    connect(ruleButton, &QToolButton::triggered, ruleButton, &QToolButton::setDefaultAction);
    connect(textButton, &QToolButton::triggered, textButton, &QToolButton::setDefaultAction);
    connect(filledRectButton, &QToolButton::triggered, filledRectButton, &QToolButton::setDefaultAction);
    connect(pathButton, &QToolButton::triggered, pathButton, &QToolButton::setDefaultAction);

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
    bucketButton->setAutoRaise(true);
    //   unveilRect->setAutoRaise(true);

    /**
     *
     * @todo  TODO used preferencemanager to get icon Size.
     *
     */
    QSize iconSize(20, 20);
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
    bucketButton->setIconSize(iconSize);

    QVBoxLayout* toolsVerticalLayout= new QVBoxLayout();
    toolsVerticalLayout->setMargin(0);
    toolsVerticalLayout->setSpacing(0);

    FlowLayout* toolsLayout= new FlowLayout();
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
    toolsLayout->addWidget(bucketButton);
    toolsLayout->addWidget(highlighterButton);

    m_npcNameTextEdit= new QLineEdit();
    m_npcNameTextEdit->setToolTip(tr("NPC Name"));

    m_displayNPCCounter= new QLCDNumber(2);
    m_displayNPCCounter->setSegmentStyle(QLCDNumber::Flat);
    m_displayNPCCounter->display(1);
    m_displayNPCCounter->setToolTip(tr("NPC's number"));

    m_colorSelector= new VColorSelector(this);
    m_editionModeCombo= new QComboBox();

    m_editionModeCombo->setFrame(false);
    m_editionModeCombo->addItem(QIcon(":/resources/icons/pen.png"), tr("Normal"),
                                static_cast<int>(Core::EditionMode::Painting));
    m_editionModeCombo->addItem(QIcon(":/resources/icons/mask.png"), tr("Mask"),
                                static_cast<int>(Core::EditionMode::Mask));
    m_editionModeCombo->addItem(QIcon(":/resources/icons/eye.png"), tr("Unmask"),
                                static_cast<int>(Core::EditionMode::Unmask));

    connect(m_editionModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this, ruleButton, pipetteButton, bucketButton, addNpcButton, anchorButton, highlighterButton]() {
                auto tool= m_ctrl->tool();
                auto mode= static_cast<Core::EditionMode>(m_editionModeCombo->currentData().toInt());
                bool painting= (mode == Core::EditionMode::Painting);

                ruleButton->setVisible(painting);
                pipetteButton->setVisible(painting);
                bucketButton->setVisible(painting);
                addNpcButton->setVisible(painting);
                anchorButton->setVisible(painting);
                highlighterButton->setVisible(painting);

                if(tool == Core::RULE || tool == Core::PIPETTE || tool == Core::BUCKET || tool == Core::ADDNPC
                   || tool == Core::ANCHOR)
                    m_ctrl->setTool(Core::HANDLER);
            });

    FlowLayout* characterToolsLayout= new FlowLayout();
    characterToolsLayout->setMargin(0);
    characterToolsLayout->setSpacing(0);
    characterToolsLayout->addWidget(addNpcButton);
    characterToolsLayout->addWidget(resetNpcNumberButton);
    characterToolsLayout->addWidget(m_displayNPCCounter);

    m_lineDiameter= new DiameterSelector(m_centralWidget, true, 1, 45);
    m_lineDiameter->setDiameter(15);
    m_lineDiameter->setToolTip(tr("height of the pen"));

    toolsVerticalLayout->addWidget(m_colorSelector);
    QFrame* line= new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    toolsVerticalLayout->addWidget(m_editionModeCombo);
    toolsVerticalLayout->addWidget(line);
    toolsVerticalLayout->addLayout(toolsLayout);
    toolsVerticalLayout->addWidget(m_lineDiameter);
    toolsVerticalLayout->addLayout(characterToolsLayout);
    toolsVerticalLayout->addWidget(m_npcNameTextEdit);
    toolsVerticalLayout->addWidget(new QLabel(tr("Opacity:"), this));
    toolsVerticalLayout->addWidget(m_opacitySlider);
    toolsVerticalLayout->addStretch(1);
    m_centralWidget->setLayout(toolsVerticalLayout);
}

void VToolsBar::updateUi(Core::PermissionMode mode)
{
    bool value= (m_isGM | (mode == Core::PC_ALL));

    m_pencilAct->setVisible(value);
    m_lineAct->setVisible(value);
    m_rectAct->setVisible(value);
    m_rectFillAct->setVisible(value);
    m_elipseAct->setVisible(value);
    m_elipseFillAct->setVisible(value);
    m_textAct->setVisible(value);
    m_addPCAct->setVisible(value);
    m_resetCountAct->setVisible(value);
    m_pathAct->setVisible(value);
    m_anchorAct->setVisible(value);
    m_textWithBorderAct->setVisible(value);
    m_handAct->setVisible(value | (mode == Core::PC_MOVE));
    m_ruleAct->setVisible(value | (mode == Core::PC_MOVE));
}

void VToolsBar::setGM(bool b)
{
    m_isGM= b;
}
