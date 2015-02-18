/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "colorselector.h"
#include "diameterselector.h"


class ToolsBar : public QDockWidget
{
Q_OBJECT

public :
    static ToolsBar* getInstance(QWidget *parent=0);
    void majCouleursPersonnelles();
    QColor donnerCouleurPersonnelle(int numero);
    void autoriserOuInterdireCouleurs();


    QColor& currentColor();

    // Selectable tools
    enum SelectableTool {PEN, LINE, EMPTYRECT, FILLRECT, EMPTYELLIPSE, FILLEDELLIPSE, TEXT, HANDLER, ADDNPC, DELNPC, MOVECHARACTER, STATECHARACTER};

public slots :
    void incrementeNumeroPnj();
    void changeCurrentColor(QColor color);
    void mettreAJourPnj(int diametre, QString nom);


signals:
    void currentToolChanged(ToolsBar::SelectableTool);
    void currentColorChanged(QColor&);
    void currentPenSizeChanged(int);
    void currentPNCSizeChanged(int);
    void currentModeChanged(int);

private:

    ToolsBar(QWidget *parent = 0);
    static ToolsBar* m_sigleton;
    void creerActions();
    void creerOutils();

    QWidget* m_centralWidget;
    QLineEdit *m_textEditLine;
    QLineEdit *m_npcNameTextEdit;
    QLCDNumber *m_displayNPCCounter;
    ColorSelector *m_colorSelector;
    DiameterSelector *m_lineDiameter;
    DiameterSelector *m_NpcDiameter;
    QActionGroup *m_toolsGroup;
    SelectableTool m_currentTool;

    QAction *m_pencilAct;
    QAction *m_lineAct;
    QAction *m_rectAct;
    QAction *m_rectFillAct;
    QAction *m_elipseAct;
    QAction *m_elipseFillAct;
    QAction *m_textAct;
    QAction *m_handAct;
    QAction *m_addPCAct;
    QAction *m_delNPCAct;
    QAction *m_movePCAct;
    QAction *m_pcStateAct;
    QAction *m_resetCountAct;

 /*   QAction* m_eraseAction;
    QAction* m_hideAction;
    QAction* m_unveilAction;*/



    QString m_currentNPCName;
    int m_currentNPCNumber;

private slots :
    void razNumeroPnj();
    void changementTaille(bool floating);
    void texteChange(const QString &texte);
    void nomPnjChange(const QString &texte);



    void currentActionChanged(QAction* p);

};

#endif
