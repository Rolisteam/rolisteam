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
/* Permet de selectionner une couleur dans une palette predefinie   */
/* qui peut etre en partie modifiee.                                */
/*                                                                  */
/********************************************************************/


#ifndef SELECTEUR_COULEUR_H
#define SELECTEUR_COULEUR_H

#include <QWidget>
#include <QFrame>
#include <QColor>
#include <QLabel>


#include "preferences/preferencesmanager.h"
#include "types.h"

class QHBoxLayout;
class QGridLayout;
class QVBoxLayout;
/**
 * @brief The SelecteurCouleur class
 */
class SelecteurCouleur : public QWidget
{
    Q_OBJECT
public:
    SelecteurCouleur(QWidget *parent = 0);
    ~SelecteurCouleur();
    void changeCouleurActuelle(QColor couleur);
    void majCouleursPersonnelles();
    void autoriserOuInterdireCouleurs();
    QColor donnerCouleurPersonnelle(int numero);

private:
    void clicUtilisateur(QPoint positionSouris, bool move = false);

    QLabel *couleurActuelle;
    QLabel *couleurEfface;
    QLabel *couleurMasque;
    QLabel *couleurDemasque;
    QWidget *couleurPredefinie[48];
    QWidget *couleurPersonnelle[16];
    QWidget *separateur1;
    QWidget *separateur2;
    QPixmap *efface_pix;
    QPixmap *masque_pix;
    QPixmap *demasque_pix;
    QHBoxLayout *couleursSpeciales;
    QGridLayout *grillePerso;
    QVBoxLayout *selecteurLayout;
    QGridLayout *grillePredef;
    bool boutonEnfonce;
    PreferencesManager* m_preferences;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif
