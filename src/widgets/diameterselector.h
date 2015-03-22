/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *    http://www.rolisteam.org/                                          *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#ifndef SELECTEUR_DIAMETRE_H
#define SELECTEUR_DIAMETRE_H

#include <QWidget>
#include <QSlider>

class AfficheurDisque;


/**
 * @brief Dessine un disque dont le diamêtre varie grace a un QSlider.
 */
class SelecteurDiametre : public QWidget
{
    Q_OBJECT
public :
    SelecteurDiametre(QWidget *parent = 0, bool plein = true, int minimum = 1, int maximum = 50);
    void changerDiametre(int nouvelleValeur);

signals:
    void valueChanging(int value);
    void valueChanged(int value);

private :
    int minimum;
    int maximum;
    AfficheurDisque *disque;
    QSlider *diametre;
};

#endif
