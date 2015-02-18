/***************************************************************************
    *     Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify     *
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
#ifndef RGRAPHICSVIEW_H
#define RGRAPHICSVIEW_H

#include <QGraphicsView>
#include "map.h"
/**
    * @brief RGraphicsView is custom graphicsview to fit rolisteam needs. It will implement some important feature
    */
class RGraphicsView : public QGraphicsView
{
public:
    /**
    * @brief constructor with parameters
    * @param Map address which it will be displayed by the graphicsview
    */
    RGraphicsView(Map *map);
    
    
    
protected:
    void keyPressEvent ( QKeyEvent * event);
    void mousePressEvent ( QMouseEvent * event);
    void focusInEvent ( QFocusEvent * event );
    void dragEnterEvent ( QDragEnterEvent * event );
    void dropEvent ( QDropEvent * event );
    void dragMoveEvent( QDragMoveEvent * event );
private:
    Map* m_map;
    
};

#endif // RGRAPHICSVIEW_H
