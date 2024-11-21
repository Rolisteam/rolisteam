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

#include <QFont>
#include <QObject>
#include <QQmlPropertyMap>
#include <QQmlEngine>
#include <QPalette>
#include <common_qml/common_qml_global.h>
#include <map>

namespace customization
{
class Theme;
class COMMON_QML_EXPORT StyleSheet : public QQmlPropertyMap
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Managed by Theme")
public:
    explicit StyleSheet(Theme* parent);
    void insertOrUpdate(const QString& key, const QVariant& value);

    // protected:
    //    QVariant updateProperty(const QString& key, const QVariant& input);
};

class COMMON_QML_EXPORT Theme : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool nightMode READ nightMode WRITE setNightMode NOTIFY nightModeChanged)
    Q_PROPERTY(QString folder READ folder NOTIFY folderChanged)
    Q_PROPERTY(QFont imFont READ imFont WRITE setImFont NOTIFY imFontChanged FINAL)
    Q_PROPERTY(QFont imLittleFont READ imLittleFont NOTIFY imFontChanged FINAL)
    Q_PROPERTY(QFont imBigFont READ imBigFont NOTIFY imFontChanged FINAL)
public:
    explicit Theme(QObject* parent= nullptr);

    Q_INVOKABLE customization::StyleSheet* styleSheet(const QString& id);
    bool nightMode() const;

    QString folder() const;

    static Theme* instance();

    static void setPath(const QString& path);

    QFont imFont() const;
    void setImFont(const QFont& newImFont);

    QFont imLittleFont() const;
    QFont imBigFont() const;

    Q_INVOKABLE QColor buttonColor(QColor button, QColor highLight, bool isHighlight, bool down, bool hovered) const;
    Q_INVOKABLE QColor buttonOutline(QColor highLight, QColor window,  bool highlighted, bool enabled) const;

public slots:
    void setNightMode(bool b);

signals:
    void nightModeChanged(bool b);
    void folderChanged(QString f);
    void imFontChanged();

private:
    void loadData(const QString& source);
    StyleSheet* addStyleSheet(const QString& name);
    QColor darkColor(const QColor& color);

private:
    static QString m_dataPath;
    std::map<QString, StyleSheet*> m_styleSheets;
    bool m_nightMode= false;
    QFont m_imFont;
};
} // namespace customization

#endif // THEME_H
