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

#ifndef VMAPTOOLBAR_H
#define VMAPTOOLBAR_H

#include <QDoubleSpinBox>
#include <QToolBar>

#include "common/widgets/colorbutton.h"
#include "vmap.h"

class VectorialMapController;
/**
 * @brief The VmapToolBar class
 */
class VmapToolBar : public QToolBar
{
    Q_OBJECT
public:
    VmapToolBar(VectorialMapController* ctrl, QWidget* parent= nullptr);
    virtual ~VmapToolBar();

    void setupUi();
    void updateUI();

private:
    void initActions();
    void updateUi();

private:
    QPointer<VectorialMapController> m_ctrl;

    // Button
    ColorButton* m_bgSelector;
    QAction* m_showSquareAct= nullptr;
    QAction* m_showHexagonAct= nullptr;
    QAction* m_gridAboveAct= nullptr;

    QAction* m_onlyGmPermAct= nullptr;
    QAction* m_characterOnlyPermAct= nullptr;
    QAction* m_allPermAct= nullptr;

    QAction* m_hiddenAct= nullptr;
    QAction* m_fogAct= nullptr;
    QAction* m_allAct= nullptr;

    QAction* m_groundAct= nullptr;
    QAction* m_objectAct= nullptr;
    QAction* m_characterAct= nullptr;
    QAction* m_hideOtherAct= nullptr;

    QAction* m_characterVisionAct= nullptr;
    QAction* m_showTransparentAct= nullptr;

    QAction* m_showPcName= nullptr;
    QAction* m_showNpcName= nullptr;
    QAction* m_showNpcNumber= nullptr;
    QAction* m_showState= nullptr;
    QAction* m_showHealthBar= nullptr;
    QAction* m_showInit= nullptr;

    QSpinBox* m_gridSize= nullptr;
    QDoubleSpinBox* m_scaleSize= nullptr;
    QComboBox* m_gridUnit= nullptr;
};

#endif // VMAPTOOLBAR_H
