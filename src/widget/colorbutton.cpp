/***************************************************************************
    *	Copyright (C) 2011 by Renaud Guezennec                             *
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
#include "colorbutton.h"
#include <QColorDialog>
ColorButton::ColorButton(QWidget * parent)
    : QPushButton(parent)
{
    connect(this,SIGNAL(clicked()),this,SLOT(changeColor()));
}
void ColorButton::setColor(QColor color)
{
    m_color=color;
    setPalette(QPalette(color));
}
const QColor& ColorButton::color() const
{
    return m_color;
}
void ColorButton::changeColor()
{
    QColor color = QColorDialog::getColor(m_color);
    if (color.isValid())
    {
        setPalette(QPalette(color));
        m_color = color;
        emit colorChanged(m_color);
    }
}
