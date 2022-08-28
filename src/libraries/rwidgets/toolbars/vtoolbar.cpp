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

#include "rwidgets/customs/diameterselector.h"
#include "rwidgets/layouts/flowlayout.h"
#include "rwidgets/toolbars/vtoolbar.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "preferences/preferencesmanager.h"

HiddingButton::HiddingButton(QWidget* w) : QToolButton(w) {}

void HiddingButton::addAction(QAction* act)
{
    setDefaultAction(act);
    connect(act, &QAction::visibleChanged, this, [this, act]() { setVisible(act->isVisible()); });
}

VToolsBar::VToolsBar(VectorialMapController* ctrl, QWidget* parent) : QWidget(parent), m_ctrl(ctrl)
{
    setWindowTitle(tr("Tools"));
    setObjectName("Toolbar");
    setupUi();

    // UI to Ctrl
    connect(m_colorSelector, &VColorSelector::currentColorChanged, m_ctrl, &VectorialMapController::setToolColor);
    connect(m_lineDiameter, &DiameterSelector::diameterChanged, m_ctrl, &VectorialMapController::setPenSize);
    connect(m_editionModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        [this](int b) { m_ctrl->setEditionMode(static_cast<Core::EditionMode>(b)); });
    connect(m_resetCountAct, &QAction::triggered, this,
        [this]()
        {
            m_displayNPCCounter->display(1);
            m_ctrl->setNpcNumber(1);
        });
    connect(m_npcNameTextEdit, &QLineEdit::textEdited, m_ctrl, &VectorialMapController::setNpcName);
    connect(m_toolsGroup, &QActionGroup::triggered, this,
        [this](QAction* act)
        {
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
            m_addPCAct->setChecked(tool == Core::NonPlayableCharacter);
            m_bucketAct->setChecked(tool == Core::BUCKET);
            m_textWithBorderAct->setChecked(tool == Core::TEXTBORDER);
        });
    connect(m_opacitySlider, &QSlider::valueChanged, m_ctrl, &VectorialMapController::setOpacity);

    // Ctrl to UI
    connect(m_ctrl, &VectorialMapController::npcNumberChanged, m_displayNPCCounter,
        QOverload<int>::of(&QLCDNumber::display));
    connect(m_ctrl, &VectorialMapController::permissionChanged, this, &VToolsBar::updateUi);
    connect(m_ctrl, &VectorialMapController::editionModeChanged, this, &VToolsBar::updateUi);
    connect(m_ctrl, &VectorialMapController::toolColorChanged, m_colorSelector, &VColorSelector::setCurrentColor);
    // connect(m_ctrl, &VectorialMapController::opacityChanged, m_opacitySlider, &RealSlider::setRealValue);
    connect(m_ctrl, &VectorialMapController::editionModeChanged, this,
        [this](Core::EditionMode mode) { m_editionModeCombo->setCurrentIndex(static_cast<int>(mode)); });

    updateUi();
}

void VToolsBar::setupUi()
{
    m_centralWidget= new QWidget(this);
    createActions();
    makeTools();
    QHBoxLayout* lay= new QHBoxLayout();
    lay->setContentsMargins(QMargins());
    lay->addWidget(m_centralWidget);
    setLayout(lay);
}

