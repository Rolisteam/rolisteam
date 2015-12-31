/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
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

    m_bgSelector =new ColorButton();

    m_currentPermission =new QComboBox();
    QStringList list;
    list << tr("GM Only") <<tr("PC Move") <<tr("ALL") ;
    m_currentPermission->addItems(list);


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

    addWidget(new QLabel(tr("Background:")));
    addWidget(m_bgSelector);
    addSeparator();
    addWidget(new QLabel(tr("Grid:")));
    addAction(m_showGridAct);
    addWidget(m_gridPattern);
    addWidget(m_gridUnit);
    m_gridSize = new QSpinBox(this);
    m_gridSize->setRange(1,std::numeric_limits<int>::max());
    addWidget(m_gridSize);
    addSeparator();
    addWidget(new QLabel(tr("Permission:")));
    addWidget(m_currentPermission);
    addSeparator();
    addWidget(new QLabel(tr("Visibility:")));
    addWidget(m_currentVisibility);
    m_showCharacterVision = new QCheckBox(tr("Enable Character Vision"));
    addWidget(m_showCharacterVision);




    connect(m_showGridAct,SIGNAL(triggered()),this,SLOT(triggerGrid()));
    connect(m_bgSelector,SIGNAL(colorChanged(QColor)),this,SLOT(setBackgroundColor(QColor)));
    connect(m_gridSize,SIGNAL(valueChanged(int)),this,SLOT(setPatternSize(int)));

    connect(m_currentVisibility,SIGNAL(currentIndexChanged(int)),this,SLOT(visibilityHasChanged(int)));
    connect(m_currentPermission,SIGNAL(currentIndexChanged(int)),this,SLOT(permissionHasChanged(int)));
    connect(m_gridPattern,SIGNAL(currentIndexChanged(int)),this,SLOT(patternChanged(int)));
    connect(m_showCharacterVision,SIGNAL(clicked(bool)),this,SLOT(managedAction()));

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
void VmapToolBar::updateUI()
{
    if(NULL!=m_vmap)
    {
        setEnabled(true);
        m_bgSelector->setColor(m_vmap->getBackGroundColor());
        m_showGridAct->setChecked(m_vmap->getOption(VisualItem::ShowGrid).toBool());
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
}
