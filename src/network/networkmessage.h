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
//#include "network/networkmanager.h"
class NetworkLink;
class NetworkManager;

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
    CharacterPlayerCategory,
    NPCCategory,
    CharacterCategory,
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

    // CharacterPlayerCategory
    AddPlayerCharacterAction = 0,
    DelPlayerCharacterAction,
    ToggleViewPlayerCharacterAction,
    ChangePlayerCharacterSizeAction,
    ChangePlayerCharacterNameAction,
    ChangePlayerCharacterColorAction,

    //NPCCategory
    addNpc =0,
    delNpc,

    //CharacterCategory
    addCharacterList=0,
    moveCharacter,
    changeCharacterState,
    changeCharacterOrientation,
    showCharecterOrientation,


    // MapCategory
    AddEmptyMap = 0,
    LoadMap,
    ImportMap,
    CloseMap,

    // PictureCategory
    AddPictureAction = 0,
    DelPictureAction,

    // Painting
    penPainting =0,
    linePainting,
    emptyRectanglePainting,
    filledRectanglePainting,
    emptyEllipsePainting,
    filledEllipsePainting,
    textPainting,
    handPainting,




    // ChatCategory
    ChatMessageAction = 0,
    DiceMessageAction,
    EmoteMessageAction,
    UpdateChatAction,
    DelChatAction,

    // MusicCategory
    StopSong =0,
    PlaySong,
    PauseSong,
    NewSong,
    ChangePositionSong,



    // SetupCategory
    AddFeatureAction = 2
};
}
/**
 * @brief The NetworkMessage class
 */
class NetworkMessage
{

public:
    virtual ~NetworkMessage();
	void sendTo(NetworkLink * link);
	void sendAll(NetworkLink * butLink = NULL);

    virtual NetMsg::Category category() const =0;
    virtual NetMsg::Action action() const =0;

protected:
    virtual NetworkMessageHeader *  buffer() =0;

protected:
    NetworkManager* m_server;
};

#endif
