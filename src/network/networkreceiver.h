/***************************************************************************
 *	Copyright (C) 2014 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include "network/networkmessagereader.h"
/**
 * @brief The NetWorkReceiver class is an abstract class. It is part of the listener pattern. Any class which need to be process some network message should
 * subclass this class.
 */
class NetWorkReceiver
{
public:
    /**
     * @brief The SendType enum describes how the server should opperate the message. The decision is taken by the GM.
     */
    enum SendType { NONE, ALL,AllExceptMe};
    /**
     * @brief processMessage virtual pure method, it must be implemented by any classes which subclass NetWorkReceiver. The goals is to read the message and trigger the right actions.
     * @param msg is the received network message, it stores the important data.
     * @return How the Server should do with this package.
     */
    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) = 0;
};

#endif // NETWORKRECEIVER_H
