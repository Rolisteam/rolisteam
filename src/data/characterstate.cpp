/***************************************************************************
	*   Copyright (C) 2015 by Renaud Guezennec                                *
	*   http://www.rolisteam.org/contact                   *
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
#include "characterstate.h"

CharacterState::CharacterState()
{
}
void CharacterState::setLabel(QString str)
{
	m_label = str;
}

void CharacterState::setColor(QColor str)
{
	m_color = str;
}

void CharacterState::setImage(QPixmap str)
{
	m_image = str;
}


const QString& CharacterState::getLabel() const
{
	return m_label;
}
const QColor& CharacterState::getColor() const
{
	return m_color;
}
const QPixmap& CharacterState::getImage() const
{
	return m_image;
}
QPixmap* CharacterState::getPixmap()
{
	return &m_image;
}
