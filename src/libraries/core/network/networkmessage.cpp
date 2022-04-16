/*************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   https://rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include "networkmessage.h"

MessageSenderInterface* NetworkMessage::m_sender= nullptr;

MessageSenderInterface::~MessageSenderInterface()= default;

NetworkMessage::NetworkMessage() {}

NetworkMessage::~NetworkMessage() {}

void NetworkMessage::sendToServer()
{
    if(nullptr == m_sender)
        return;

    m_sender->sendMessage(this);
}

quint64 NetworkMessage::getSize() const
{
    if(buffer() == nullptr)
        return 0;

    NetworkMessageHeader* header= buffer();
    return header->dataSize + sizeof(NetworkMessageHeader);
}

void NetworkMessage::setMessageSender(MessageSenderInterface* sender)
{
    m_sender= sender;
}
