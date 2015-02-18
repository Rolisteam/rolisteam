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
#include <QPushButton>

#include "types.h"

class PreferencesManager;
class ColorLabel : public QAbstractButton
{
    Q_OBJECT
public:
   ColorLabel(QWidget * parent = 0);
signals:
    void clickedColor(const QColor& p);
    void doubledclicked();

protected:
    void paintEvent( QPaintEvent * event );
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseDoubleClickEvent (QMouseEvent *event);


};

class ColorButton : public QPushButton
{
    Q_OBJECT
public:
   ColorButton(QPixmap* p,QWidget * parent = 0);

protected:
    virtual void paintEvent ( QPaintEvent * event );


private:
   QPixmap* m_background;

};


class ColorSelector : public QWidget
{
    Q_OBJECT


public:
    enum PAINTINGMODE{NORMAL,HIDING,UNVEIL,ERASING};
    ColorSelector(QWidget *parent = 0);
    void setCurrentColor(QColor& color);
    QColor& currentColor();
    void customColorUpdate();
    void allowOrForbideColors();
    QColor getPersonalColor(int numero);

signals:
    void currentColorChanged(QColor&);
    void currentModeChanged(int);


private slots:
    void selectColor(const QColor& color);
    void colorSelectorDialog();
    void onGroupEdition(QAbstractButton*);
private:
    ColorLabel *m_currentColorLabel;
    ColorButton *m_eraseColor;
    ColorButton *m_hideColor;
    ColorButton *m_unveilColor;


    ColorLabel *couleurPredefinie[48];
    ColorLabel *couleurPersonnelle[16];
    QWidget *separateur1;
    QWidget *separateur2;
    QPixmap *efface_pix;
    QPixmap *masque_pix;
    QPixmap *unveil_pix;
 //   bool boutonEnfonce;

    QColor m_currentColor;

    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
    /**
      * pointer to the button group which manage editing mode.
      */
    QButtonGroup* m_editingModeGroup;
};

#endif
