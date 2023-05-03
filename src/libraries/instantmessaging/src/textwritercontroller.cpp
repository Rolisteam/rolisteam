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
#include "instantmessaging/textwritercontroller.h"

#include "utils/networkdownloader.h"
#include <QRegularExpression>

static const int MaxHistorySize= 100;

namespace InstantMessaging
{
TextWriterController::TextWriterController(QObject* parent) : QObject(parent) {}

QString TextWriterController::text() const
{
    return m_text;
}

bool TextWriterController::diceCommand() const
{
    return m_diceCmd;
}

QUrl TextWriterController::imageLink() const
{
    return m_imageLink;
}

void TextWriterController::computeText()
{
    QUrl url;
    static auto reg1= QRegularExpression("((?:https?)://\\S+)");
    static auto reg2= QRegularExpression("((?:www)\\S+)");

    auto matcher= reg1.match(m_text);
    auto matcher2= reg2.match(m_text);
    QString text= m_text;
    QString replacePattern;

    QRegularExpression usedRE;

    if(matcher.hasMatch())
    {
        url= QUrl::fromUserInput(matcher.captured(0));
        qDebug() << url;
        usedRE= reg1;
        replacePattern= "<a href=\"\\1\">\\1</a>";
    }
    else if(matcher2.hasMatch())
    {
        url= QUrl::fromUserInput(matcher2.captured(0));
        qDebug() << url;
        usedRE= reg2;
        replacePattern= "<a href=\"http://\\1\">\\1</a>";
    }

    if(url.isValid())
    {
        setUrl(url);
        auto n= new NetworkDownloader(url);

        connect(n, &NetworkDownloader::finished, this,
                [text, url, usedRE, replacePattern, n, this](const QByteArray& data, bool isImage)
                {
                    Q_UNUSED(data)
                    auto realText= text;
                    auto pattern= replacePattern;
                    if(isImage)
                    {
                        m_imageLink= url;
                        pattern= "";
                    }
                    realText= realText.replace(usedRE, pattern);

                    setText(realText);
                    emit textComputed();
                    send();
                    n->deleteLater();
                });

        n->download();
    }
    else
    {
        emit textComputed();
        send();
    }
}

void TextWriterController::setText(const QString& text)
{
    if(m_text == text)
        return;
    m_text= text;
    emit textChanged(m_text);
    setDiceCommand(text.startsWith("!"));
}

void TextWriterController::setDiceCommand(bool b)
{
    if(m_diceCmd == b)
        return;
    m_diceCmd= b;
    emit diceCommandChanged(m_diceCmd);
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
    setUrl(QUrl());
}

void TextWriterController::setUrl(const QUrl& url)
{
    if(m_imageLink == url)
        return;
    m_imageLink= url;
    emit urlChanged();
}

} // namespace InstantMessaging
