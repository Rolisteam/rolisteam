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
#include "textwritercontroller.h"

#include <QRegularExpression>

static const int MaxHistorySize= 100;

namespace InstantMessaging
{
TextWriterController::TextWriterController(QObject *parent) : QObject(parent)
{

}

QString TextWriterController::text() const
{
    return m_text;
}

QString TextWriterController::interpretedText() const
{
    QString result= m_text;
    QString result2= result.replace(QRegularExpression("((?:https?)://\\S+)"), "<a href=\"\\1\">\\1</a>");
    if(result == result2)
    {
        result2 = result.replace(QRegularExpression("((?:www)\\S+)"), "<a href=\"http://\\1\">\\1</a>");
    }

    return result2;
}

void TextWriterController::setText(const QString& text)
{
    if(m_text == text)
        return;
    m_text = text;
    emit textChanged(m_text);
}

void TextWriterController::up()
{
    if(m_historicPostion - 1 < 0 || m_history.isEmpty())
        return;

    setText(m_history[--m_historicPostion]);
}

void TextWriterController::down()
{
    if(m_historicPostion + 1 >= m_history.size())
        return;

    setText(m_history[++m_historicPostion]);
}

void TextWriterController::send()
{
    m_history.append(m_text);
    while(m_history.size() > MaxHistorySize)
    {
        m_history.removeFirst();
    }
    m_historicPostion= m_history.size();
    setText(QString());
}

} // namespace InstantMessaging
