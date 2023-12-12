#include "network/messagedispatcher.h"
#include "network/networkmessagereader.h"

#include "network/channel.h"

MessageDispatcher::MessageDispatcher(QObject* parent) : QObject(parent) {}

void MessageDispatcher::dispatchMessage(QByteArray data, Channel* channel, ServerConnection* emitter)
{
    if(data.isEmpty())
        return;

    bool sendToAll= true;
    bool saveIt= true;
    NetworkMessageReader* msg= new NetworkMessageReader();

    msg->setData(data);

    if(channel == nullptr && msg->category() != NetMsg::AdministrationCategory)
    {
        qWarning() << "####\nchannel is nullptr\n####";
    }

    if(emitter)
        qInfo() << "[Server][Received Message]" << cat2String(msg->header()) << act2String(msg->header()) << channel
                << emitter->name() << msg->getSize();

    if(msg->category() == NetMsg::AdministrationCategory)
    {
        emit messageForAdmin(msg, channel, emitter);
        sendToAll= false;
        if((NetMsg::ClearTable == msg->action()) || (NetMsg::AddChannel == msg->action()))
        {
            sendToAll= true;
            saveIt= false;
        }
    }
    else if(msg->category() == NetMsg::UserCategory)
    {
        if(msg->action() == NetMsg::PlayerConnectionAction)
        {
            emitter->setInfoPlayer(msg);
            saveIt= false;
            sendToAll= false;
        }
        else if(msg->action() == NetMsg::DelPlayerAction)
        {
            saveIt= false;
            sendToAll= false;
            if(channel != nullptr)
                channel->removeClient(emitter);
        }

    }
    else if(msg->category() == NetMsg::PlayerCharacterCategory)
    {
        if(msg->action() == NetMsg::ChangePlayerPropertyAct)
        {
            auto uuid= msg->string8();
            auto property= msg->string16();
            if(property == "name")
            {
                auto name= msg->string16();

                if(channel != nullptr)
                {
                    QMetaObject::invokeMethod(channel, "renamePlayer", Qt::QueuedConnection, Q_ARG(QString, uuid),
                                              Q_ARG(QString, name));
                }
            }
        }
    }
    else if(msg->category() == NetMsg::SetupCategory)
    {
        saveIt= false;
    }

    if((sendToAll) && (nullptr != channel))
    {
        channel->sendMessage(msg, emitter, saveIt);
    }
}

QString MessageDispatcher::cat2String(NetworkMessageHeader* head)
{
    QString str;
    NetMsg::Category cat= NetMsg::Category(head->category);
    switch(cat)
    {
    case NetMsg::AdministrationCategory:
        str= QStringLiteral("AdministrationCategory");
        break;
    case NetMsg::UserCategory:
        str= QStringLiteral("UserCategory");
        break;
    case NetMsg::PlayerCharacterCategory:
        str= QStringLiteral("PlayerCharacterCategory");
        break;
    case NetMsg::CharacterSheetCategory:
        str= QStringLiteral("CharacterSheetCategory");
        break;
    case NetMsg::InstantMessageCategory:
        str= QStringLiteral("InstantMessageCategory");
        break;
    case NetMsg::MusicCategory:
        str= QStringLiteral("MusicCategory");
        break;
    case NetMsg::SetupCategory:
        str= QStringLiteral("SetupCategory");
        break;
    case NetMsg::CampaignCategory:
        str= QStringLiteral("CampaignCategory");
        break;
    case NetMsg::VMapCategory:
        str= QStringLiteral("VMapCategory");
        break;
    case NetMsg::MediaCategory:
        str= QStringLiteral("MediaCategory");
        break;
    case NetMsg::SharedNoteCategory:
        str= QStringLiteral("SharedNoteCategory");
        break;
    case NetMsg::WebPageCategory:
        str= QStringLiteral("WebPageCategory");
        break;
    case NetMsg::MindMapCategory:
        str= QStringLiteral("MindMapCategory");
        break;
    default:
        str= QStringLiteral("UnknownCategory");
        break;
    }
    return str;
}

