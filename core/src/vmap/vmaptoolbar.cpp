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

#include "vmaptoolbar.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"

VmapToolBar::VmapToolBar(VectorialMapMediaController* ctrl, QWidget* parent) : QToolBar(parent), m_ctrl(ctrl)
{
    setObjectName("VMapToolBar");
    setWindowTitle(tr("Toolbar for VMap"));
    setupUi();
}

VmapToolBar::~VmapToolBar() {}

void VmapToolBar::setupUi()
{
    m_showGridAct= new QAction(QIcon(":/resources/icons/grid.png"), tr("Show Grid"), this);
    m_showGridAct->setToolTip(tr("Show/Hide Grid"));
    m_showGridAct->setCheckable(true);

    m_gridAbove= new QCheckBox(tr("Grid Above"), this);
    m_gridAbove->setToolTip(tr("Grid Above"));

    m_bgSelector= new ColorButton();

    m_currentPermission= new QComboBox();
    QStringList list;
    list << tr("GM Only") << tr("PC Move") << tr("ALL");
    m_currentPermission->addItems(list);

    m_currentLayer= new QComboBox();
    QStringList listLayer;
    listLayer << tr("Ground") << tr("Object") << tr("Character");
    m_currentLayer->addItems(listLayer);

    m_gridPattern= new QComboBox();
    QStringList listGrid;
    listGrid << tr("None") << tr("Square") << tr("Hexagon");
    m_gridPattern->addItems(listGrid);

    m_gridUnit= new QComboBox();
    QStringList listUnit;
    listUnit << QStringLiteral("m") << QStringLiteral("km") << QStringLiteral("cm") << QStringLiteral("mi")
             << QStringLiteral("yd") << tr("inch") << tr("foot") << tr("px");
    m_gridUnit->addItems(listUnit);

    m_currentVisibility= new QComboBox();
    QStringList listVisi;
    listVisi << tr("Hidden") << tr("Fog of War") << tr("All");
    m_currentVisibility->addItems(listVisi);

    m_scaleSize= new QDoubleSpinBox(this);
    m_scaleSize->setRange(0.1, std::numeric_limits<double>::max());
    m_scaleSize->setValue(1.0);

    m_gridSize= new QSpinBox(this);
    m_gridSize->setRange(1, std::numeric_limits<int>::max());

    addWidget(new QLabel(tr("Background:")));
    addWidget(m_bgSelector);
    addSeparator();
    addWidget(new QLabel(tr("Grid:")));
    addAction(m_showGridAct);
    addWidget(m_gridAbove);
    addWidget(m_gridPattern);
    addWidget(m_gridSize);
    addWidget(new QLabel(tr("px :")));
    addWidget(m_scaleSize);
    addWidget(m_gridUnit);
    addSeparator();
    addWidget(new QLabel(tr("Permission:")));
    addWidget(m_currentPermission);
    addSeparator();
    addWidget(new QLabel(tr("Visibility:")));
    addWidget(m_currentVisibility);
    addWidget(new QLabel(tr("Layer:")));
    addWidget(m_currentLayer);

    m_showOnlyItemsFromThisLayer= new QCheckBox(tr("Hide other Layers"));
    addWidget(m_showOnlyItemsFromThisLayer);

    // m_showCharacterVision= new QCheckBox(tr("Character Vision"));
    m_showCharacterVision= new QToolButton(this);
    m_showCharacterVision->setToolTip(tr("Character Vision"));
    m_showCharacterVision->setCheckable(true);
    m_showCharacterVision->setIcon(QIcon(":/resources/images/sight.svg"));
    addWidget(m_showCharacterVision);
    // :/resources/images/sight.svg
    m_collision= new QCheckBox(tr("Collision"));
    addWidget(m_collision);

    m_showTransparentItem= new QAction(tr("Show transparent Item"), this);
    addAction(m_showTransparentItem);

    connect(m_showOnlyItemsFromThisLayer, SIGNAL(clicked(bool)), this, SLOT(managedAction()));
    connect(m_showGridAct, SIGNAL(triggered()), this, SLOT(triggerGrid()));
    connect(m_gridUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), m_ctrl,
            [this](int value) { m_ctrl->setGridUnit(static_cast<Core::ScaleUnit>(value)); });
    connect(m_bgSelector, &ColorButton::colorChanged, m_ctrl, &VectorialMapMediaController::setBackgroundColor);
    connect(m_gridSize, QOverload<int>::of(&QSpinBox::valueChanged), m_ctrl, &VectorialMapMediaController::setGridSize);
    connect(m_scaleSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_ctrl,
            &VectorialMapMediaController::setGridScale);

    connect(m_currentVisibility, QOverload<int>::of(&QComboBox::currentIndexChanged), m_ctrl,
            [this](int value) { m_ctrl->setVisibilityMode(static_cast<Core::VisibilityMode>(value)); });
    connect(m_currentPermission, QOverload<int>::of(&QComboBox::currentIndexChanged), m_ctrl,
            [this](int value) { m_ctrl->setPermissionMode(static_cast<Core::PermissionMode>(value)); });
    connect(m_currentLayer, QOverload<int>::of(&QComboBox::currentIndexChanged), m_ctrl,
            [this](int value) { m_ctrl->setLayer(static_cast<Core::Layer>(value)); });

    connect(m_gridPattern, QOverload<int>::of(&QComboBox::currentIndexChanged), m_ctrl,
            [this](int value) { m_ctrl->setGridPattern(static_cast<Core::GridPattern>(value)); });

    connect(m_showGridAct, &QAction::triggered, m_ctrl, &VectorialMapMediaController::setGridVisibility);
    connect(m_gridAbove, &QCheckBox::toggled, m_ctrl, &VectorialMapMediaController::setGridAbove);

    connect(m_showCharacterVision, &QToolButton::clicked, m_ctrl, &VectorialMapMediaController::setCharacterVision);
    connect(m_collision, &QCheckBox::clicked, m_ctrl, &VectorialMapMediaController::setCollision);
    connect(m_showTransparentItem, &QAction::triggered, m_ctrl, &VectorialMapMediaController::showTransparentItem);

    // Update Ui
    connect(m_ctrl, &VectorialMapMediaController::gridSizeChanged, m_gridSize, &QSpinBox::setValue);
    connect(m_ctrl, &VectorialMapMediaController::gridUnitChanged, this,
            [this](Core::ScaleUnit unit) { m_gridUnit->setCurrentIndex(static_cast<int>(unit)); });

    connect(m_ctrl, &VectorialMapMediaController::gridAboveChanged, m_gridAbove, &QCheckBox::setChecked);
    // connect(m_ctrl, &VectorialMapMediaController::gridColorChanged, this, [this](int) {
    /// TODO
    //});
    connect(m_ctrl, &VectorialMapMediaController::gridScaleChanged, m_scaleSize, &QDoubleSpinBox::setValue);

    connect(m_ctrl, &VectorialMapMediaController::gridVisibilityChanged, m_showGridAct, &QAction::setChecked);
    connect(m_ctrl, &VectorialMapMediaController::backgroundColorChanged, m_bgSelector, &ColorButton::setColor);

    connect(m_ctrl, &VectorialMapMediaController::layerChanged, this,
            [this](Core::Layer layer) { m_currentLayer->setCurrentIndex(static_cast<int>(layer)); });
    connect(m_ctrl, &VectorialMapMediaController::collisionChanged, m_collision, &QCheckBox::setChecked);
    connect(m_ctrl, &VectorialMapMediaController::characterVision, m_showCharacterVision, &QCheckBox::setChecked);
    connect(m_ctrl, &VectorialMapMediaController::visibilityModeChanged, this,
            [this](Core::VisibilityMode mode) { m_currentVisibility->setCurrentIndex(static_cast<int>(mode)); });
    connect(m_ctrl, &VectorialMapMediaController::permissionModeChanged, this,
            [this](Core::PermissionMode mode) { m_currentPermission->setCurrentIndex(static_cast<int>(mode)); });
}
void VmapToolBar::setCurrentMap(VMap* map)
{
    /*  if(m_vmap != nullptr)
          disconnect(m_vmap);

      m_vmap= map;

      if(m_vmap)
      {
          connect(m_vmap, &VMap::mapChanged, this, &VmapToolBar::updateUI);
      }*/
    updateUI();
}
void VmapToolBar::triggerGrid()
{
    // if(nullptr != m_vmap)
    {
        // m_vmap->setOption(VisualItem::ShowGrid, m_showGridAct->isChecked());
    }
}
void VmapToolBar::setUnit()
{
    // if(nullptr != m_vmap)
    {
        //  m_vmap->setOption(VisualItem::Unit, m_gridUnit->currentIndex());
    }
}

