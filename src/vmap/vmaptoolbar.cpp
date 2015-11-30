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
    m_showGridAct = addAction(QIcon(":/resources/icons/grid.png"),tr("Show Grid"));
    m_showGridAct->setToolTip(tr("Show/Hide Grid"));
    m_showGridAct->setCheckable(true);

    m_bgSelector =new ColorButton();
    addWidget(m_bgSelector);

    m_currentPermission =new QComboBox();
    QStringList list;
    list << tr("GM Only") <<tr("PC Move") <<tr("ALL") ;
    m_currentPermission->addItems(list);
    addWidget(m_currentPermission);


    m_gridPattern =new QComboBox();
    QStringList listGrid;
    listGrid << tr("None") <<tr("Square") <<tr("Hexagon") ;
    m_gridPattern->addItems(listGrid);
    addWidget(m_gridPattern);

    connect(m_showGridAct,SIGNAL(triggered()),this,SLOT(triggerGrid()));
    connect(m_bgSelector,SIGNAL(colorChanged(QColor)),this,SLOT(setBackgroundColor(QColor)));
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
void VmapToolBar::updateUI()
{
    m_bgSelector->setColor(m_vmap->getBackGroundColor());
    m_showGridAct->setChecked(m_vmap);
}