void VToolsBar::createActions()
{
    m_toolsGroup= new QActionGroup(this);

    m_pencilAct= new QAction(QIcon::fromTheme("pen"), tr("Pen"), m_toolsGroup);
    m_pencilAct->setData(Core::PEN);

    m_lineAct= new QAction(QIcon::fromTheme("line"), tr("Line"), m_toolsGroup);
    m_lineAct->setData(Core::LINE);

    m_rectAct= new QAction(QIcon::fromTheme("emptyrectangle"), tr("Empty Rectangle"), m_toolsGroup);
    m_rectAct->setData(Core::EMPTYRECT);

    m_rectFillAct= new QAction(QIcon::fromTheme("filledrectangle"), tr("filled Rectangle"), m_toolsGroup);
    m_rectFillAct->setData(Core::FILLRECT);

    m_elipseAct= new QAction(QIcon::fromTheme("emptyellipse"), tr("Empty Ellipse"), m_toolsGroup);
    m_elipseAct->setData(Core::EMPTYELLIPSE);

    m_elipseFillAct= new QAction(QIcon::fromTheme("filledellipse"), tr("Filled Ellipse"), m_toolsGroup);
    m_elipseFillAct->setData(Core::FILLEDELLIPSE);

    m_textAct= new QAction(QIcon::fromTheme("text"), tr("Text"), m_toolsGroup);
    m_textAct->setData(Core::TEXT);

    m_handAct= new QAction(QIcon::fromTheme("hand"), tr("Hand"), m_toolsGroup);
    m_handAct->setData(Core::HANDLER);

    m_addPCAct= new QAction(QIcon::fromTheme("add"), tr("Add NPC"), m_toolsGroup);
    m_addPCAct->setData(Core::NonPlayableCharacter);

    m_ruleAct= new QAction(QIcon::fromTheme("rule"), tr("Rule"), m_toolsGroup);
    m_ruleAct->setData(Core::RULE);

    m_pathAct= new QAction(QIcon::fromTheme("path"), tr("Path"), m_toolsGroup);
    m_pathAct->setData(Core::PATH);
    m_anchorAct= new QAction(QIcon::fromTheme("insert-link-2"), tr("Anchor"), m_toolsGroup);
    m_anchorAct->setData(Core::ANCHOR);

    m_pipette= new QAction(QIcon::fromTheme("pipettecursor"), tr("Pipette"), m_toolsGroup);
    m_pipette->setData(Core::PIPETTE);

    m_highlighterAct= new QAction(QIcon::fromTheme("marker-512"), tr("Highlighter"), m_toolsGroup);
    m_highlighterAct->setData(Core::HIGHLIGHTER);

    m_bucketAct= new QAction(QIcon::fromTheme("000000-paint-bucket-512"), tr("Paint Bucket"), m_toolsGroup);
    m_bucketAct->setData(Core::BUCKET);

    m_textWithBorderAct= new QAction(QIcon::fromTheme("textwithBorder"), tr("Text With Border"), m_toolsGroup);
    m_textWithBorderAct->setData(Core::TEXTBORDER);

    m_resetCountAct= new QAction(QIcon(":/resources/images/chronometre.png"), tr("Reset NPC counter"), this);

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

    auto* penButton= new HiddingButton(this);
    auto* lineButton= new HiddingButton(this);
    auto* emptyRectButton= new HiddingButton(this);
    auto* filledRectButton= new HiddingButton(this);
    auto* emptyEllipseButton= new HiddingButton(this);
    auto* filledEllipseButton= new HiddingButton(this);
    auto* textButton= new HiddingButton(this);
    auto* handleButton= new HiddingButton(this);
    auto* addNpcButton= new HiddingButton(this);
    auto* resetNpcNumberButton= new HiddingButton(this);
    auto* ruleButton= new HiddingButton(this);
    auto* pathButton= new HiddingButton(this);
    auto* anchorButton= new HiddingButton(this);
    auto* pipetteButton= new HiddingButton(this);
    auto* bucketButton= new HiddingButton(this);
    auto* highlighterButton= new HiddingButton(this);

    penButton->addAction(m_pencilAct);
    lineButton->addAction(m_lineAct);
    emptyRectButton->addAction(m_rectAct);
    filledRectButton->addAction(m_rectFillAct);
    emptyEllipseButton->addAction(m_elipseAct);
    filledEllipseButton->addAction(m_elipseFillAct);
    textButton->addAction(m_textAct);
    handleButton->addAction(m_handAct);
    addNpcButton->addAction(m_addPCAct);
    resetNpcNumberButton->addAction(m_resetCountAct);
    ruleButton->addAction(m_ruleAct);
    pathButton->addAction(m_pathAct);
    pipetteButton->addAction(m_pipette);
    anchorButton->addAction(m_anchorAct);
    highlighterButton->addAction(m_highlighterAct);
    textButton->addAction(m_textWithBorderAct);
    bucketButton->addAction(m_bucketAct);
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

    auto pref= PreferencesManager::getInstance();
    auto iconSideSize= pref->value(QStringLiteral("IconSize"), 20).toInt();
    QSize iconSize(iconSideSize, iconSideSize);
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
    toolsVerticalLayout->setContentsMargins(QMargins());
    toolsVerticalLayout->setSpacing(0);

    FlowLayout* toolsLayout= new FlowLayout();
    toolsLayout->setSpacing(0);
    toolsLayout->setContentsMargins(QMargins());
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
    m_editionModeCombo->addItem(
        QIcon(":/resources/images/pen.png"), tr("Normal"), static_cast<int>(Core::EditionMode::Painting));
    m_editionModeCombo->addItem(
        QIcon(":/resources/images/mask.png"), tr("Mask"), static_cast<int>(Core::EditionMode::Mask));
    m_editionModeCombo->addItem(
        QIcon(":/resources/images/eye.png"), tr("Unmask"), static_cast<int>(Core::EditionMode::Unmask));

    /* connect(m_editionModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
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

                 if(tool == Core::RULE || tool == Core::PIPETTE || tool == Core::BUCKET
                    || tool == Core::NonPlayableCharacter || tool == Core::ANCHOR)
                     m_ctrl->setTool(Core::HANDLER);
             });*/

    FlowLayout* characterToolsLayout= new FlowLayout();
    characterToolsLayout->setContentsMargins(QMargins());
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
    m_opacityLabel= new QLabel(tr("Opacity:"), this);
    toolsVerticalLayout->addWidget(m_opacityLabel);
    toolsVerticalLayout->addWidget(m_opacitySlider);

    m_zoomSlider= std::make_unique<RealSlider>();
    m_zoomSlider->setOrientation(Qt::Horizontal);
    m_zoomSlider->setStart(0.01);
    m_zoomSlider->setEnd(5.0);

    m_zoomSpinBox= std::make_unique<QDoubleSpinBox>();
    m_zoomSpinBox->setMinimum(0.01);
    m_zoomSpinBox->setMaximum(5.0);

    m_smallScene= std::make_unique<QLabel>();
    m_smallScene->setScaledContents(true);

    // connect(m_zoomSlider.get(), &RealSlider::valueChanged, m_ctrl, &VectorialMapController::setZoomLevel);
    // connect(m_zoomSpinBox.get(), &QDoubleSpinBox::valueChanged, m_ctrl, &VectorialMapController::setZoomLevel);
    connect(m_ctrl, &VectorialMapController::zoomLevelChanged, this,
        [this]()
        {
            auto zl= m_ctrl->zoomLevel();
            qDebug() << zl;
            /*m_zoomSlider->blockSignals(true);
            m_zoomSpinBox->blockSignals(true);
            m_zoomSlider->setValue(zl);
            m_zoomSpinBox->setValue(zl);
            m_zoomSlider->blockSignals(false);
            m_zoomSpinBox->blockSignals(false);*/
        });
    // toolsVerticalLayout->addWidget(m_zoomSlider.get());
    // toolsVerticalLayout->addWidget(m_zoomSpinBox.get());
    toolsVerticalLayout->addWidget(m_smallScene.get());
    toolsVerticalLayout->addStretch(1);
    m_centralWidget->setLayout(toolsVerticalLayout);
}

