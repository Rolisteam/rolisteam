/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
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

#include "vmaptoolbar.h"

VmapToolBar::VmapToolBar()
    : m_vmap(NULL)
{
    setObjectName("VMapToolBar");
    setWindowTitle(tr("Toolbar for VMap"));
    setupUi();
}

VmapToolBar::~VmapToolBar()
{

}

void VmapToolBar::setupUi()
{
    m_showGridAct = new QAction(QIcon(":/resources/icons/grid.png"),tr("Show Grid"),this);
    m_showGridAct->setToolTip(tr("Show/Hide Grid"));
    m_showGridAct->setCheckable(true);

    m_gridAbove = new QCheckBox(tr("Grid Above"),this);
    m_gridAbove->setToolTip(tr("Grid Above"));

    m_bgSelector =new ColorButton();

    m_currentPermission =new QComboBox();
    QStringList list;
    list << tr("GM Only") <<tr("PC Move") <<tr("ALL") ;
    m_currentPermission->addItems(list);

    m_currentLayer = new QComboBox();
    QStringList listLayer;
    listLayer << tr("Ground") <<tr("Object") <<tr("Character");
    m_currentLayer->addItems(listLayer);


    m_gridPattern =new QComboBox();
    QStringList listGrid;
    listGrid << tr("None") <<tr("Square") <<tr("Hexagon") ;
    m_gridPattern->addItems(listGrid);

    m_gridUnit=new QComboBox();
    QStringList listUnit;
    listUnit << tr("m") <<tr("km") <<tr("cm")<<tr("mile")<<tr("yard")<<tr("inch")<<tr("foot") <<tr("px");
    m_gridUnit->addItems(listUnit);

    m_currentVisibility=new QComboBox();
    QStringList listVisi;
    listVisi <<tr("Hidden") <<tr("Fog of War")<< tr("All");
    m_currentVisibility->addItems(listVisi);


    m_scaleSize = new QSpinBox(this);
    m_scaleSize->setRange(1,std::numeric_limits<int>::max());

    m_gridSize = new QSpinBox(this);
    m_gridSize->setRange(1,std::numeric_limits<int>::max());

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

    m_showOnlyItemsFromThisLayer = new QCheckBox(tr("Hide other Layers"));
    addWidget(m_showOnlyItemsFromThisLayer);

    m_showCharacterVision = new QCheckBox(tr("Character Vision"));
    addWidget(m_showCharacterVision);

    m_collision = new QCheckBox(tr("Collision"));
    addWidget(m_collision);


    m_showTransparentItem = new QAction(tr("Show transparent Item"),this);
    addAction(m_showTransparentItem);


    connect(m_showOnlyItemsFromThisLayer,SIGNAL(clicked(bool)),this,SLOT(managedAction()));
    connect(m_showGridAct,SIGNAL(triggered()),this,SLOT(triggerGrid()));
    connect(m_gridAbove,&QCheckBox::toggled,[=](bool b){
        if(nullptr!=m_vmap)
        {
            m_vmap->setOption(VisualItem::GridAbove,b);
        }
    });
    connect(m_gridUnit,SIGNAL(currentIndexChanged(int)),this,SLOT(setUnit()));
    connect(m_bgSelector,SIGNAL(colorChanged(QColor)),this,SLOT(setBackgroundColor(QColor)));
    connect(m_gridSize,SIGNAL(valueChanged(int)),this,SLOT(setPatternSize(int)));
    connect(m_scaleSize,SIGNAL(valueChanged(int)),this,SLOT(setScaleSize(int)));

    connect(m_currentVisibility,SIGNAL(currentIndexChanged(int)),this,SLOT(visibilityHasChanged(int)));
    connect(m_currentPermission,SIGNAL(currentIndexChanged(int)),this,SLOT(permissionHasChanged(int)));
    connect(m_currentLayer,SIGNAL(currentIndexChanged(int)),this,SLOT(layerHasChanged(int)));

    connect(m_gridPattern,SIGNAL(currentIndexChanged(int)),this,SLOT(patternChanged(int)));
    connect(m_showCharacterVision,SIGNAL(clicked(bool)),this,SLOT(managedAction()));
    connect(m_collision,SIGNAL(clicked(bool)),this,SLOT(managedAction()));
    connect(m_showTransparentItem,SIGNAL(triggered()),this,SLOT(managedAction()));

}
void VmapToolBar::setCurrentMap(VMap* map)
{
    m_vmap = map;
    updateUI();
}
void VmapToolBar::triggerGrid()
{
    if(NULL!=m_vmap)
    {
        m_vmap->setOption(VisualItem::ShowGrid,m_showGridAct->isChecked());
    }
}
void VmapToolBar::setUnit()
{
    if(NULL!=m_vmap)
    {
        m_vmap->setOption(VisualItem::Unit,m_gridUnit->currentIndex());
    }
}
void VmapToolBar::setBackgroundColor(QColor color)
{
    if(NULL!=m_vmap)
    {
        m_vmap->setBackGroundColor(color);
    }
}
void VmapToolBar::patternChanged(int index)
{
    if(NULL!=m_vmap)
    {
        m_vmap->setOption(VisualItem::GridPattern,index);
    }
}

