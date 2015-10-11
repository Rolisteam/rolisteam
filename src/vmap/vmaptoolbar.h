/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#ifndef VMAPTOOLBAR_H
#define VMAPTOOLBAR_H

#include <QToolBar>


#include "vmap.h"
#include "widgets/colorbutton.h"

class VmapToolBar : public QToolBar
{
    Q_OBJECT
public:
    VmapToolBar();
    ~VmapToolBar();

    void setupUi();


public slots:
    void setCurrentMap(VMap* map);


private:
    void updateUi();


private:
    VMap* m_vmap;

    //Button
    ColorButton* m_bgSelector;



    QAction* m_showGridAct;
    QAction* m_Act;
   /* QAction* m_Act;
    QAction* m_Act;
    QAction* m_Act;*/

};

#endif // VMAPTOOLBAR_H
