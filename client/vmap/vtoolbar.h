/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
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
#include <QLineEdit>
#include <QLCDNumber>
#include <QDockWidget>
#include <QResizeEvent>

#include "vcolorselector.h"
#include "widgets/diameterselector.h"
#include "widgets/realslider.h"
#include "map/map.h"
/**
  *  @brief toolbar is a QWidget subclass which gathering all tool required for drawing maps.
  */
class VToolsBar : public QWidget
{
Q_OBJECT

public :
    /**
      * @brief constructor for Qt widget
      */
    VToolsBar(QWidget *parent = 0);
    /**
      * @brief part of the singleton pattern
      */
    static VToolsBar* getInstance(QWidget* parent=0);
    /**
      * @brief accessor to the current color
      */
    QColor& currentColor();
    
    // Selectable tools
    /**
      * @brief tools id
      */
    enum SelectableTool {PEN, LINE, EMPTYRECT, FILLRECT, EMPTYELLIPSE, FILLEDELLIPSE, TEXT, HANDLER, ADDNPC,RULE,PATH,ANCHOR,TEXTBORDER,PIPETTE};
    /**
     * @brief The EditionMode enum
     */
    enum EditionMode {Painting,Mask,Unmask};

    /**
      * @brief accessor to the current tool
      */
    VToolsBar::SelectableTool getCurrentTool();
    /**
     * @brief getCurrentPenSize
     * @return
     */
    int getCurrentPenSize();

public slots :
    /**
     * @brief setCurrentTool
     */
    void setCurrentTool(VToolsBar::SelectableTool);
    /**
      * @brief increase NPC number
      * @todo rename it
      */
    void increaseNpcNumber();
    /**
      * @brief accessor to set the current color
      */
    void changeCurrentColor(QColor color);
    /**
     * @brief updateUi
     */
    void updateUi(Map::PermissionMode mode);
    /**
     * @brief setGM
     */
    void setGM(bool);
    /**
     * @brief setCurrentOpacity
     */
    void setCurrentOpacity(qreal);
    /**
     * @brief setCurrentColor
     */
    void setCurrentColor(QColor);
signals:
    /**
      * @brief emited when current tool has been changed by user
      */
    void currentToolChanged(VToolsBar::SelectableTool);
    /**
      * @brief emitted when current color has been changed by user
      */
    void currentColorChanged(QColor&);
    /**
      * @brief emitted when user has changed the pen size.
      */
    void currentPenSizeChanged(int);
    /**
      * @brief emitted when current mode has changed
      */
    void currentModeChanged(int);
    /**
     * @brief currentNpcNameChanged
     */
    void currentNpcNameChanged(QString);
    /**
     * @brief currentNpcNumberChanged
     */
    void currentNpcNumberChanged(int);
    /**
     * @brief currentEditionModeChanged
     */
    void currentEditionModeChanged(VToolsBar::EditionMode);

    void opacityChanged(qreal);


private slots :
    void resetNpcCount();
    void npcNameChange(const QString &texte);
    void currentActionChanged(QAction* p);
    void currentEditionModeChange();


private:
    static VToolsBar* m_sigleton;/// address of single instance
    void createActions(); /// utility function
    void makeTools(); /// utility function

    QWidget* m_centralWidget; /// address to the main widget
    QLineEdit* m_textEditLine; /// text line @warning is it still used ?
    QLineEdit* m_npcNameTextEdit;/// text line to define the npc name
    QLCDNumber* m_displayNPCCounter; /// count how many npc have been created
    VColorSelector* m_colorSelector; /// select a color
    DiameterSelector* m_lineDiameter;/// select pen diameter
    QActionGroup* m_toolsGroup;/// group all tools and manage which one is the current one
    SelectableTool m_currentTool; /// current tool
    EditionMode m_currentEditionMode;
    //paiting or fow edition
    QAction* m_paintingModeAct;
    QAction* m_veilModeAct;
    QAction* m_unveilModeAct;

    //tools
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
  //  QAction* m_unmaskPathAct;
    QAction* m_textWithBorderAct;
    QComboBox* m_editionModeCombo;
    RealSlider* m_opacitySlider;
    
    QString m_currentNPCName;
    int m_currentNPCNumber;

    bool m_isGM;
    

};
    
#endif
