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
#ifndef PLAYERMESSAGEHELPER_H
#define PLAYERMESSAGEHELPER_H

#include <QByteArray>

class Player;
class NetworkMessageWriter;
class NetworkMessageReader;
class Character;
class PlayerModel;
class ClientManager;
class PlayerMessageHelper
{
public:
    PlayerMessageHelper();

    static void sendOffConnectionInfo(Player* player, const QByteArray& password);
    static void writePlayerIntoMessage(NetworkMessageWriter& msg, Player* player);
    static void writeCharacterIntoMessage(NetworkMessageWriter& msg, Character* character);
    static void sendOffPlayerInformations(Player* player);

    static bool readPlayer(NetworkMessageReader& msg, Player* player);
    static Character* readCharacter(NetworkMessageReader& msg);
};

#endif // PLAYERMESSAGEHELPER_H
