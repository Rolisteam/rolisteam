/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef MESSAGEHELPER_H
#define MESSAGEHELPER_H

#include <QByteArray>

class Player;
class DiceAliasModel;
class DiceAlias;
class CharacterStateModel;
class CharacterState;
class MessageHelper
{
public:
    MessageHelper();

    static void sendOffConnectionInfo(Player* player, const QByteArray& password);
    static void sendOffGoodBye();
    static void sendOffPlayerInformations(Player* player);
    static void sendOffAllDiceAlias(DiceAliasModel* model);
    static void sendOffOneDiceAlias(DiceAlias* da, int row);

    static void sendOffAllCharacterState(CharacterStateModel* model);
    static void sendOffOneCharacterState(CharacterState* da, int row);
    static void sendOffImage(ImageController* ctrl);
};

#endif // MESSAGEHELPER_H
