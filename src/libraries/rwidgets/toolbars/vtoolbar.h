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
#include "rwidgets/customs/diameterselector.h"
#include "rwidgets/customs/realslider.h"
#include "rwidgets/customs/vcolorselector.h"
#include "rwidgets/rwidgets_global.h"

class VectorialMapController;

class HiddingButton : public QToolButton
{
    Q_OBJECT
public:
    HiddingButton(QWidget* w);

    void addAction(QAction* act);
};

/**
 *  @brief toolbar is a QWidget subclass which gathering all tool required for drawing maps.
 */
class RWIDGET_EXPORT ToolBox : public QWidget
{
    Q_OBJECT

public:
    ToolBox(VectorialMapController* ctrl, QWidget* parent= nullptr);
    ~ToolBox();
    void setImage(const QPixmap& img);

private slots:
    void setupUi();
    void updateUi();

private:
    void createActions(); /// utility function
    void makeTools();     /// utility function

private:
    QPointer<VectorialMapController> m_ctrl;

    QWidget* m_centralWidget= nullptr;         /// address to the main widget
    QLineEdit* m_npcNameTextEdit= nullptr;     /// text line to define the npc name
    QLCDNumber* m_displayNPCCounter= nullptr;  /// count how many npc have been created
    VColorSelector* m_colorSelector= nullptr;  /// select a color
    DiameterSelector* m_lineDiameter= nullptr; /// select pen diameter
    QActionGroup* m_toolsGroup= nullptr;       /// group all tools and manage which one is the current one

    std::unique_ptr<RealSlider> m_zoomSlider;
    std::unique_ptr<QDoubleSpinBox> m_zoomSpinBox;
    std::unique_ptr<QLabel> m_smallScene;

    // paiting or fow edition
    QAction* m_paintingModeAct= nullptr;
    QAction* m_veilModeAct= nullptr;
    QAction* m_unveilModeAct= nullptr;

    // tools
    QAction* m_pencilAct= nullptr;
    QAction* m_lineAct= nullptr;
    QAction* m_rectAct= nullptr;
    QAction* m_rectFillAct= nullptr;
    QAction* m_elipseAct= nullptr;
    QAction* m_elipseFillAct= nullptr;
    QAction* m_textAct= nullptr;
    QAction* m_handAct= nullptr;
    QAction* m_addPCAct= nullptr;
    QAction* m_resetCountAct= nullptr;
    QAction* m_ruleAct= nullptr;
    QAction* m_pathAct= nullptr;
    QAction* m_anchorAct= nullptr;
    QAction* m_pipette= nullptr;
    QAction* m_bucketAct= nullptr;
    QAction* m_highlighterAct= nullptr;
    QAction* m_textWithBorderAct= nullptr;
    QComboBox* m_editionModeCombo= nullptr;
    QLabel* m_opacityLabel= nullptr;
    RealSlider* m_opacitySlider= nullptr;
};

#endif
