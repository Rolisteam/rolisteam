/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef CHARACTERSHEETWINDOW_H
#define CHARACTERSHEETWINDOW_H
#include <QTreeView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "headerview.h"
#include "submdiwindows.h"
#include "charactersheetmodel.h"
/**
* @brief herits from SubMdiWindows. It displays and manages all classes required to deal with the character sheet MVC architrecture.
*/
class CharacterSheetWindow : public SubMdiWindows
{
    Q_OBJECT
public:
	/**
	* @brief default construtor
	*/
    CharacterSheetWindow(QWidget* parent = NULL);

	/**
	* @brief compulsary function , it should set the custom window menu
	* @param pointer to the menu of custom window
	* @return bool : true if the characterSheetWindow has amend the menu, false if the custom menu is not required
	*/
    virtual bool defineMenu(QMenu* menu);
	/**
	* @brief cumpulsary function, it returns the subwindows type.
	* @return type of subwindows. 
	*/
    virtual SubWindowType getType();


protected slots:
	/**
	* @brief slot is called when the user click on the m_addLine button. That leads to add one line after the current position (in the current CharacterSheet).
	*/
    void addLine();
	/**
	* @brief slot is called when the user click on the m_addSection button. That leads to add a section after the current section (in the current CharacterSheet).
	*/
    void addSection();
	/**
	* @brief slot is called when the user click on the m_addCharacterSheet button. That Leads to add an empty characterSheet in the model (after all others).
	*/
    void addCharacterSheet();
    /**
    * @brief slot is called when the user click on the  mice right button
    */
    void displayCustomMenu(const QPoint & pos);

    void saveCharacterSheet();
    void openCharacterSheet();

protected:
    virtual void closeEvent ( QCloseEvent * event );
private:
	/**
	* @brief The view class
	*/
    QTreeView m_view;
	/**
	* @brief the model for the characterSheet view
	*/
    CharacterSheetModel m_model;


	/**
	* @brief event button
	* @todo It should be a better idea to use QAction instead of Button and profide a contextual menu or a custom menu instead of these awful buttons.
	*/
    QAction* m_addSection;
	/**
	* @brief event button
	*/
    QAction* m_addLine;
	/**
	* @brief event button
	*/
    QAction* m_addCharacterSheet;
    /**
    * @brief event button
    */
    QAction* m_saveCharacterSheet;
    /**
    * @brief event button
    */
    QAction* m_openCharacterSheet;



	/**
	* @brief layout
	*/
    QVBoxLayout m_vertiLayout;
	/**
	* @brief layout
	*/
    QHBoxLayout m_horizonLayout;
	/**
	* @brief gathering widget
	*/
    QWidget m_widget;

    QString m_fileUri;


};

#endif // CHARACTERSHEETWINDOW_H
