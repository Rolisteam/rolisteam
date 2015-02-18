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

#include "types.h"

class PreferencesManager;
class ColorLabel : public QWidget
{
    Q_OBJECT
public:
   ColorLabel(QWidget * parent = 0);
signals:
    void clickedColor(const QColor& p);
    void doubledclicked();
protected:
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseDoubleClickEvent (QMouseEvent *event);
};




class ColorSelector : public QWidget
{
    Q_OBJECT
public:
    ColorSelector(QWidget *parent = 0);
    void setCurrentColor(QColor& color);
    QColor& currentColor();
    void majCouleursPersonnelles();
    void autoriserOuInterdireCouleurs();
    QColor donnerCouleurPersonnelle(int numero);

signals:
    void currentColorChanged(QColor&);

private slots:
    void selectColor(const QColor& color);
    void colorSelectorDialog();
private:
    ColorLabel *m_currentColorLabel;
    QLabel *couleurEfface;
    QLabel *couleurMasque;
    QLabel *couleurDemasque;
    ColorLabel *couleurPredefinie[48];
    ColorLabel *couleurPersonnelle[16];
    QWidget *separateur1;
    QWidget *separateur2;
    QPixmap *efface_pix;
    QPixmap *masque_pix;
    QPixmap *demasque_pix;
 //   bool boutonEnfonce;

    QColor m_currentColor;

    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
};

#endif