void VmapToolBar::patternChanged(int index)
{
    // if(nullptr != m_vmap)
    {
        //  m_vmap->setOption(VisualItem::GridPattern, index);
    }
}

void VmapToolBar::visibilityHasChanged(int index)
{
    // if(nullptr != m_vmap)
    {
        // m_vmap->setVisibilityMode(static_cast<Core::VisibilityMode>(index));
    }
}
void VmapToolBar::permissionHasChanged(int index)
{
    //  if(nullptr != m_vmap)
    {
        //  m_vmap->setPermissionMode(static_cast<Core::PermissionMode>(index));
    }
}
void VmapToolBar::layerHasChanged(int index)
{
    // if(nullptr != m_vmap)
    {
        //  m_vmap->setCurrentLayer(static_cast<Core::Layer>(index));
    }
}
void VmapToolBar::updateUI()
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
void VmapToolBar::setPatternSize(int p)
{
    //  if(nullptr != m_vmap)
    {
        // m_vmap->setOption(VisualItem::GridSize, p);
    }
}
void VmapToolBar::setScaleSize(double p)
{
    //  if(nullptr != m_vmap)
    {
        // m_vmap->setOption(VisualItem::Scale, p);
    }
}

void VmapToolBar::managedAction()
{
    //  if(nullptr == m_vmap)
    {
        return;
    }
    QObject* obj= sender();
    if(obj == m_showCharacterVision)
    {
        // m_vmap->setOption(VisualItem::EnableCharacterVision, m_showCharacterVision->isChecked());
    }
    else if(obj == m_collision)
    {
        // m_vmap->setOption(VisualItem::CollisionStatus, m_collision->isChecked());
    }
    else if(obj == m_showOnlyItemsFromThisLayer)
    {
        // m_vmap->setOption(VisualItem::HideOtherLayers, m_showOnlyItemsFromThisLayer->isChecked());
    }
    else if(obj == m_showTransparentItem)
    {
        //  m_vmap->showTransparentItems();
    }
}
