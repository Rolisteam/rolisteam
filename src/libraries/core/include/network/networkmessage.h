/*************************************************************************
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   Copyright (C) 2015 by Renaud Guezennec                              *
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

#ifndef NETWORK_MESSAGE_H
#define NETWORK_MESSAGE_H

#include "network_global.h"
#include <QStringList>
#include <QtGlobal>
//
class NetworkLink;
class ClientManager;
struct NETWORK_EXPORT NetworkMessageHeader
{
    quint8 category;
    quint8 action;
    quint32 dataSize;
};
namespace NetMsg
{
enum Category
{
    AdministrationCategory,
    UserCategory,
    PlayerCharacterCategory,
    CharacterSheetCategory,
    InstantMessageCategory,
    MusicCategory,
    CampaignCategory,
    VMapCategory,
    MediaCategory,
    SharedNoteCategory,
    WebPageCategory,
    MindMapCategory,
    Dice3DCategory
};

enum Action
{
    // AdministrationCategory
    EndConnectionAction= 0,
    HeartbeatAsk,
    HeartbeatAnswer,
    ConnectionInfo,
    Goodbye,
    Kicked,
    MoveChannel,
    SetChannelList,
    RenameChannel,
    NeedPassword,
    AuthentificationSucessed,
    AuthentificationFail,
    LockChannel,
    UnlockChannel,
    JoinChannel,
    DeleteChannel,
    AddChannel,
    ChannelPassword,
    ResetChannelPassword,
    BanUser,
    ClearTable,
    AdminPassword,
    AdminAuthSucessed,
    AdminAuthFail,
    MovedIntoChannel,
    GMStatus,
    ResetChannel,

    // UserCategory
    PlayerConnectionAction= 0,
    DelPlayerAction,

    // Player and Character Category
    AddCharacterToPlayerAct= 0,
    RemoveCharacterToPlayerAct,
    ChangePlayerPropertyAct,
    ChangeCharacterPropertyAct,

    // CharacterSheetCategory
    addCharacterSheet= 0,
    updateFieldCharacterSheet,
    updateCellSheet,
    closeCharacterSheet,

    // InstantMessageCategory
    InstantMessageAction= 0,
    AddChatroomAction,
    RemoveChatroomAction,
    UpdateChatAction,

    // MusicCategory
    StopSong= 0,
    PlaySong,
    PauseSong,
    NewSong,
    ChangePositionSong,

    // CampaignCategory
    addDiceAlias= 0,
    moveDiceAlias,
    removeDiceAlias,
    addCharacterState,
    moveCharacterState,
    removeCharacterState,
    DiceAliasModel,
    CharactereStateModel,

    // Vmap
    AddItem= 0,
    DeleteItem,
    UpdateItem,
    AddPoint,
    DeletePoint,
    MovePoint,
    SetParentItem,
    CharacterVisionChanged,
    CharacterStateChanged,
    CharacterChanged,
    HighLightPosition,

    // mediacategory
    AddMedia= 0,
    UpdateMediaProperty,
    CloseMedia,
    AddSubImage,
    RemoveSubImage,

    // SharedNoteCategory
    updateTextAndPermission= 0,
    updateText,
    updatePermissionOneUser,

    // WebPage
    UpdateContent= 0,

    // Mindmap
    AddNodes= 0,
    RemoveNode,
    UpdateNode,
    UpdatePackage,
    UpdateLink,
    UpdateMindMapPermission,
    AddChildToPackage,
    RemoveChildFromPackage,

    // 3d dice
    Roll3DAct= 0
};
} // namespace NetMsg

class NetworkMessage;
class NETWORK_EXPORT MessageSenderInterface
{
public:
    virtual ~MessageSenderInterface();
    virtual void sendMessage(const NetworkMessage* msg)= 0;
};

/**
 * @brief The NetworkMessage class is pure virtual class to manage network message.
 */
class NETWORK_EXPORT NetworkMessage
{
public:
    enum RecipientMode
    {
        All,
        OneOrMany
    };
    explicit NetworkMessage();
    virtual ~NetworkMessage();
    virtual void sendToServer();
    virtual NetMsg::Category category() const= 0;
    virtual NetMsg::Action action() const= 0;
    void setLinkToServer(NetworkLink* server);
    virtual NetworkMessage::RecipientMode getRecipientMode() const= 0;
    virtual QStringList getRecipientList() const= 0;
    quint64 getSize() const;
    virtual NetworkMessageHeader* buffer() const= 0;

    static void setMessageSender(MessageSenderInterface*);

protected:
    static MessageSenderInterface* m_sender;
};
#endif