void VToolsBar::updateUi()
{
    auto mode= m_ctrl->permission();
    auto editionMode= m_ctrl->editionMode();

    auto isGm= m_ctrl->localGM();

    auto pcMove= mode == Core::PC_MOVE;
    auto pcAll= mode == Core::PC_ALL;
    auto painting= editionMode == Core::EditionMode::Painting;
    auto isFow= m_ctrl->visibility() == Core::VisibilityMode::FOGOFWAR;

    if(!isFow)
    {
        m_editionModeCombo->setCurrentIndex(0);
        m_ctrl->setEditionMode(Core::EditionMode::Painting);
    }

    m_editionModeCombo->setVisible(isFow && (isGm || pcAll));

    m_pencilAct->setVisible(isGm || pcAll);
    m_lineAct->setVisible(isGm || pcAll);
    m_rectAct->setVisible(isGm || pcAll);
    m_rectFillAct->setVisible(isGm || pcAll);
    m_elipseAct->setVisible(isGm || pcAll);
    m_elipseFillAct->setVisible(isGm || pcAll);
    m_textAct->setVisible((isGm || pcAll) && painting);
    m_addPCAct->setVisible((isGm || pcAll) && painting);
    m_pipette->setVisible((isGm || pcAll) && painting);
    m_colorSelector->setVisible((isGm || pcAll) && painting);
    m_resetCountAct->setVisible((isGm || pcAll) && painting);
    m_pathAct->setVisible(isGm || pcAll);
    m_anchorAct->setVisible((isGm || pcAll) && painting);
    m_bucketAct->setVisible((isGm || pcAll) && painting);
    m_npcNameTextEdit->setVisible((isGm || pcAll) && painting);
    m_displayNPCCounter->setVisible((isGm || pcAll) && painting);
    m_opacityLabel->setVisible((isGm || pcAll) && painting);
    m_opacitySlider->setVisible((isGm || pcAll) && painting);
    m_textWithBorderAct->setVisible(isGm || pcAll);
    m_handAct->setVisible(isGm || pcAll || pcMove);
}

void VToolsBar::setImage(const QPixmap& img)
{
    m_smallScene->setPixmap(img);
}
