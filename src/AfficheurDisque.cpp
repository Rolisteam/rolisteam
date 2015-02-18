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


#include <QtGui>
#include <QDebug>

#include "AfficheurDisque.h"



#define MARGIN_NPC 10
#define MARGIN_PC 5

#define PEN_SIZE 4
/********************************************************************/
/* Variables globales utilisees par tous les elements de            */
/* l'application                                                    */
/********************************************************************/
// Definit la taille courante du trait
int G_diametreTraitCourant;


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/
AfficheurDisque::AfficheurDisque(QWidget *parent, bool plein, int minimum,int maximum)
    : QWidget(parent)
{
    // Initialisation des variables de la classe
    diametreCourant = minimum;
    diametreMinimum = minimum;
    m_maxDiameter=maximum;


    disquePlein = plein;

    int marge=MARGIN_NPC;
    // Mise a jour des variables globales (Argh...)
    if (disquePlein)
      {
        G_diametreTraitCourant = minimum;
        marge=MARGIN_PC;
      }
    m_scale = (float)(width()-marge)/(float)(m_maxDiameter*2+PEN_SIZE*2);
    //qDebug() << "scale ://" << m_scale <<width() ;
}

/********************************************************************/
/* Dessine le contenu du widget                                     */
/********************************************************************/
void AfficheurDisque::paintEvent(QPaintEvent *event)
{
        Q_UNUSED(event)
    int diametreAffiche;
    // Creation du painter
    QPainter painter(this);
    // Antialiasing pour les dessins
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Si le disque est plein (utilise pour le diametre du trait)
    if (disquePlein)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        diametreAffiche = diametreCourant;
    }
    // Si le disque est vide (utilise pour le diametre des PNJ)
    else
    {
        QPen pen(Qt::black);
        pen.setWidth(PEN_SIZE);
        painter.setPen(pen);
        painter.setBrush(Qt::white);
        diametreAffiche = diametreCourant - diametreMinimum +1;
        //qDebug() << diametreAffiche << diametreCourant << diametreMinimum;
    }

    // Dessin du disque
    painter.drawEllipse((width()-(diametreCourant*m_scale))/2, (height()-(diametreCourant*m_scale))/2, diametreCourant*m_scale, diametreCourant*m_scale);
    painter.setPen(Qt::darkGray);
    // Affichage du diametre
    painter.drawText(0, 0, width(), height(), Qt::AlignRight | Qt::AlignBottom, QString::number(diametreAffiche));
}

/********************************************************************/
/* Modifie le diametre du disque et le redessine                    */
/********************************************************************/
void AfficheurDisque::changerDiametre(int diametre)
{
    diametreCourant = diametre;

    // Mise a jour des variables globales
    if (disquePlein)
        G_diametreTraitCourant = diametre;

    // Rafraichissement du widget
    update();
}

