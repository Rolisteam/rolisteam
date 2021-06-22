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
#include <QMap>

#include "diceparser/include/parsingtoolbox.h"

namespace InstantMessaging
{
namespace
{
QString replacePlaceHolder(const QString& str, const QJsonObject& obj)
{
    auto result= str;

    auto instructions= obj["instructions"].toArray();

    QStringList allDiceResult;
    std::transform(instructions.begin(), instructions.end(), std::back_inserter(allDiceResult),
                   [](const QJsonValue& jsonVal) {
                       auto instruction= jsonVal.toObject();
                       auto diceval= instruction["diceval"].toObject();
                       return diceval["string"].toString();
                   });

    result= result.replace("%1", obj["scalar"].toString());
    result= result.replace("%2", allDiceResult.join(","));
    result= result.replace("%3", obj["scalar"].toString());

    QStringList values;
    std::transform(instructions.begin(), instructions.end(), std::back_inserter(values), [](const QJsonValue& jsonVal) {
        auto inst= jsonVal.toObject();
        auto scalar= inst["scalar"].toDouble();
        return QString::number(scalar);
    });

    QMap<Dice::ERROR_CODE, QString> errorMap;
    qDebug() << "values" << values;
    result= ParsingToolBox::replaceVariableToValue(result, values, errorMap);
    result= ParsingToolBox::replacePlaceHolderFromJson(result, obj);

    return result;
}

} // namespace
DiceMessage::DiceMessage(const QString& ownerId, const QString& writer, const QDateTime& time, QObject* parent)
    : MessageBase(ownerId, writer, time, MessageInterface::MessageType::Dice, parent)
{
}

QString DiceMessage::text() const
{
    return m_text;
}

QString DiceMessage::command() const
{
    return m_command;
}

QString DiceMessage::details() const
{
    return m_details;
}

QString DiceMessage::result() const
{
    return m_result;
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
            /*auto highlight= objVal["highlight"].toBool();
            auto hasColor= !color.isEmpty();*/

            if((face != previousFace && previousFace != -1))
            {
                diceArray << QString("d%1:(%2)").arg(previousFace).arg(vec.join(" "));
                vec.clear();
            }

            vec.append(strValue);
            previousFace= face;

            if(islast)
                diceArray << QString("d%1:(%2)").arg(face).arg(vec.join(" "));
            --i;
        }

        auto scalar= objInst["scalar"].toString();
        auto string= objInst["string"].toString();

        if(!string.isEmpty())
        {
            m_result= replacePlaceHolder(string, obj);
        }
    }

    m_details= diceArray.join(" - ");
}
} // namespace InstantMessaging
