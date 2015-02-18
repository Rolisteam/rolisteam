/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2010 by Renaud Guezennec                             *
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

#ifndef TEXTEDIT_AMELIORE_H
#define TEXTEDIT_AMELIORE_H

#include <QTextEdit>
#include <QKeyEvent>

/**
  * @brief clever QTextEdit which deals with user inputs and trigger some action in response of some sort of input
  */
class TchatEditor : public QTextEdit
{
Q_OBJECT
public :
    TchatEditor(QWidget *parent = 0);
signals :
        void onEntry();
        void onArrowUp();
        void onArrowDown();
protected :
        void keyPressEvent(QKeyEvent *e);
};

#endif
