/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QQmlPropertyMap>
#include <map>

namespace customization
{
class Theme;
class StyleSheet : public QQmlPropertyMap
{
    Q_OBJECT
public:
    explicit StyleSheet(Theme* parent);

protected:
    QVariant updateProperty(const QString& key, const QVariant& input);
};

class Theme : public QObject
{
    Q_OBJECT
public:
    explicit Theme(QObject *parent = nullptr);

    Q_INVOKABLE customization::StyleSheet* styleSheet(const QString& id);

    static Theme* instance();

    static void setPath(const QString& path);
private:
    void loadData(const QString& source);
    StyleSheet* addStyleSheet(const QString& name);


private:
    static QString m_dataPath;
    std::map<QString, StyleSheet *> m_styleSheets;
};
}

#endif // THEME_H
