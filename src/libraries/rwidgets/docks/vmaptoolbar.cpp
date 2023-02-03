/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
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

#include "rwidgets/docks/vmaptoolbar.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include "common_widgets/colorbutton.h"
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QToolButton>

VmapTopBar::VmapTopBar(VectorialMapController* ctrl, QWidget* parent) : QToolBar(parent), m_ctrl(ctrl)
{
    setObjectName("VmapTopBar");
    setWindowTitle(tr("TopBar for VMap"));
    initActions();
    setupUi();

    connect(m_ctrl, &VectorialMapController::localGMChanged, this,
            [this](bool b)
            {
                setEnabled(b);
                setVisible(b);
            });

    if(m_ctrl)
    {
        setEnabled(m_ctrl->localGM());
        setVisible(m_ctrl->localGM());
    }
}

VmapTopBar::~VmapTopBar()
{
    qDebug() << "Destructor topbar;";
}

void VmapTopBar::initActions()
{
    if(!m_ctrl)
        return;

    m_showSquareAct= new QAction(QIcon::fromTheme("grid"), tr("Square Grid"), this);
    m_showSquareAct->setObjectName("grid_square");
    m_showSquareAct->setCheckable(true);

    m_showHexagonAct= new QAction(QIcon::fromTheme("hexa_grid"), tr("Hexagonal Grid"), this);
    m_showSquareAct->setObjectName("grid_hexa");
    m_showHexagonAct->setCheckable(true);

    auto group= new QActionGroup(this);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
    group->addAction(m_showSquareAct);
    group->addAction(m_showHexagonAct);

    connect(group, &QActionGroup::triggered, this,
            [this](QAction* action)
            {
                m_ctrl->setGridVisibility(action->isChecked());
                if(action == m_showSquareAct)
                    m_ctrl->setGridPattern(Core::GridPattern::SQUARE);
                else
                    m_ctrl->setGridPattern(Core::GridPattern::HEXAGON);
            });

    m_gridAboveAct= new QAction(QIcon::fromTheme("grid_above"), tr("Grid Above"), this);
    m_gridAboveAct->setObjectName("grid_above");
    m_gridAboveAct->setCheckable(true);

    connect(m_gridAboveAct, &QAction::triggered, m_ctrl, &VectorialMapController::setGridAbove);

    m_onlyGmPermAct= new QAction(QIcon::fromTheme("red_round"), tr("Permission: GM only"), this);
    m_onlyGmPermAct->setObjectName("permission_gm");
    m_onlyGmPermAct->setData(QVariant::fromValue(Core::PermissionMode::GM_ONLY));
    m_onlyGmPermAct->setCheckable(true);
    addAction(m_onlyGmPermAct);

    m_characterOnlyPermAct= new QAction(QIcon::fromTheme("orange_round"), tr("Permission: PC Move"), this);
    m_characterOnlyPermAct->setObjectName("permission_pc");
    m_characterOnlyPermAct->setData(QVariant::fromValue(Core::PermissionMode::PC_MOVE));
    m_characterOnlyPermAct->setCheckable(true);
    addAction(m_characterOnlyPermAct);

    m_allPermAct= new QAction(QIcon::fromTheme("green_round"), tr("Permission: All"), this);
    m_allPermAct->setObjectName("permission_all");
    m_allPermAct->setData(QVariant::fromValue(Core::PermissionMode::PC_ALL));
    m_allPermAct->setCheckable(true);
    addAction(m_allPermAct);

    group= new QActionGroup(this);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
    group->addAction(m_onlyGmPermAct);
    group->addAction(m_characterOnlyPermAct);
    group->addAction(m_allPermAct);

    connect(group, &QActionGroup::triggered, this,
            [this](QAction* action) { m_ctrl->setPermission(action->data().value<Core::PermissionMode>()); });

    auto updatePerm= [this](Core::PermissionMode perm)
    {
        m_onlyGmPermAct->setChecked(perm == Core::PermissionMode::GM_ONLY);
        m_characterOnlyPermAct->setChecked(perm == Core::PermissionMode::PC_MOVE);
        m_allPermAct->setChecked(perm == Core::PermissionMode::PC_ALL);
    };
    connect(m_ctrl, &VectorialMapController::permissionChanged, this, updatePerm);
    updatePerm(m_ctrl->permission());

    m_hiddenAct= new QAction(QIcon::fromTheme("mask"), tr("Visibility: Hidden"), this);
    m_hiddenAct->setObjectName("Visibility_hidden");
    m_hiddenAct->setData(QVariant::fromValue(Core::VisibilityMode::HIDDEN));
    m_hiddenAct->setCheckable(true);
    addAction(m_hiddenAct);

    m_fogAct= new QAction(QIcon::fromTheme("fogofwar"), tr("Visibility: Fog"), this);
    m_fogAct->setObjectName("Visibility_fow");
    m_fogAct->setData(QVariant::fromValue(Core::VisibilityMode::FOGOFWAR));
    m_fogAct->setCheckable(true);
    addAction(m_fogAct);

    m_allAct= new QAction(QIcon::fromTheme("eye"), tr("Visibility: All"), this);
    m_allAct->setObjectName("Visibility_all");
    m_allAct->setData(QVariant::fromValue(Core::VisibilityMode::ALL));
    m_allAct->setCheckable(true);
    addAction(m_allAct);

    group= new QActionGroup(this);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
    group->addAction(m_hiddenAct);
    group->addAction(m_fogAct);
    group->addAction(m_allAct);

    connect(group, &QActionGroup::triggered, this,
            [this](QAction* action) { m_ctrl->setVisibility(action->data().value<Core::VisibilityMode>()); });

    auto updateVisiblility= [this](Core::VisibilityMode perm)
    {
        m_hiddenAct->setChecked(perm == Core::VisibilityMode::HIDDEN);
        m_fogAct->setChecked(perm == Core::VisibilityMode::FOGOFWAR);
        m_allAct->setChecked(perm == Core::VisibilityMode::ALL);
    };
    updateVisiblility(m_ctrl->visibility());
    connect(m_ctrl, &VectorialMapController::visibilityChanged, this, updateVisiblility);

    m_groundAct= new QAction(QIcon::fromTheme("ground_layer"), tr("Current Layer: Ground"), this);
    m_groundAct->setObjectName("layer_ground");
    addAction(m_groundAct);
    m_groundAct->setData(QVariant::fromValue(Core::Layer::GROUND));
    m_groundAct->setCheckable(true);

    m_objectAct= new QAction(QIcon::fromTheme("object_layer"), tr("Current Layer: Object"), this);
    m_objectAct->setObjectName("layer_object");
    addAction(m_objectAct);
    m_objectAct->setData(QVariant::fromValue(Core::Layer::OBJECT));
    m_objectAct->setCheckable(true);

    m_characterAct= new QAction(QIcon::fromTheme("character_layer"), tr("Current Layer: Character"), this);
    m_characterAct->setObjectName("layer_character");
    addAction(m_characterAct);
    m_characterAct->setData(QVariant::fromValue(Core::Layer::CHARACTER_LAYER));
    m_characterAct->setCheckable(true);

    m_gameMasterAct= new QAction(QIcon::fromTheme("gm_icon"), tr("Current Layer: GameMaster"), this);
    m_gameMasterAct->setObjectName("layer_gm");
    addAction(m_gameMasterAct);
    m_gameMasterAct->setData(QVariant::fromValue(Core::Layer::GAMEMASTER_LAYER));
    m_gameMasterAct->setCheckable(true);

    group= new QActionGroup(this);
    group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
    group->addAction(m_groundAct);
    group->addAction(m_objectAct);
    group->addAction(m_characterAct);
    group->addAction(m_gameMasterAct);

    connect(group, &QActionGroup::triggered, this,
            [this](QAction* action) { m_ctrl->setLayer(action->data().value<Core::Layer>()); });

    auto updateLayer= [this](Core::Layer layer)
    {
        m_groundAct->setChecked(layer == Core::Layer::GROUND);
        m_objectAct->setChecked(layer == Core::Layer::OBJECT);
        m_characterAct->setChecked(layer == Core::Layer::CHARACTER_LAYER);
        m_gameMasterAct->setChecked(layer == Core::Layer::GAMEMASTER_LAYER);
    };
    connect(m_ctrl, &VectorialMapController::layerChanged, this, updateLayer);
    updateLayer(m_ctrl->layer());

    m_hideOtherAct= new QAction(QIcon::fromTheme("hideotherlayers"), tr("hide other layers"), this);
    m_hideOtherAct->setObjectName("hide_layer");
    addAction(m_hideOtherAct);
    m_hideOtherAct->setCheckable(true);

    m_characterVisionAct= new QAction(QIcon::fromTheme("sight"), tr("Character Vision"), this);
    m_characterVisionAct->setCheckable(true);
    m_characterVisionAct->setObjectName("character_vision");
    addAction(m_characterVisionAct);
    m_showTransparentAct= new QAction(QIcon::fromTheme("sun"), tr("Show Transparent Item"), this);
    m_showTransparentAct->setCheckable(true);
    m_showTransparentAct->setObjectName("show_transparent");
    addAction(m_showTransparentAct);

    m_showPcName= new QAction(QIcon::fromTheme("pcName"), tr("Show/Hide PC name", "PC = playable character"), this);
    m_showPcName->setObjectName("show_pc_name");
    addAction(m_showPcName);
    m_showNpcName
        = new QAction(QIcon::fromTheme("npcName"), tr("Show/Hide NPC name", "NPC = non-playable character"), this);
    m_showNpcName->setObjectName("show_npc_name");
    addAction(m_showNpcName);
    m_showNpcNumber= new QAction(QIcon::fromTheme("npcNumber"), tr("Show/Hide NPC number"), this);
    m_showNpcNumber->setObjectName("show_npc_number");
    addAction(m_showNpcNumber);
    m_showState= new QAction(QIcon::fromTheme("stateName"), tr("Show/Hide State"), this); //
    m_showState->setObjectName("show_states");
    addAction(m_showState);
    m_showHealthBar= new QAction(QIcon::fromTheme("healthBar"), tr("Show/Hide Health Bar"), this);
    m_showHealthBar->setObjectName("show_health");
    addAction(m_showHealthBar);
    m_showInit= new QAction(QIcon::fromTheme("initScore"), tr("Show/Hide Initiative"), this);
    m_showInit->setObjectName("show_initScore");
    addAction(m_showInit);

    m_showPcName->setCheckable(true);
    m_showNpcName->setCheckable(true);
    m_showNpcNumber->setCheckable(true);
    m_showState->setCheckable(true);
    m_showHealthBar->setCheckable(true);
    m_showInit->setCheckable(true);

    connect(m_showPcName, &QAction::triggered, m_ctrl, &VectorialMapController::setPcNameVisible);
    connect(m_showNpcName, &QAction::triggered, m_ctrl, &VectorialMapController::setNpcNameVisible);
    connect(m_showNpcNumber, &QAction::triggered, m_ctrl, &VectorialMapController::setNpcNumberVisible);
    connect(m_showState, &QAction::triggered, m_ctrl, &VectorialMapController::setStateLabelVisible);
    connect(m_showHealthBar, &QAction::triggered, m_ctrl, &VectorialMapController::setHealthBarVisible);
    connect(m_showInit, &QAction::triggered, m_ctrl, &VectorialMapController::setInitScoreVisible);
    connect(m_characterVisionAct, &QAction::triggered, m_ctrl, &VectorialMapController::setCharacterVision);

    m_showPcName->setChecked(m_ctrl->pcNameVisible());
    m_showNpcName->setChecked(m_ctrl->npcNameVisible());
    m_showNpcNumber->setChecked(m_ctrl->npcNumberVisible());
    m_showState->setChecked(m_ctrl->stateLabelVisible());
    m_showHealthBar->setChecked(m_ctrl->healthBarVisible());
    m_showInit->setChecked(m_ctrl->initScoreVisible());
}

