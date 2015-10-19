/***************************************************************************
	*   Copyright (C) 2015 by Renaud Guezennec                                *
	*   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#ifndef CHARACTERSTATE_H
#define CHARACTERSTATE_H

#include <QString>
#include <QColor>
#include <QPixmap>
/**
 * @brief The CharacterState class stores all data for CharacterState
 */
class CharacterState
{
public:
    /**
     * @brief CharacterState
     */
	CharacterState();
    /**
     * @brief setLabel
     * @param str
     */
	void setLabel(QString str);
    /**
     * @brief setColor
     * @param str
     */
	void setColor(QColor str);
    /**
     * @brief setImage
     * @param str
     */
	void setImage(QPixmap str);
    /**
     * @brief getLabel
     * @return
     */
	const QString& getLabel() const;
    /**
     * @brief getColor
     * @return
     */
	const QColor& getColor() const;
    /**
     * @brief getImage
     * @return
     */
	const QPixmap& getImage() const;


	QPixmap* getPixmap();


private:
	QString m_label;
	QColor m_color;
	QPixmap m_image;
};

#endif // CHARACTERSTATE_H
