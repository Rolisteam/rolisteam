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
/* QWorkspace avec une image de fond                                */
/*                                                                  */
/********************************************************************/	


#ifndef WORKSPACE_AMELIORE_H
#define WORKSPACE_AMELIORE_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QWorkspace>


#include "preferencesmanager.h"

class WorkspaceAmeliore : public QWorkspace
{
public :
    WorkspaceAmeliore(QWidget *parent = 0);
    ~WorkspaceAmeliore();

private :
	QImage *imageFond;		// Image de fond du workspace

protected :
    //void paintEvent(QPaintEvent *event);
    void resizeEvent ( QResizeEvent * event );
private:
    QBrush m_background;
    QColor m_color;

    QPixmap* m_backgroundPicture;
    QPixmap* m_variableSizeBackground;

    PreferencesManager* m_preferences;
};

#endif
