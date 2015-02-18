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


/**
 *
 * @brief QMdiArea with backgound picture, Manging all subWindows
 *
 */


#ifndef WORKSPACE_AMELIORE_H
#define WORKSPACE_AMELIORE_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QMdiArea>
#include "BarreOutils.h"
#include "submdiwindows.h"

class QPixmap;
class ImprovedWorkspace : public QMdiArea
{
    Q_OBJECT

public:
    ImprovedWorkspace(QWidget *parent = 0);

    void addWidget(SubMdiWindows* subWindow);

public slots:
    void currentToolChanged(ToolsBar::SelectableTool );

signals:
    void currentCursorChanged(QCursor*);
    void currentToolHasChanged(ToolsBar::SelectableTool);

protected:
    void resizeEvent ( QResizeEvent * event );






private:
    /**
      * initialize all cursor
      */
    void initCursors();

    /**
      * store the picture
      */
    QPixmap* m_backgroundPicture;
    /**
      * store the pixmap of the whole background
      */
    QPixmap* m_variableSizeBackground;

    /**
      * cursor for drawing
      */
    QCursor *m_drawingCursor;

    /**
      * cursor for text
      */
    QCursor *m_textCursor;
    /**
      * cursor for moving items
      */
    QCursor *m_moveCursor;
    /**
      * cursor for rotate items
      */
    QCursor *m_rotateCursor;
    /**
      * cursor for selecting color
      */
    QCursor *m_pipetteCursor;
    /**
      * cursor for adding items
      */
    QCursor *m_addCursor;
    /**
      * cursor for deleting items
      */
    QCursor *m_deleteCursor;
    /**
      * cursor for getting item's status.
      */
    QCursor *m_stateCursor;

    /**
      * current cursor.
      */
    QCursor *m_handCursor;

    /**
      * current cursor.
      */
    QCursor *m_currentCursor;

private slots:
    /**
      * slot call each time when the subWindowChanged
      */
   // void activeSubWindowChanged(QMdiSubWindow*);

};

#endif
