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

#ifndef AFFICHEUR_DISQUE_H
#define AFFICHEUR_DISQUE_H

#include <QWidget>

/**
 * @brief The AfficheurDisque class display a size selector for circle.
 */
class AfficheurDisque : public QWidget
{
    Q_OBJECT

    public :
        AfficheurDisque(QWidget *parent = 0, bool plein = true, int minimum = 1,int maximum=50);
    public slots :
        void changerDiametre(int diametre);
    protected:
        void paintEvent(QPaintEvent *event);
    private :
        int diametreCourant;
        int diametreMinimum;
        int m_maxDiameter;
        float m_scale;
        bool disquePlein;
};

#endif
