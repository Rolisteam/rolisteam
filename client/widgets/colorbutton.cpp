/***************************************************************************
 *     Copyright (C) 2010 by Joseph Boudou                                 *
 *     Copyright (C) 2014 by Renaud Guezennec                              *
 *     http://www.rolisteam.org/                                           *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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


#include "colorbutton.h"

#include <QColorDialog>
#include <QPaintEvent>
#include <QStyleOptionFocusRect>
#include <QDebug>
#include <QStyleFactory>


ColorButton::ColorButton(QWidget * parent,bool transparency)
    : QPushButton(parent), m_color(QColor("tan")),m_hasTransparency(transparency)
{

    setStyle(QStyleFactory::create("fusion"));
    //}
    setColor(m_color);

    setMaximumSize(sizeHint());

    // should never be the default button
    setDefault(false);
    setAutoDefault(false);
  //  m_dialog.setCurrentColor(m_color);

    connect(this, SIGNAL(pressed()), this, SLOT(openDialog()));
    //connect(&m_dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(setColor(const QColor &)));
}

ColorButton::ColorButton(const QColor & color, QWidget * parent)
    : QPushButton(parent), m_color(color)//, m_dialog(color, this)
{

    setStyle(QStyleFactory::create("fusion"));

    setColor(m_color);

    setMaximumSize(sizeHint());

    setDefault(false);
    setAutoDefault(false);
   // m_dialog.setCurrentColor(m_color);
    connect(this, SIGNAL(clicked()), this, SLOT(openDialog()));
   // connect(&m_dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(setColor(const QColor &)));
}
ColorButton::~ColorButton()
{


}
void ColorButton::openDialog()
{
    QColorDialog dialog;
    dialog.setCurrentColor(m_color);
    dialog.setOption(QColorDialog::ShowAlphaChannel,m_hasTransparency);
    if(QDialog::Accepted==dialog.exec())
    {
        setColor(dialog.currentColor());
    }
}

QColor ColorButton::color() const
{
    return m_color;
}
void ColorButton::setTransparency(bool state)
{

  m_hasTransparency = state;
}

QSize ColorButton::sizeHint() const
{
    return QSize(28,20);
}

void ColorButton::setColor(const QColor & color)
{
    if(m_color!=color)
    {
        m_color = color;
        QPalette tmp = palette();
        tmp.setColor(QPalette::Button,m_color);
        tmp.setColor(QPalette::Window,m_color);
        setPalette(tmp);
        setAutoFillBackground(true);
        setStyleSheet(QString("ColorButton { background-color: rgb(%1,%2,%3);}").arg(color.red()).arg(color.green()).arg(color.blue()));
        emit colorChanged(m_color);
    }
}


