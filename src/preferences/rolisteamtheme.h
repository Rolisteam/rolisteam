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
#include <QPalette>
#include <QString>
#include <QStyle>
#include <QJsonObject>

/**
 * @brief The RolisteamTheme class should store all data required for theme.
 */
class RolisteamTheme
{
public:
    /**
     * @brief RolisteamTheme
     * @param pal
     * @param name
     * @param css
     */
    RolisteamTheme(QPalette pal,QString name,QString css,QStyle* style,QString bgPath,int pos, QColor bgColor,bool);
    /**
     * @brief ~RolisteamTheme
     */
    virtual ~RolisteamTheme();

    /**
     * @brief setPalette
     */
    void setPalette(QPalette);
    /**
     * @brief setName
     */
    void setName(QString);
    /**
     * @brief setCss
     */
    void setCss(QString);
    /**
     * @brief setRemovable
     */
    void setRemovable(bool);

    /**
     * @brief getPalette
     * @return
     */
    const QPalette& getPalette() const;
    /**
     * @brief getName
     * @return
     */
    const QString& getName() const;
    /**
     * @brief getCss
     * @return
     */
    const QString& getCss() const;
    /**
     * @brief isRemovable
     * @return
     */
    bool isRemovable() const;
    /**
     * @brief getStyle
     * @return
     */
    QStyle* getStyle() const;
    /**
     * @brief setStyle
     * @param style
     */
    void setStyle(QStyle* style);


    void setBackgroundColor(QColor);
    void setBackgroundPosition(int);
    void setBackgroundImage(QString img);

    QString getBackgroundImage() const;
    int getBackgroundPosition() const;
    QColor getBackgroundColor() const;

    QColor getDiceHighlightColor() const;
    void  setDiceHighlightColor(QColor c);

    QString getStyleName() const;

    void writeTo(QJsonObject& json);
    bool readFrom(const QJsonObject& json);

private:
    QPalette m_palette;
    QString m_name;
    QString m_css;
    bool m_removable;
    QString m_bgPath;
    QColor m_bgColor;
    int m_position;
    QColor m_diceHighlightColor;
    QColor m_gmColor;
    QString m_styleName;
    int m_position;
};

#endif // ROLISTEAMTHEME_H
