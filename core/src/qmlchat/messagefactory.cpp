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
#include "messagefactory.h"

#include "qmlchat/dicemessage.h"
#include "qmlchat/messageinterface.h"
#include "qmlchat/textmessage.h"

namespace InstantMessaging
{
MessageInterface* MessageFactory::createMessage(const QString& uuid, const QString& writerId, const QDateTime& time,
                                                InstantMessaging::MessageInterface::MessageType type)
{
    using IM= InstantMessaging::MessageInterface;
    MessageInterface* msg= nullptr;
    switch(type)
    {
    case IM::Text:
        msg= new TextMessage(uuid, writerId, time);
        break;
    case IM::Dice:
        msg= new DiceMessage(uuid, writerId, time);
        break;
    case IM::Command:
        // TODO command message
        break;
    case IM::Error:
        // TODO ERROR MESSAGE
        break;
    }
    return msg;
}
} // namespace InstantMessaging
