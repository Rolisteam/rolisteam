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
/* Dessine un disque dont le diametre varie grace a un QSlider.     */
/*                                                                  */
/********************************************************************/


#ifndef SELECTEUR_DIAMETRE_H
#define SELECTEUR_DIAMETRE_H

#include <QWidget>
#include <QSlider>

#include "displaydisk.h"

/**
    * @brief widget to select diameter used into the toolbar
    */
class DiameterSelector : public QWidget
{
    Q_OBJECT
public :
    /**
    * @brief constuctor with arguments
    * @param parent widget
    * @param full or not
    * @param minimum value
    * @param maximum value
    * @todo Make real default constructor of DiameterSelector
    */
    DiameterSelector(QWidget *parent = 0, bool plein = true, int minimum = 1, int maximum = 50);
    /**
    * @brief sets the new diameter
    * @param new diameter value
    */
    void changerDiametre(int nouvelleValeur);
    
signals:
    /**
    * @brief is emited when the value has been changed
    */
    void diameterChanged(int);
    
private :
    int minimum;
    int maximum;
    DisplayDisk *disk;
    QSlider *m_diameterSlider;
};

#endif
