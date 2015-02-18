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
class ValueChooser : public QWidget
{
    Q_OBJECT
 public:
    ValueChooser();
    int getValue() const ;
    void setColor(QColor& color);

public slots:
    void colorHasChanged(int,int);

signals:
    void valueChanged(int);

protected:
    void paintEvent ( QPaintEvent * event );
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent ( QWheelEvent * event );
private:
    int m_currentValue;
    QColor m_color;
    QRect m_gradianRect;
    QPolygon m_polygon;
};

class ColorTable : public QWidget
{
    Q_OBJECT
 public:
    ColorTable();
signals:
    int dataChanged(int,int);

protected:
    virtual void paintEvent ( QPaintEvent * event);
    virtual void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *);
};


class ColorTableChooser : public QWidget
{
    Q_OBJECT
public:
    ColorTableChooser(QWidget* parent);

public slots:
    void colorHasChanged(int,int);
    void valueHasChanged(int);
protected:
    void resizeEvent(QResizeEvent * event);
signals:
    void currentColorChanged(QColor);


private:
    QColor m_color;
    QVBoxLayout* m_layout;
    ColorTable* m_colorTable;
    ValueChooser* m_valueChooser;
    int m_h;
    int m_s;
    //int v;

};

#endif // COLORTABLE_H
