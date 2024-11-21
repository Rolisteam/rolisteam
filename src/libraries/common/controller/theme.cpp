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
#include <QFileInfo>
#include <QRegularExpression>
#include <QTextStream>

namespace customization
{

namespace
{
QString pathToDarkMode(const QString& url, bool darkMode)
{
    auto path= url;
    path= path.replace("qrc", "");
    if(!darkMode || !QFileInfo::exists(path))
        return url;

    QFileInfo info(path);
    auto fileName= info.fileName();
    auto prefixPath= info.absolutePath();

    auto combo= QString("%1/+dark/%2").arg(prefixPath, fileName);

    if(QFileInfo::exists(combo))
        return QString("qrc%1").arg(combo);
    else
        return url;
}
} // namespace
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
    Q_ASSERT(QFileInfo::exists(m_dataPath));
}

void Theme::setNightMode(bool b)
{
    if(b == m_nightMode)
        return;
    m_nightMode= b;
    loadData(m_dataPath);
    emit nightModeChanged(m_nightMode);
    emit folderChanged(folder());
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
                    if(!key.endsWith("_dark"))
                    {
                        currentStyleSheet->insertOrUpdate(key, darkColor(color));
                    }
                    else if(m_nightMode)
                    {
                        currentStyleSheet->insertOrUpdate(key.remove("_dark"), color);
                    }
                }
                else
                {
                    currentStyleSheet->insertOrUpdate(key, pathToDarkMode(value, m_nightMode));
                }
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
    result.setAlpha(color.alpha());
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

QFont Theme::imFont() const
{
    return m_imFont;
}

void Theme::setImFont(const QFont& newImFont)
{
    if(m_imFont == newImFont)
        return;
    m_imFont= newImFont;
    emit imFontChanged();
}

QFont Theme::imLittleFont() const
{
    QFont res= m_imFont;
    res.setPointSize(res.pointSize() * 0.8);
    return res;
}

QFont Theme::imBigFont() const
{
    QFont res= m_imFont;
    res.setPointSize(res.pointSize() * 2);
    res.setBold(true);
    return res;
}

QColor mergedColors(const QColor& colorA, const QColor& colorB, int factor)
{
    const int maxFactor= 100;
    const auto rgbColorB= colorB.toRgb();
    QColor tmp= colorA.toRgb();
    tmp.setRed((tmp.red() * factor) / maxFactor + (rgbColorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (rgbColorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (rgbColorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

QColor highlightedOutline(QColor highLight, bool isDarkMode)
{
    QColor highlightedOutline= highLight.darker(isDarkMode ? 75 : 125).toHsv();
    if(highlightedOutline.value() > 160)
        highlightedOutline.setHsl(highlightedOutline.hue(), highlightedOutline.saturation(), 160);
    return highlightedOutline;
}

QColor Theme::buttonColor(QColor button, QColor highLight, bool isHighlight, bool down, bool hovered) const
{
    QColor buttonColor= button;
    int val= qGray(buttonColor.rgb());
    auto colorOffset= 100 + ((m_nightMode ? -1 : 1) * qMax(1, (180 - val) / 6));

    buttonColor= buttonColor.lighter(colorOffset);
    buttonColor= buttonColor.toHsv();
    buttonColor.setHsv(buttonColor.hue(), int(buttonColor.saturation() * 0.75), buttonColor.value());
    auto outline= highlightedOutline(highLight, m_nightMode);
    if(isHighlight)
        buttonColor= mergedColors(buttonColor, outline.lighter(m_nightMode ? 70 : 130), 90);
    if(!hovered)
        buttonColor= buttonColor.darker(m_nightMode ? 96 : 104);
    if(down)
        buttonColor= buttonColor.darker(m_nightMode ? 90 : 110);

    return buttonColor;
}

QColor Theme::buttonOutline(QColor highLight, QColor window, bool highlighted, bool enabled) const
{
    QColor darkOutline
        = enabled && highlighted ? highlightedOutline(highLight, m_nightMode) : window.darker(m_nightMode ? 60 : 140);
    return !enabled ? darkOutline.lighter(m_nightMode ? 85 : 115) : darkOutline;
}

} // namespace customization