void VmapTopBar::setupUi()
{
    if(!m_ctrl)
        return;
    // bg
    m_bgSelector= new ColorButton();
    m_bgSelector->setColor(m_ctrl->backgroundColor());
    addWidget(m_bgSelector);
    connect(m_bgSelector, &ColorButton::colorChanged, m_ctrl, &VectorialMapController::setBackgroundColor);
    addSeparator();

    // grid
    auto btn= new QToolButton();
    btn->setDefaultAction(m_showSquareAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_showHexagonAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_gridAboveAct);
    addWidget(btn);

    m_gridUnit= new QComboBox();
    QStringList listUnit;
    listUnit << QStringLiteral("m") << QStringLiteral("km") << QStringLiteral("cm") << QStringLiteral("mi")
             << QStringLiteral("yd") << tr("inch") << tr("foot") << tr("px");
    m_gridUnit->addItems(listUnit);
    // connect(m_gridUnit, &QComboBox::currentIndexChanged, m_ctrl, &VectorialMapController::setScaleUnit);

    m_gridSize= new QSpinBox(this);
    m_gridSize->setRange(1, std::numeric_limits<int>::max());
    connect(m_gridSize, QOverload<int>::of(&QSpinBox::valueChanged), m_ctrl, &VectorialMapController::setGridSize);
    m_gridSize->setValue(m_ctrl->gridSize());

    m_scaleSize= new QDoubleSpinBox(this);
    m_scaleSize->setRange(0.1, std::numeric_limits<double>::max());
    connect(m_scaleSize, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged), m_ctrl,
            &VectorialMapController::setGridScale);
    m_scaleSize->setValue(m_ctrl->gridScale());

    addWidget(m_gridSize);
    addWidget(new QLabel(tr("px")));
    addWidget(m_scaleSize);
    addWidget(m_gridUnit);

    addSeparator();
    // permission
    btn= new QToolButton();
    btn->setDefaultAction(m_onlyGmPermAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_characterOnlyPermAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_allPermAct);
    addWidget(btn);

    addSeparator();
    // visibility
    btn= new QToolButton();
    btn->setDefaultAction(m_hiddenAct);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_fogAct);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_allAct);
    addWidget(btn);

    addSeparator();
    // layer
    btn= new QToolButton();
    btn->setDefaultAction(m_groundAct);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_objectAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_characterAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_gameMasterAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_hideOtherAct);
    addWidget(btn);

    addSeparator();
    // character vision
    btn= new QToolButton();
    btn->setDefaultAction(m_characterVisionAct);
    addWidget(btn);

    btn= new QToolButton();
    btn->setDefaultAction(m_showTransparentAct);
    addWidget(btn);

    connect(m_hideOtherAct, &QAction::triggered, m_ctrl, &VectorialMapController::hideOtherLayers);

    addSeparator();

    btn= new QToolButton();
    btn->setDefaultAction(m_showPcName);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_showNpcName);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_showNpcNumber);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_showState);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_showHealthBar);
    addWidget(btn);
    btn= new QToolButton();
    btn->setDefaultAction(m_showInit);
    addWidget(btn);
}

