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


/********************************************************************/
/*                                                                  */
/* Editeur de notes permettant aux utilisateurs de prendre des      */
/* notes en partie                                                  */
/*                                                                  */
/********************************************************************/	


#ifndef MINUTES_EDITOR_H
#define MINUTES_EDITOR_H

#include <QWidget>
#include <QMainWindow>
#include <QTextEdit>
#include <QCloseEvent>
#include <QToolBar>
#include <QComboBox>
#include <QFile>
#include "submdiwindows.h"

/**
* @brief MinutesEditor is dedicated to be a simple text processor. It provides some helpful features, such as Font style, and save/load mechanism.
* @todo we should improve MinutesEditor to support openDocument file.  
*/
class QFontComboBox;
class MinutesEditor : public SubMdiWindows
{
Q_OBJECT

public :
	/**
	* @brief the default constructor
	*/
    MinutesEditor();
	/**
	* @brief writes all contents of the instance into a file. 
	* @param file which must be writen.
	* @todo perhaps it should be useful to override operator << and >> for QDataStream and QTextStream?
	*/
    void saveMinutes(QFile &file);
	/**
	* @brief read data from a file and set the appropriate member.
	* @param file which must be read.
	*/
    void openMinutes(QFile &file);
	/**
	* @brief allows to determine how the window must be resized.
	*/
    QSize sizeHint() const;
	/**
	* @brief allows to amend the custom menu.
	* @param menu ; pointeur to the custom menu.
	*/

    virtual bool defineMenu(QMenu* menu);



private slots :
	/**
	* @brief is called when the user has clicked on normal style button.
	*/
    void normalStyle();
	/**
	* @brief is called when the user has clicked on bold style button
	*/
    void boldStyle();
	/**
	* @brief is called when the user has clicked on italic style button.
	*/
    void italicStyle();
	/**
	* @brief is called when the user has clicked on underline style
	*/
    void underlineStyle();
	/**
	* @brief force the size update
	*/
    void updateSize();
	/**
	* @brief is called when the user has changed the size.
	* @param the new size 
	*/
    void changeSize(int index);



    /*void drawDoc();
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *a);
    void onRead( int now ,int tot );
    void currentCharFormatChanged(const QTextCharFormat &format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void printPreview(QPrinter *);*/




protected:
	/**
	* @brief handle the close event to make some backup (useless at the moment).
	*/
    void closeEvent(QCloseEvent *event);

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    QAction* m_italicAct;
    QAction* m_boldAct;
    QAction* m_underLineAct;

    QAction* m_centerAct;
    QAction* m_leftAct;
    QAction* m_rightAct;
    QAction* m_justifyAct;

    QComboBox* m_styleSelector;
    QFontComboBox* m_fontSelector;
        /**
        * @brief pointeur to the texteditor
        */
    QTextEdit* m_minutes;
        /**
        * @brief pointeur to the fontstyle toolbar
        */
    QToolBar* m_styleBar;
        /**
        * @brief allows to change font size.
        */
    QComboBox* m_sizeSelector;

};

#endif
