/***************************************************************************
 *	Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#include "data/rolisteamtheme.h"

#include <QStyleFactory>
RolisteamTheme::RolisteamTheme() : m_paletteModel(new PaletteModel()) {}

RolisteamTheme::RolisteamTheme(QPalette pal, QString name, QString css, QStyle* style, QString bgPath, int pos,
                               QColor bgColor, bool isRemovable)
    : m_paletteModel(new PaletteModel())
    , m_name(name)
    , m_css(css)
    , m_removable(isRemovable)
    , m_bgPath(bgPath)
    , m_bgColor(bgColor)
    , m_position(pos)
{
    setStyle(style);
    m_paletteModel->setPalette(pal);
}

RolisteamTheme::~RolisteamTheme()= default;

void RolisteamTheme::setPalette(QPalette pal)
{
    m_paletteModel->setPalette(pal);
}

void RolisteamTheme::setName(QString str)
{
    m_name= str;
}

void RolisteamTheme::setCss(QString str)
{
    m_css= str;
}

void RolisteamTheme::setRemovable(bool b)
{
    m_removable= b;
}

QPalette RolisteamTheme::getPalette() const
{
    return m_paletteModel->getPalette();
}
const QString& RolisteamTheme::getName() const
{
    return m_name;
}
const QString& RolisteamTheme::getCss() const
{
    return m_css;
}
bool RolisteamTheme::isRemovable() const
{
    return m_removable;
}
QStyle* RolisteamTheme::getStyle() const
{
    return QStyleFactory::create(m_styleName);
}
void RolisteamTheme::setStyle(QStyle* style)
{
    if(nullptr != style)
    {
        m_styleName= style->objectName();
    }
}
void RolisteamTheme::setBackgroundColor(QColor c)
{
    m_bgColor= c;
}

void RolisteamTheme::setBackgroundImage(QString img)
{
    m_bgPath= img;
}

QString RolisteamTheme::getStyleName() const
{
    return m_styleName;
}

PaletteModel* RolisteamTheme::paletteModel() const
{
    return m_paletteModel.get();
}
QString RolisteamTheme::getBackgroundImage() const
{
    return m_bgPath;
}

int RolisteamTheme::getBackgroundPosition() const
{
    return m_position;
}
void RolisteamTheme::setBackgroundPosition(int p)
{
    m_position= p;
}
QColor RolisteamTheme::getBackgroundColor() const
{
    return m_bgColor;
}
void RolisteamTheme::setDiceHighlightColor(QColor c)
{
    m_diceHighlightColor= c;
}
QColor RolisteamTheme::getDiceHighlightColor() const
{
    return m_diceHighlightColor;
}