void VmapTopBar::updateUI()
{
    // if(nullptr != m_vmap)
    {
        setEnabled(true);
        // m_bgSelector->setColor(m_vmap->getBackGroundColor());
        /*     m_showGridAct->setChecked(m_vmap->getOption(VisualItem::ShowGrid).toBool());
             m_gridSize->setValue(m_vmap->getOption(VisualItem::GridSize).toInt());
             m_currentLayer->setCurrentIndex(static_cast<int>(m_vmap->currentLayer()));
             m_showCharacterVision->setChecked(m_vmap->getOption(VisualItem::EnableCharacterVision).toBool());
             m_gridUnit->setCurrentIndex(m_vmap->getOption(VisualItem::Unit).toInt());
             m_currentPermission->setCurrentIndex(m_vmap->getOption(VisualItem::PermissionMode).toInt());
             m_currentVisibility->setCurrentIndex(m_vmap->getOption(VisualItem::VisibilityMode).toInt());
             m_gridPattern->setCurrentIndex(m_vmap->getOption(VisualItem::GridPattern).toInt());
             m_collision->setChecked(m_vmap->getOption(VisualItem::CollisionStatus).toBool());
             m_showOnlyItemsFromThisLayer->setChecked(m_vmap->getOption(VisualItem::HideOtherLayers).toBool());
             m_currentLayer->setCurrentIndex(m_vmap->getOption(VisualItem::MapLayer).toInt());
             m_gridAbove->setChecked(m_vmap->getOption(VisualItem::GridAbove).toBool());
             m_scaleSize->setValue(m_vmap->getOption(VisualItem::Scale).toReal());*/
    }
    /*  else
      {
          setEnabled(false);
      }*/
}