void VmapToolBar::visibilityHasChanged(int index)
{
    if(NULL!=m_vmap)
    {
        m_vmap->setVisibilityMode((VMap::VisibilityMode)index);
    }
}
void VmapToolBar::permissionHasChanged(int index)
{
    if(NULL!=m_vmap)
    {
        m_vmap->setPermissionMode((Map::PermissionMode)index);
    }
}
void VmapToolBar::layerHasChanged(int index)
{
    if(NULL!=m_vmap)
    {
        m_vmap->setCurrentLayer((VisualItem::Layer)index);
    }
}
void VmapToolBar::updateUI()
{
    if(NULL!=m_vmap)
    {
        setEnabled(true);
        m_bgSelector->setColor(m_vmap->getBackGroundColor());
        m_showGridAct->setChecked(m_vmap->getOption(VisualItem::ShowGrid).toBool());
        m_gridSize->setValue(m_vmap->getOption(VisualItem::GridSize).toInt());
        m_currentLayer->setCurrentIndex(m_vmap->getCurrentLayer());
        m_showCharacterVision->setChecked(m_vmap->getOption(VisualItem::EnableCharacterVision).toBool());
        m_gridUnit->setCurrentIndex(m_vmap->getOption(VisualItem::Unit).toInt());
        m_currentPermission->setCurrentIndex(m_vmap->getOption(VisualItem::PermissionMode).toInt());
        m_currentVisibility->setCurrentIndex(m_vmap->getVisibilityMode());
        m_gridPattern->setCurrentIndex(m_vmap->getOption(VisualItem::GridPattern).toInt());
        m_collision->setChecked(m_vmap->getOption(VisualItem::CollisionStatus).toBool());
        m_showOnlyItemsFromThisLayer->setChecked(m_vmap->getOption(VisualItem::HideOtherLayers).toBool());
    }
    else
    {
        setEnabled(false);
    }

}
void VmapToolBar::setPatternSize(int p)
{
    if(NULL!=m_vmap)
    {
        m_vmap->setOption(VisualItem::GridSize,p);
    }
}
void  VmapToolBar::setScaleSize(int p)
{
    if(NULL!=m_vmap)
    {
        m_vmap->setOption(VisualItem::Scale,p);
    }
}

void VmapToolBar::managedAction()
{
    if(NULL==m_vmap)
    {
        return;
    }
    QObject* obj = sender();
    if(obj==m_showCharacterVision)
    {
        m_vmap->setOption(VisualItem::EnableCharacterVision,m_showCharacterVision->isChecked());
    }
    else if(obj == m_collision)
    {
        m_vmap->setOption(VisualItem::CollisionStatus,m_collision->isChecked());
    }
    else if(obj == m_showOnlyItemsFromThisLayer)
    {
        m_vmap->setOption(VisualItem::HideOtherLayers,m_showOnlyItemsFromThisLayer->isChecked());
    }
    else if(obj == m_showTransparentItem)
    {
        m_vmap->showTransparentItems();
    }
}
