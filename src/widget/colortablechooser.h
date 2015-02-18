/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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

#ifndef COLORTABLE_H
#define COLORTABLE_H

#include <QColor>
#include <QWidget>


class QVBoxLayout;
/**
  * @brief SaturationChooser is a widget which stores and manages saturation value
  */
class SaturationChooser : public QWidget
{
    Q_OBJECT
 public:
    /**
      * @brief default constructor
      */
    SaturationChooser();
    /**
      * @brief accessor for the current value
      * @return int value
      */
    int getValue() const ;
    /**
      * @brief setter for current color
      * @param base color
      */
    void setColor(QColor& color);

public slots:
    /**
      * @brief slot to handle change in the base color
      * @param new color's hue
      * @param new color's lightness
      */
    void colorHasChanged(int,int);

signals:
    /**
      * @brief emited when saturation changes
      */
    void valueChanged(int);

protected:
     /**
       * @brief redefines the painting method to display the appropriate gradiant
       */
    void paintEvent ( QPaintEvent * event );
    /**
      * @brief redefines mouse click behaviour to make it easier to use
      */
    void mousePressEvent(QMouseEvent* e);
    /**
      * @brief redefines mouse move behaviour to make it easier to use
      */
    void mouseMoveEvent(QMouseEvent* e);
    /**
      * @brief redefines mouse wheel behaviour to make it easier to use
      */
    void wheelEvent ( QWheelEvent * event );
private:

    int m_currentValue;///< stores current saturation value
    QColor m_color; ///< stores the current color
    QRect m_gradianRect;///< selector zone
    QPolygon m_polygon; ///< zone's geometry
};
/**
  * @brief ColorTable is a widget which display a large panel of color, and allows user to chose one of them.
  * @todo make the performance improvement, using image should be enough or implement a better resizing policy.
  */
class ColorTable : public QWidget
{
    Q_OBJECT
 public:
    /**
      * @brief default constructor
      */
    ColorTable();
signals:
    /**
      * @brief emited when user click on the widget to select another color.
      */
    int dataChanged(int,int);

protected:
    /**
      * @brief displays the large set of color thanks of gradiant use.
      */
    virtual void paintEvent ( QPaintEvent * event);
    /**
      * @brief manages the resize event to make sure, it keeps square shape.
      */
    virtual void resizeEvent(QResizeEvent * event);
    /**
      * @brief manages the click and emit the selected color's values
      */
    void mousePressEvent(QMouseEvent *);
};

/**
  * @brief ColorTableChooser is the parent widget of ColorTable and SaturationChooser
  */
class ColorTableChooser : public QWidget
{
    Q_OBJECT
public:
    /**
      * @brief constructor
      * @param parent widget
      */
    ColorTableChooser(QWidget* parent);

public slots:
    /**
      * @brief handle current color change, users click on the colortable
      */
    void colorHasChanged(int,int);
    /**
      * @brief handle the current saturation of the current color, users click on saturationchooser
      */
    void valueHasChanged(int);
protected:
    /**
      * @brief resize the widget
      */
    void resizeEvent(QResizeEvent * event);
signals:
    /**
      * @brief emitted when user has selected another color.
      */
    void currentColorChanged(QColor);


private:
    QColor m_color;/// current color.
    QVBoxLayout* m_layout;/// vertical layout
    ColorTable* m_colorTable;/// colortable
    SaturationChooser* m_valueChooser; /// saturation chooser
    int m_h; /// current hue
    int m_s; /// current saturation
    //int v;

};

#endif // COLORTABLE_H
