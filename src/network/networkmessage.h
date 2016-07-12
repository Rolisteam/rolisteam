/*************************************************************************
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   Copyright (C) 2015 by Renaud Guezennec                              *
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
    SetupCategory,
    SharePreferencesCategory,
    VMapCategory
};

enum Action {
    // ConnectionCategory
    EndConnectionAction = 0,
    heartbeat,

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
    ChangePlayerCharacterAvatarAction,

    //NPCCategory
    addNpc =0,
    delNpc,

    //CharacterCategory
    addCharacterList=0,
    moveCharacter,
    changeCharacterState,
    changeCharacterOrientation,
    showCharecterOrientation,
    addCharacterSheet,
    updateFieldCharacterSheet,
    closeCharacterSheet,


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
    AddFeatureAction = 2,

    //SharePreferencesCategory
    addDiceAlias = 0,
    moveDiceAlias,
    removeDiceAlias,
    addState,
    moveState,
    removeState,

    //Vmap
    addVmap = 0,
    vmapChanges,
    loadVmap,
    closeVmap,
    addItem,
    DelItem,
    MoveItem,
    DelPoint,
    OpacityItemChanged,
    LayerItemChanged,
    GeometryItemChanged,
    AddPoint,
    GeometryViewChanged,
    SetParentItem
};
}
/**
 * @brief The NetworkMessage class is pure virtual class to manage network message.
 */
class NetworkMessage
{

public:
    virtual ~NetworkMessage();
    /**
     * @brief sendTo
     * @param link
     */
	void sendTo(NetworkLink * link);
    /**
     * @brief sendAll
     * @param butLink
     */
	void sendAll(NetworkLink * butLink = NULL);
    /**
     * @brief category
     * @return
     */

    virtual NetMsg::Category category() const =0;
    /**
     * @brief action
     * @return
     */
    virtual NetMsg::Action action() const =0;


    quint64 getSize();

    virtual NetworkMessageHeader *  buffer() =0;
protected:
    /**
     * @brief buffer
     * @return
     */

protected:
    NetworkManager* m_server;
};
#endif