QString MessageDispatcher::act2String(NetworkMessageHeader* head)
{
    QString str("Unknown Action");
    NetMsg::Action act= NetMsg::Action(head->action);
    NetMsg::Category cat= NetMsg::Category(head->category);
    if(cat == NetMsg::AdministrationCategory)
    {
        switch(act)
        {
        case NetMsg::EndConnectionAction:
            str= QStringLiteral("EndConnectionAction");
            break;
        case NetMsg::Heartbeat:
            str= QStringLiteral("Heartbeat");
            break;
        case NetMsg::ConnectionInfo:
            str= QStringLiteral("ConnectionInfo");
            break;
        case NetMsg::Goodbye:
            str= QStringLiteral("Goodbye");
            break;
        case NetMsg::Kicked:
            str= QStringLiteral("Kicked");
            break;
        case NetMsg::RenameChannel:
            str= QStringLiteral("RenameChannel");
            break;
        case NetMsg::MoveChannel:
            str= QStringLiteral("MoveChannel");
            break;
        case NetMsg::NeedPassword:
            str= QStringLiteral("NeedPassword");
            break;
        case NetMsg::SetChannelList:
            str= QStringLiteral("SetChannelList");
            break;
        case NetMsg::ChannelPassword:
            str= QStringLiteral("ChannelPassword");
            break;
        case NetMsg::JoinChannel:
            str= QStringLiteral("JoinChannel");
            break;
        case NetMsg::ClearTable:
            str= QStringLiteral("ClearTable");
            break;
        case NetMsg::AddChannel:
            str= QStringLiteral("AddChannel");
            break;
        case NetMsg::DeleteChannel:
            str= QStringLiteral("DeleteChannel");
            break;
        case NetMsg::AuthentificationSucessed:
            str= QStringLiteral("AuthentificationSucessed");
            break;
        case NetMsg::AuthentificationFail:
            str= QStringLiteral("AuthentificationFail");
            break;
        case NetMsg::LockChannel:
            str= QStringLiteral("LockChannel");
            break;
        case NetMsg::UnlockChannel:
            str= QStringLiteral("UnlockChannel");
            break;
        case NetMsg::BanUser:
            str= QStringLiteral("BanUser");
            break;
        case NetMsg::AdminPassword:
            str= QStringLiteral("AdminPassword");
            break;
        case NetMsg::AdminAuthSucessed:
            str= QStringLiteral("AdminAuthSucessed");
            break;
        case NetMsg::AdminAuthFail:
            str= QStringLiteral("AdminAuthFail");
            break;
        case NetMsg::MovedIntoChannel:
            str= QStringLiteral("MovedIntoChannel");
            break;
        case NetMsg::GMStatus:
            str= QStringLiteral("GMStatus");
            break;
        case NetMsg::ResetChannel:
            str= QStringLiteral("ResetChannel");
            break;
        case NetMsg::ResetChannelPassword:
            str= QStringLiteral("ResetChannelPassword");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::UserCategory)
    {
        switch(act)
        {
        case NetMsg::PlayerConnectionAction:
            str= QStringLiteral("PlayerConnectionAction");
            break;
        case NetMsg::DelPlayerAction:
            str= QStringLiteral("DelPlayerAction");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::PlayerCharacterCategory)
    {
        switch(act)
        {
        case NetMsg::AddCharacterToPlayerAct:
            str= QStringLiteral("AddCharacterToPlayerAct");
            break;
        case NetMsg::RemoveCharacterToPlayerAct:
            str= QStringLiteral("RemoveCharacterToPlayerAct");
            break;
        case NetMsg::ChangePlayerPropertyAct:
            str= QStringLiteral("ChangePlayerPropertyAct");
            break;
        case NetMsg::ChangeCharacterPropertyAct:
            str= QStringLiteral("ChangeCharacterPropertyAct");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::CharacterSheetCategory)
    {
        switch(act)
        {
        case NetMsg::addCharacterSheet:
            str= QStringLiteral("addCharacterList");
            break;
        case NetMsg::updateFieldCharacterSheet:
            str= QStringLiteral("moveCharacter");
            break;
        case NetMsg::closeCharacterSheet:
            str= QStringLiteral("changeCharacterState");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::InstantMessageCategory)
    {
        switch(act)
        {
        case NetMsg::InstantMessageAction:
            str= QStringLiteral("InstantMessageAction");
            break;
        case NetMsg::UpdateChatAction:
            str= QStringLiteral("UpdateChatAction");
            break;
        case NetMsg::RemoveChatroomAction:
            str= QStringLiteral("RemoveChatroomAction");
            break;
        case NetMsg::AddChatroomAction:
            str= QStringLiteral("AddChatroomAction");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::MusicCategory)
    {
        switch(act)
        {
        case NetMsg::StopSong:
            str= QStringLiteral("StopSong");
            break;
        case NetMsg::PlaySong:
            str= QStringLiteral("PlaySong");
            break;
        case NetMsg::PauseSong:
            str= QStringLiteral("PauseSong");
            break;
        case NetMsg::NewSong:
            str= QStringLiteral("NewSong");
            break;
        case NetMsg::ChangePositionSong:
            str= QStringLiteral("ChangePositionSong");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::SetupCategory)
    {
        switch(act)
        {
        case NetMsg::AddFeatureAction:
            str= QStringLiteral("AddFeatureAction");
            break;

        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::CampaignCategory)
    {
        switch(act)
        {
        case NetMsg::addDiceAlias:
            str= QStringLiteral("addDiceAlias");
            break;
        case NetMsg::moveDiceAlias:
            str= QStringLiteral("moveDiceAlias");
            break;
        case NetMsg::removeDiceAlias:
            str= QStringLiteral("removeDiceAlias");
            break;
        case NetMsg::addCharacterState:
            str= QStringLiteral("addCharacterState");
            break;
        case NetMsg::moveCharacterState:
            str= QStringLiteral("moveCharacterState");
            break;
        case NetMsg::removeCharacterState:
            str= QStringLiteral("removeCharacterState");
            break;
        case NetMsg::CharactereStateModel:
            str= QStringLiteral("CharactereStateModel");
            break;
        case NetMsg::DiceAliasModel:
            str= QStringLiteral("DiceAliasModel");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::VMapCategory)
    {
        switch(act)
        {
        case NetMsg::UpdateItem:
            str= QStringLiteral("UpdateItem");
            break;
        case NetMsg::AddItem:
            str= QStringLiteral("AddItem");
            break;
        case NetMsg::DeleteItem:
            str= QStringLiteral("DeleteItem");
            break;
        case NetMsg::DeletePoint:
            str= QStringLiteral("DeletePoint");
            break;
        case NetMsg::AddPoint:
            str= QStringLiteral("AddPoint");
            break;
        case NetMsg::CharacterVisionChanged:
            str= QStringLiteral("CharacterVisionChanged");
            break;
        case NetMsg::CharacterStateChanged:
            str= QStringLiteral("CharacterStateChanged");
            break;
        case NetMsg::CharacterChanged:
            str= QStringLiteral("CharacterChanged");
            break;
        case NetMsg::SetParentItem:
            str= QStringLiteral("SetParentItem");
            break;
        case NetMsg::MovePoint:
            str= QStringLiteral("MovePoint");
            break;
        case NetMsg::HighLightPosition:
            str= QStringLiteral("HighLightPosition");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::MediaCategory)
    {
        switch(act)
        {
        case NetMsg::AddMedia:
            str= QStringLiteral("AddMedia");
            break;
        case NetMsg::UpdateMediaProperty:
            str= QStringLiteral("UpdateMediaProperty");
            break;
        case NetMsg::CloseMedia:
            str= QStringLiteral("CloseMedia");
            break;
        case NetMsg::AddSubImage:
            str= QStringLiteral("AddSubImage");
            break;
        case NetMsg::RemoveSubImage:
            str= QStringLiteral("RemoveSubImage");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::SharedNoteCategory)
    {
        switch(act)
        {
        case NetMsg::updateText:
            str= QStringLiteral("updateText");
            break;
        case NetMsg::updateTextAndPermission:
            str= QStringLiteral("updateTextAndPermission");
            break;
        case NetMsg::updatePermissionOneUser:
            str= QStringLiteral("updatePermissionOneUser");
            break;

        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::WebPageCategory)
    {
        switch(act)
        {
        case NetMsg::UpdateContent:
            str= QStringLiteral("UpdateContent");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::MindMapCategory)
    {
        switch(act)
        {
        case NetMsg::AddMessage:
            str= QStringLiteral("AddMessage");
            break;
        case NetMsg::RemoveMessage:
            str= QStringLiteral("RemoveMessage");
            break;
        case NetMsg::UpdateNode:
            str= QStringLiteral("UpdateNode");
            break;
        case NetMsg::UpdateLink:
            str= QStringLiteral("UpdateLink");
            break;
        case NetMsg::UpdateMindMapPermission:
            str= QStringLiteral("UpdateMindMapPermission");
            break;
        case NetMsg::UpdatePackage:
            str= QStringLiteral("UpdatePackage");
            break;
        default:
            str= QStringLiteral("Unknown Action");
            break;
        }
    }
    return str;
}
