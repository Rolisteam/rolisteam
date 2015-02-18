/***************************************************************************
  *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
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
#ifndef SUBMDIWINDOWS_H
#define SUBMDIWINDOWS_H

#include <QMdiSubWindow>

#include "ToolBar.h"
#include "cleveruri.h"

class PreferencesManager;
class RClient;

/**
* @brief is an abstract class which defines interfaces with all kinds of subwindows.
*/
class SubMdiWindows : public QMdiSubWindow
{
    Q_OBJECT
public:
	/**
	* @brief default constructor
	*/
    SubMdiWindows(QWidget* parent=0 );
	/**
	* @brief enum which lists all type of subwindow
	*/
    //enum SubWindowType {MAP,TCHAT,PICTURE,TEXT,CHARACTERSHEET};


    virtual void saveFile(const QString & file)=0;
    virtual void openFile(const QString & file)=0;
    virtual void setRClient(RClient* t);

    virtual bool hasDockWidget() const =0;
    virtual QDockWidget* getDockWidget() =0;

public slots:
	/**
	* @brief is called when the window' state changes
	* @param the previous state.
	* @param the new state.
	*/
    void changedStatus(Qt::WindowStates oldState,Qt::WindowStates newState);
	/**
	* @brief is called when the selected tool changes.
	* @param the current tool.
	*/
    virtual void currentToolChanged(ToolsBar::SelectableTool);
	/**
	* @brief is called when the cursor must be changed. 
	* @param the new cursor.
	* @todo gathering this function and currentToolChanged should be a better choice.
	*/
    virtual void currentCursorChanged(QCursor*);
	/**
	* @brief is called when the user has changed the selected color.
	* @param the new color.
	*/
    virtual void currentColorChanged(QColor&);

	/**
	* @brief is called when the user has changed the pen size.
	* @param the new pen size.
	*/
    virtual void currentPenSizeChanged(int);

	/**
	* @brief is called when user has changed the NPC size.
	* @param the new size.
	*/
    virtual void currentNPCSizeChanged(int);
	/**
	* @brief virtual function which allows controlor class to know the type of the current instance.
	*/
    virtual CleverURI::ContentType getType();

	/**
	* @brief must be overriden in subclasses. It allows to define the custom menu
	* @param the new size.
	*/
    virtual bool defineMenu(QMenu* menu)=0;

    virtual void setCleverURI(CleverURI* uri);

protected:
	/**
	* @brief true if the subwindows is active. Otherwise, false.
	*/
    bool m_active;
	/**
	* @brief pointeur to the current cursor.
	*/
    QCursor* m_currentCursor;
	/**
	* @brief the current tool, it is an enum item.
	*/
    ToolsBar::SelectableTool m_currentTool;
	/**
	* @brief the current color of the pen.
	*/
    QColor m_penColor;
	/**
	* @brief the pen size.
	*/
    int m_penSize;
	/**
	* @brief the NPC size.
	*/
    int m_npcSize;
	/**
	* @brief subwindows type.
	*/
    CleverURI* m_uri;


    RClient* m_client;
    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;

};

#endif // SUBMDIWINDOWS_H
