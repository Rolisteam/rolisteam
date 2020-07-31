/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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

#ifndef TOOLS_BAR_H
#define TOOLS_BAR_H

#include <QAction>
#include <QDockWidget>
#include <QLCDNumber>
#include <QLineEdit>
#include <QPointer>
#include <QResizeEvent>

#include "media/mediatype.h"
#include "vcolorselector.h"
#include "widgets/diameterselector.h"
#include "widgets/realslider.h"

class VectorialMapController;
/**
 *  @brief toolbar is a QWidget subclass which gathering all tool required for drawing maps.
 */
class VToolsBar : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief constructor for Qt widget
     */
    VToolsBar(VectorialMapController* ctrl, QWidget* parent= nullptr);
    // Selectable tools
    /**
     * @brief tools id
     */

public slots:
    /**
     * @brief updateUi
     */
    void updateUi(Core::PermissionMode mode);
    /**
     * @brief setGM
     */
    void setGM(bool);

private slots:
    void setupUi();

private:
    void createActions(); /// utility function
    void makeTools();     /// utility function

private:
    QPointer<VectorialMapController> m_ctrl;

    QWidget* m_centralWidget;         /// address to the main widget
    QLineEdit* m_npcNameTextEdit;     /// text line to define the npc name
    QLCDNumber* m_displayNPCCounter;  /// count how many npc have been created
    VColorSelector* m_colorSelector;  /// select a color
    DiameterSelector* m_lineDiameter; /// select pen diameter
    QActionGroup* m_toolsGroup;       /// group all tools and manage which one is the current one
    // paiting or fow edition
    QAction* m_paintingModeAct;
    QAction* m_veilModeAct;
    QAction* m_unveilModeAct;

    // tools
    QAction* m_pencilAct;
    QAction* m_lineAct;
    QAction* m_rectAct;
    QAction* m_rectFillAct;
    QAction* m_elipseAct;
    QAction* m_elipseFillAct;
    QAction* m_textAct;
    QAction* m_handAct;
    QAction* m_addPCAct;
    QAction* m_resetCountAct;
    QAction* m_ruleAct;
    QAction* m_pathAct;
    QAction* m_anchorAct;
    QAction* m_pipette;
    QAction* m_bucketAct;
    QAction* m_highlighterAct;
    QAction* m_textWithBorderAct;
    QComboBox* m_editionModeCombo;
    RealSlider* m_opacitySlider;

    bool m_isGM;
};

#endif
