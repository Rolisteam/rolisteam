/*************************************************************************
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


#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include <QtGlobal>

class Liaison;

struct NetworkMessageHeader
{
    quint8  category;
    quint8  action;
    quint32 dataSize;
};

namespace NetMsg
{
    enum Category {
        ConnectionCategory,
        PlayerCategory,
        CharacterCategory,
        NPCCategory,
        PersonCategory,
        DrawCategory,
        MapCategory,
        PictureCategory,
        ChatCategory,
        MusicCategory,
        SetupCategory
    };

    enum Action {
        // ConnectionCategory
        EndConnectionAction = 0,

        // PlayerCategory
        PlayerConnectionAction = 0,
        AddPlayerAction,
        DelPlayerAction,
        ChangePlayerNameAction,
        ChangePlayerColorAction,

        // CharacterCategory
        AddCharacterAction = 0,
        DelCharacterAction,
        ToggleViewCharacterAction,
        ChangeCharacterSizeAction,
        ChangeCharacterNameAction,
        ChangeCharacterColorAction,

        // TODO

        // PictureCategory
        AddPictureAction = 0,
        DelPictureAction,

        // ChatCategory
        ChatMessageAction = 0,
        DiceMessageAction,
        EmoteMessageAction,
        UpdateChatAction,
        DelChatAction,

        // TODO

        // SetupCategory
        AddFeatureAction = 2
    };
}

class NetworkMessage
{
    public:
        void sendTo(Liaison * link);
        void sendAll(Liaison * butLink = NULL);

        virtual NetMsg::Category category() const =0;
        virtual NetMsg::Action action() const =0;

    protected:
        virtual NetworkMessageHeader *  buffer() =0;
};

#endif
