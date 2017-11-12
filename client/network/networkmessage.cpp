/*************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
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

#ifndef UNIT_TEST
#include "network/networklink.h"
#include "network/networkmanager.h"
#endif



NetworkMessage::NetworkMessage(NetworkLink* linkToServer)
    : m_linkToServer(linkToServer)
{

}

NetworkMessage::~NetworkMessage()
{

}

void NetworkMessage::sendTo(NetworkLink * link)
{
    if (link == nullptr)
    {
        sendAll();
        return;
    }

    NetworkMessageHeader * header = buffer();
    #ifndef UNIT_TEST
    link->sendData((char *)header, header->dataSize + sizeof(NetworkMessageHeader));
    #endif
}

void NetworkMessage::sendAll(NetworkLink * butLink)
{
    NetworkMessageHeader* header = buffer();
#ifndef UNIT_TEST
    m_linkToServer = ClientManager::getLinkToServer();
    if(nullptr != m_linkToServer)
    {
        m_linkToServer->sendData(reinterpret_cast<char*>(header), header->dataSize + sizeof(NetworkMessageHeader), butLink);
    }
#endif
}
quint64 NetworkMessage::getSize()
{
    if(buffer()!=nullptr)
    {
        NetworkMessageHeader* header = buffer();
        return  header->dataSize + sizeof(NetworkMessageHeader);
    }
    return 0;
}
void NetworkMessage::setLinkToServer(NetworkLink* linkToServer)
{
    m_linkToServer = linkToServer;
}
