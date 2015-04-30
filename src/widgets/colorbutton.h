/***************************************************************************
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


#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>
#include <QColorDialog>

/**
 * @brief A button to choose a color.
 */
class ColorButton
        : public QPushButton
{
    Q_OBJECT

public:
    ColorButton(QWidget * parent, bool transparency=false);
    ColorButton(const QColor & color = QColor("tan"), QWidget * parent = NULL);

    QColor color() const;

    virtual QSize sizeHint() const;
    void setTransparency(bool);

signals:
    void colorChanged(const QColor & color);

public slots:
    void setColor(const QColor & color);


private:
    QColor       m_color;
    QColorDialog m_dialog;
};

#endif
