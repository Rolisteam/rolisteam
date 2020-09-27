/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "theme.h"

#include <QQmlEngine>
#include <QQmlFileSelector>

Theme::Theme(QQmlEngine* engine, QObject* parent) : QObject(parent), m_engine(engine)
{
    m_selector.reset(new QQmlFileSelector(m_engine));
    m_selector->setExtraSelectors(QStringList() << QStringLiteral("+dark"));
}

Theme::~Theme()= default;

bool Theme::nightMode() const
{
    return m_nightMode;
}

void Theme::setNightMode(bool b)
{
    if(m_nightMode == b)
        return;
    m_nightMode= b;
    emit nightModeChanged();
    emit undoIconChanged();
    emit redoIconChanged();
    emit listIconChanged();
}

QString Theme::imagePath(const QString& image) const
{
    return QStringLiteral("qrc:/icons/resources/icons/%1%2")
        .arg(m_nightMode ? QStringLiteral("+dark/") : QStringLiteral(""))
        .arg(image);
}

QString Theme::undoIcon() const
{
    return imagePath("undo.svg");
}

QString Theme::redoIcon() const
{
    return imagePath("redo.svg");
}

QString Theme::listIcon() const
{
    return imagePath("list.svg");
}
