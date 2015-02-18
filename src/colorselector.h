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

//#include "types.h"
#include "colortablechooser.h"

class PreferencesManager;
/**
  * @brief shows selected color and detect any event such as click or doubleclick
  */
class ColorLabel : public QAbstractButton
{
    Q_OBJECT
public:
    /**
      * @brief default constructor
      */
   ColorLabel(QWidget * parent = 0);
signals:
   /**
     * @brief send off the current color
     */
    void clickedColor(const QColor& p);
    /**
      * @brief is emitted when users doubleclick on it.
      */
    void doubledclicked();

protected:
    /**
      * @brief redefine the drawing of the widget to show a colof filled square
      */
    void paintEvent( QPaintEvent * event );

    virtual void resizeEvent(QResizeEvent * event);

    /**
      * @brief redefine the mousePressEvent behaviour to emit clickedColor signal
      */
    virtual void mousePressEvent(QMouseEvent *ev);
    /**
      * @brief redefine mouseDoubleClickEvent to emit the doubledclicked signal
      */
    virtual void mouseDoubleClickEvent (QMouseEvent *event);
};
/**
  * @brief custom pushbutton to display a pixmap as background
  * @todo Should be moved into stand alone files (h and cpp) and put them in the widget directory
  *
  */
class BackgroundButton : public QPushButton
{
    Q_OBJECT
public:
    /**
      * @brief constructor with arguments
      * @param pixmap
      */
   BackgroundButton(QPixmap* p,QWidget * parent = 0);

protected:
   /**
     * @brief draw the button
     */
    virtual void paintEvent ( QPaintEvent * event );


private:
   /**
     * @brief stores the background picture
     */
   QPixmap* m_background;

};

/**
  * @brief custom widget to display many color squares, click select the color.
  */
class ColorSelector : public QWidget
{
    Q_OBJECT


public:
    /**
      * @brief Painting mode
      */
    enum PAINTINGMODE{NORMAL,HIDING,UNVEIL,ERASING};
    /**
      * @brief Constructor compliant with QWidget constructor
      */
    ColorSelector(QWidget *parent = 0);
    /**
      * @brief define the current color
      * @param new color
      */
    void setCurrentColor(QColor& color);
    /**
      * @brief accessor to the current color
      * @return current color
      */
    QColor& currentColor();
    /**
      * @brief draw previously selected color in another section
      */
    void customColorUpdate();
    /**
      * @brief no longer use method
      * @todo must be removed
      */
    void allowOrForbideColors();
    /**
      * @brief accessor to personnal color
      */
    QColor getPersonalColor(int numero);

signals:
    /**
      * @brief emitted when color changed
      */
    void currentColorChanged(QColor&);
    /**
      * @brief emitted when current painting mode has changed
      */
    void currentModeChanged(int);


private slots:
    /**
      * @brief select the color as current
      * @param new color
      * @todo Dupplicate with the setter ?
      */
    void selectColor(const QColor& color);
    /**
      * @brief open the color selector dialog box
      */
    void colorSelectorDialog();
    /**
      * @brief slot called when user click on painting mode button
      * @param changed the mode
      */
    void onGroupEdition(QAbstractButton*);
private:
    /**
      * @brief current colorlabel
      */
    ColorLabel *m_currentColorLabel;
    /**
      * @brief erase tool button
      */
    BackgroundButton *m_eraseColor;
    /**
      * @brief hide (fog of war) button
      */
    BackgroundButton *m_hideColor;
    /**
      * @brief removed fog of war.
      */
    BackgroundButton *m_unveilColor;

    /**
      * @brief array of color label
      * @todo perhaps QList will be more efficient
      * @todo name it in english
      */
    ColorLabel *couleurPredefinie[48];
    /**
      * @brief array of personal color
      */
    ColorLabel *couleurPersonnelle[16];
    /**
      * @brief widget to split the tool bar
      */
    QWidget *separateur1;
    /**
      * @brief widget to split the tool bar
      */
    QWidget *separateur2;
    /**
      * @brief erase icon
      * @todo be removed, no need to be a member of the class
      */
    QPixmap *efface_pix;
    /**
      * @brief hide icon
            * @todo be removed, no need to be a member of the class
      */
    QPixmap *masque_pix;
    /**
      * @brief unveil icon
            * @todo be removed, no need to be a member of the class
      */
    QPixmap *unveil_pix;
 //   bool boutonEnfonce;

    /**
      * @brief current color
      */
    QColor m_currentColor;

    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
    /**
      * pointer to the button group which manage editing mode.
      */
    QButtonGroup* m_editingModeGroup;
    ColorTableChooser* m_colorTableChooser;

};

#endif
