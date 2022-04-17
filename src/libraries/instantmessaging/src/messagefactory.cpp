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
#include "instantmessaging/messagefactory.h"

#include <QLoggingCategory>

#include <common/logcontroller.h>

#include "instantmessaging/commandmessage.h"
#include "instantmessaging/dicemessage.h"
#include "instantmessaging/errormessage.h"
#include "instantmessaging/messageinterface.h"
#include "instantmessaging/textmessage.h"

QLoggingCategory rDice("rolisteam.dice");

namespace InstantMessaging
{
MessageInterface* MessageFactory::createMessage(const QString& uuid, const QString& writerId, const QDateTime& time,
                                                InstantMessaging::MessageInterface::MessageType type,
                                                const QString& text)
{
    using IM= InstantMessaging::MessageInterface;
    MessageInterface* msg= nullptr;
    switch(type)
    {
    case IM::Text:
        msg= new TextMessage(uuid, writerId, time);
        break;
    case IM::Dice:
        qCInfo(rDice) << QString("Player %1 just roll: %2").arg(writerId, helper::log::humanReadableDiceResult(text));
        msg= new DiceMessage(uuid, writerId, time);
        break;
    case IM::Command:
        msg= new CommandMessage(uuid, writerId, time);
        break;
    case IM::Error:
        msg= new ErrorMessage(uuid, writerId, time);
        break;
    }
    if(msg)
        msg->setText(text);
    return msg;
}
} // namespace InstantMessaging
