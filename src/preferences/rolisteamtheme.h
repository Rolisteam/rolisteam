/***************************************************************************
*	Copyright (C) 2015 by Renaud Guezennec                                *
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
#ifndef ROLISTEAMTHEME_H
#define ROLISTEAMTHEME_H

#include <QObject>
/**
 * @brief The RolisteamTheme class should store all data required for theme.
 */
class RolisteamTheme
{
    Q_OBJECT
public:
    RolisteamTheme();
    ~RolisteamTheme();


    void setPalette(QPalette);
    void setName(QString);
    void setCss(QString);
    void setRemovable(bool);


    const QPalette& getPalette() const;
    const QString& getName() const;
    const QString& getCss() const;
    bool isRemovable();

private:
    QPalette m_palette;
    QString m_name;
    QString m_css;
    bool m_removable;
};

#endif // ROLISTEAMTHEME_H
