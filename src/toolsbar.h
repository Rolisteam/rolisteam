/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *      http://www.rolisteam.org/                                        *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


/********************************************************************/
/* DockWidget contenant la barre d'outils.                          */
/********************************************************************/


#ifndef BARRE_OUTILS_H
#define BARRE_OUTILS_H

#include <QAction>
#include <QLineEdit>
#include <QLCDNumber>
#include <QDockWidget>
#include <QActionGroup>

class ColorSelector;
class DiameterSelector;
class Map;
/**
 * @brief The ToolBar class is gathering all tool and manages selection of them.
 */
class ToolsBar : public QDockWidget
{
    Q_OBJECT
    
public :
	ToolsBar(QWidget *parent = 0);
	virtual ~ToolsBar();
    void majCouleursPersonnelles();
    QColor donnerCouleurPersonnelle(int numero);
    void updateUi();
    int getCurrentNpcNumber() const;


    // Outils selectionnables par l'utilisateur
	enum SelectableTool {Pen, Line, EmptyRect, FilledRect, EmptyEllipse, FilledEllipse, Text, Handler, AddNpc, DelNpc, MoveCharacterToken, ChangeCharacterState};

	ToolsBar::SelectableTool getCurrentTool() const;
signals:
	/**
	* @brief emited when current tool has been changed by user
	*/
	void currentToolChanged(ToolsBar::SelectableTool);
	/**
	* @brief emitted when current color has been changed by user
	*/
	void currentColorChanged(QColor&);
	/**
	* @brief emitted when user has changed the pen size.
	*/
	void currentPenSizeChanged(int);
	/**
	* @brief emitted when current NPC size has changed
	*/
	void currentNpcSizeChanged(int);
	/**
	* @brief emitted when current mode has changed
	*/
	void currentModeChanged(int);
    /**
     * @brief currentTextChanged
     */
	void currentTextChanged(QString);
    /**
     * @brief currentNpcNameChanged
     */
	void currentNpcNameChanged(QString);

    void currentNpcNumberChanged(int);

public slots :
	void incrementNpcNumber();
	void changeCurrentColor(QColor coul);
    void mettreAJourPnj(int diametre, QString nom);
    void changeMap(Map * map);

    
private :
    void creerActions();
    void creerOutils();

    QWidget *outils;
	QLineEdit *m_textEdit;
	QLineEdit *m_npcNameEdit;
    QLCDNumber *afficheNumeroPnj;
    ColorSelector* m_color;
	DiameterSelector *m_lineDiameter;
    DiameterSelector *m_npcDiameter;
    Map * m_map;
    //DiameterSelector* m_pcDiameter;


private slots :
    void razNumeroPnj();
	void changeSize(bool floating);


 /*   void crayonSelectionne();
    void ligneSelectionne();
    void rectVideSelectionne();
    void rectPleinSelectionne();
    void elliVideSelectionne();
    void elliPleinSelectionne();
    void texteSelectionne();
    void mainSelectionne();
    void ajoutPnjSelectionne();
    void supprPnjSelectionne();
    void deplacePersoSelectionne();
	void etatPersoSelectionne();*/

    void currentToolHasChanged(QAction*);

//    void changeCharacterSize(int size);
//    void sendNewCharacterSize(int size);


private:
	QString m_currentNPCName;
	QString m_currentText;

	int m_currentNpcNumber;
    QActionGroup* m_actionGroup;
	ToolsBar::SelectableTool m_currentTool;
	QAction* m_pencilAct;
	QAction* m_lineAct;
	QAction* m_rectAct;
	QAction* m_filledRectAct;
	QAction* m_ellipseAct;
	QAction* m_filledEllipseAct;
	QAction* m_textAct;
	QAction* m_handAct;
	QAction* m_addNpcAct;
	QAction* m_delNpcAct;
	QAction* m_moveCharacterAct;
	QAction* m_changeCharacterState;
	QAction* m_resetCountAct;
};

#endif
