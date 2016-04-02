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

#ifndef VMAPTOOLBAR_H
#define VMAPTOOLBAR_H

#include <QToolBar>


#include "vmap.h"
#include "widgets/colorbutton.h"

/**
 * @brief The VmapToolBar class
 */
class VmapToolBar : public QToolBar
{
    Q_OBJECT
public:
    /**
     * @brief VmapToolBar
     */
    VmapToolBar();
    /**
      */
    virtual ~VmapToolBar();
    /**
     * @brief setupUi
     */
    void setupUi();
    /**
     * @brief updateUI
     */
    void updateUI();


public slots:
    /**
     * @brief setCurrentMap
     * @param map
     */
    void setCurrentMap(VMap* map);
    /**
     * @brief triggerGrid
     */
    void triggerGrid();
    /**
     * @brief setBackgroundColor
     */
    void setBackgroundColor(QColor);
    void visibilityHasChanged(int);
    void permissionHasChanged(int index);
    void patternChanged(int);
    void setPatternSize(int);
    void setScaleSize(int);
    void managedAction();

    void layerHasChanged(int index);
private:
    /**
     * @brief updateUi
     */
    void updateUi();


private:
    VMap* m_vmap;

    //Button
    ColorButton* m_bgSelector;
    QAction* m_showGridAct;
    QAction* m_Act;

    QComboBox* m_gridPattern;
    QComboBox* m_gridUnit;
    QComboBox* m_currentLayer;
    QComboBox* m_currentPermission;
    QComboBox* m_currentVisibility;
    QSpinBox* m_gridSize;
    QSpinBox* m_scaleSize;
    QCheckBox* m_showCharacterVision;
};

#endif // VMAPTOOLBAR_H
