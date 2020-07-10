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
#include "dicemessage.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace InstantMessaging
{
DiceMessage::DiceMessage(const QString& ownerId, const QString& writer, const QDateTime& time, QObject* parent)
    : MessageBase(ownerId, writer, time, MessageInterface::MessageType::Dice, parent)
{
}

QString DiceMessage::text() const
{
    return m_result;
}

QString DiceMessage::command() const
{
    return m_command;
}

QString DiceMessage::details() const
{
    return m_details;
}

QString DiceMessage::json() const
{
    return m_text;
}

QString DiceMessage::comment() const
{
    return m_comment;
}

void DiceMessage::setText(const QString& text)
{
    if(m_text == text)
        return;
    m_text= text;
    emit textChanged();
    computeResult();
}

void DiceMessage::computeResult()
{
    QJsonDocument doc= QJsonDocument::fromJson(m_text.toUtf8());
    auto obj= doc.object();
    m_command= obj["command"].toString();
    m_comment= obj["comment"].toString();
    m_result= obj["scalar"].toString();
    auto instructions= obj["instructions"].toArray();

    QStringList diceArray;
    for(auto instruction : instructions)
    {
        auto objInst= instruction.toObject();
        auto dicevalues= objInst["diceval"].toArray();

        int previousFace= -1;
        QStringList vec;
        int i= dicevalues.count() - 1;
        for(auto val : dicevalues)
        {
            bool islast= (i == 0);
            auto objVal= val.toObject();
            auto face= objVal["face"].toInt();
            auto strValue= objVal["string"].toString();
            auto color= objVal["color"].toString();
            auto highlight= objVal["highlight"].toBool();
            auto hasColor= !color.isEmpty();

            if((face != previousFace && previousFace != -1))
            {
                diceArray << QString("d%1:(%2)").arg(previousFace).arg(vec.join(" "));
                vec.clear();
            }

            QString style;
            if(hasColor)
            {
                style+= QStringLiteral("color:%1;").arg(color);
            }
            if(highlight)
            {
                if(style.isEmpty())
                    style+= QStringLiteral("color:%1;")
                                .arg("red"); // default color must get the value from the setting object
                style+= QStringLiteral("font-weight:bold;");
            }
            if(!style.isEmpty())
                strValue= QString("<span style=\"%2\">%1</span>").arg(strValue).arg(style);

            vec.append(strValue);
            previousFace= face;

            if(islast)
                diceArray << QString("d%1:(%2)").arg(face).arg(vec.join(" "));
            --i;
        }

        auto scalar= objInst["scalar"].toString();
        auto string= objInst["string"].toString();
    }

    m_details= diceArray.join(" - ");
}
} // namespace InstantMessaging
