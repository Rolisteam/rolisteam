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
#include <common_qml/theme.h>

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace customization
{
StyleSheet::StyleSheet(Theme* parent) : QQmlPropertyMap(parent) {}

void StyleSheet::insertOrUpdate(const QString& key, const QVariant& value)
{
    insert(key, value);

    emit valueChanged(key, value);
}

QString Theme::m_dataPath= "";

Theme::Theme(QObject* parent) : QObject(parent)
{
    loadData(m_dataPath);
}

StyleSheet* Theme::styleSheet(const QString& id)
{
    auto it= m_styleSheets.find(id);
    if(it == m_styleSheets.end())
        return nullptr;
    return it->second;
}

bool Theme::nightMode() const
{
    return m_nightMode;
}

QString Theme::folder() const
{
    return m_nightMode ? QStringLiteral("+dark/") : QString();
}

void Theme::setPath(const QString& path)
{
    m_dataPath= path;
}

void Theme::setNightMode(bool b)
{
    if(b == m_nightMode)
        return;
    m_nightMode= b;
    emit nightModeChanged(m_nightMode);
    emit folderChanged(folder());
    loadData(m_dataPath);
}

Theme* Theme::instance()
{
    static Theme theme;
    return &theme;
}

void Theme::loadData(const QString& source)
{
    QFile file(source);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    StyleSheet* currentStyleSheet= nullptr;
    while(!in.atEnd())
    {
        auto line= file.readLine();
        if(line.isEmpty())
            continue;

        auto str= line;

        if(str.startsWith("["))
        {
            auto label= str.mid(1, str.length() - 3);
            currentStyleSheet= addStyleSheet(label);
        }
        else if(currentStyleSheet)
        {
            QRegularExpression exp("^(\\w+)=(.*)$");
            auto match= exp.match(str, 0, QRegularExpression::PartialPreferCompleteMatch);
            if(match.hasMatch())
            {
                auto key= match.captured(1);
                auto value= match.captured(2);
                bool ok;
                auto intVal= value.toInt(&ok);
                if(ok)
                {
                    currentStyleSheet->insertOrUpdate(key, intVal);
                    continue;
                }

                auto realVal= value.toDouble(&ok);
                if(ok)
                {
                    currentStyleSheet->insertOrUpdate(key, realVal);
                    continue;
                }

                QColor color;
                color.setNamedColor(value);
                if(color.isValid())
                {
                    currentStyleSheet->insertOrUpdate(key, darkColor(color));
                }
                else
                    currentStyleSheet->insertOrUpdate(key, value);
            }
        }
    }
}

QColor Theme::darkColor(const QColor& color)
{
    if(!m_nightMode)
        return color;

    std::vector<int> c({color.red(), color.green(), color.blue()});
    auto brighness= [](const QColor& current)
    { return ((current.red() * 299) + (current.green() * 587) + (current.blue() * 114)) / 1000; };
    auto avg= std::accumulate(c.begin(), c.end(), 0.0) / static_cast<double>(c.size());
    auto allEqual= std::all_of(c.begin(), c.end(), [color](int tmp) { return color.red() == tmp; });
    QColor result= color;
    if(allEqual)
    {
        auto cVal= static_cast<int>(255 - avg);
        result= QColor(cVal, cVal, cVal);
    }
    else
    {
        auto brighnessVal= brighness(result);
        auto target= 255 - brighness(result);
        auto darker= brighnessVal > 128;
        int i= 0;
        while(std::abs(brighnessVal - target) > 50 && i < 8)
        {
            result= darker ? result.darker(120) : result.lighter(120);
            brighnessVal= brighness(result);
            ++i;
        }
    }

    return result;
}

StyleSheet* Theme::addStyleSheet(const QString& name)
{
    auto styleSheet= Theme::styleSheet(name);
    if(nullptr == styleSheet)
    {
        styleSheet= new StyleSheet(this);
        m_styleSheets.insert({name, styleSheet});
    }
    return styleSheet;
}

} // namespace customization
