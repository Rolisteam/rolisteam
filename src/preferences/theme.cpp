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
#include "theme.h"

Theme::Theme()
{
}

QString Theme::name() const
{
    return m_name;
}
void Theme::setName(QString name)
{
    m_name=name;
}
QString Theme::backgroundImage() const
{
    return m_backgroundImage;
}

void Theme::setBackgroundImage(QString bgimage)
{
    m_backgroundImage=bgimage;
}

QColor Theme::backgroundColor()const
{
    return m_backgroundColor;
}
void Theme::setBackgroundColor(QColor color)
{
    m_backgroundColor=color;
}

QFont Theme::chatFont()const
{
    return m_chatFont;
}
void Theme::setChatFont(QFont font)
{
m_chatFont=font;
}

QColor Theme::chatColor() const
{
    return m_chatBackgroundColor;
}
void Theme::setChatColor(QColor color)
{
    m_chatBackgroundColor=color;
}

QString Theme::chatBackGroundImage() const
{
    return m_chatBackgroundImage;
}
void Theme::setChatBackGroundImage(QString uri)
{
    m_chatBackgroundImage=uri;
}
QDataStream& operator<<(QDataStream& out, const Theme& con)
{
  out << con.name();
  out << con.backgroundColor();
  out << con.backgroundImage();
  out << con.chatBackGroundImage();
  out << con.chatColor();
  out << con.chatFont();
  return out;
}

QDataStream& operator>>(QDataStream& is,Theme& con)
{
    is >> (con.m_name);
  is >>(con.m_backgroundColor);
  is >>(con.m_backgroundImage);
  is >> (con.m_chatBackgroundImage);
  is >> (con.m_chatBackgroundColor);
  is >> (con.m_chatFont);
  return is;
}
