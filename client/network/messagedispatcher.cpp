#include "messagedispatcher.h"
#include "network/networkmessagereader.h"

MessageDispatcher::MessageDispatcher(QObject *parent) : QObject(parent)
{

}

void MessageDispatcher::dispatchMessage(QByteArray data, Channel* channel, TcpClient* emitter)
{
    bool sendToAll = true;
    bool saveIt = true;
    NetworkMessageReader* msg = new NetworkMessageReader();

    msg->setData(data);

    qDebug() << "[Server][Received Message]" <<cat2String(msg->header()) << act2String(msg->header()) << channel << emitter->getName() << msg->getSize();

    if(msg->category()== NetMsg::AdministrationCategory)
    {
        emit messageForAdmin(msg,channel,emitter);
        sendToAll = false;
        if((NetMsg::ClearTable == msg->action()) || (NetMsg::AddChannel == msg->action()))
        {
            sendToAll = true;
            saveIt = false;
        }
    }
    else if(msg->category()== NetMsg::PlayerCategory)
    {
        if(msg->action() == NetMsg::PlayerConnectionAction)
        {
            emitter->setInfoPlayer(msg);

            msg->resetToData();

            QString name = msg->string16();
            QString uuid = msg->string8();
            msg->rgb();
            msg->uint8();
            emitter->setName(name);
            emitter->setId(uuid);
            saveIt = false;
        }
        else if(msg->action() == NetMsg::DelPlayerAction)
        {
            saveIt = false;
            sendToAll = false;
            if(channel != nullptr)
                channel->removeClient(emitter);
        }
    }
    else if(msg->category() == NetMsg::SetupCategory)
    {
        if(msg->action() == NetMsg::AddFeatureAction)
        {
            QString uuid   = msg->string8();
            QString name   = msg->string8();
            quint8 version = msg->uint8();
            emitter->addPlayerFeature(uuid,name,version);
            saveIt = false;
        }
    }

    if((sendToAll)&&(nullptr != channel))
    {
        channel->sendMessage(msg,emitter,saveIt);
    }

}

QString MessageDispatcher::cat2String(NetworkMessageHeader* head)
{
    QString str;
    NetMsg::Category cat = NetMsg::Category(head->category);
    switch (cat)
    {
    case NetMsg::AdministrationCategory:
        str = QStringLiteral("AdministrationCategory");
        break;
    case NetMsg::PlayerCategory:
        str = QStringLiteral("PlayerCategory");
        break;
    case NetMsg::CharacterPlayerCategory:
        str = QStringLiteral("CharacterPlayerCategory");
        break;
    case NetMsg::NPCCategory:
        str = QStringLiteral("NPCCategory");
        break;
    case NetMsg::CharacterCategory:
        str = QStringLiteral("CharacterCategory");
        break;
    case NetMsg::DrawCategory:
        str = QStringLiteral("DrawCategory");
        break;
    case NetMsg::MapCategory:
        str = QStringLiteral("MapCategory");
        break;
    case NetMsg::ChatCategory:
        str = QStringLiteral("ChatCategory");
        break;
    case NetMsg::MusicCategory:
        str = QStringLiteral("MusicCategory");
        break;
    case NetMsg::SetupCategory:
        str = QStringLiteral("SetupCategory");
        break;
    case NetMsg::SharePreferencesCategory:
        str = QStringLiteral("SharePreferencesCategory");
        break;
    case NetMsg::VMapCategory:
        str = QStringLiteral("VMapCategory");
        break;
    case NetMsg::MediaCategory:
        str = QStringLiteral("MediaCategory");
        break;
    case NetMsg::SharedNoteCategory:
        str = QStringLiteral("SharedNoteCategory");
        break;

    }
    return str;
}

QString MessageDispatcher::act2String(NetworkMessageHeader* head)
{
    QString str;
    NetMsg::Action act = NetMsg::Action(head->action);
    NetMsg::Category cat = NetMsg::Category(head->category);
    if(cat == NetMsg::AdministrationCategory)
    {
        switch (act)
        {
        case  NetMsg::EndConnectionAction:
            str = QStringLiteral("EndConnectionAction");
            break;
        case  NetMsg::Heartbeat:
            str = QStringLiteral("heartbeat");
            break;
        case  NetMsg::ConnectionInfo:
            str = QStringLiteral("ConnectionInfo");
            break;
        case  NetMsg::Goodbye:
            str = QStringLiteral("Goodbye");
            break;
        case  NetMsg::Kicked:
            str = QStringLiteral("Kicked");
            break;
        case  NetMsg::MoveChannel:
            str = QStringLiteral("MoveChannel");
            break;
        case  NetMsg::SetChannelList:
            str = QStringLiteral("SetChannelList");
            break;
        case  NetMsg::ChannelPassword:
            str = QStringLiteral("ChannelPassword");
            break;
        case  NetMsg::JoinChannel:
            str = QStringLiteral("Join Channel");
            break;
        case  NetMsg::ClearTable:
            str = QStringLiteral("Clear Table");
            break;
        case  NetMsg::AddChannel:
            str = QStringLiteral("AddChannel");
            break;
        case  NetMsg::DeleteChannel:
            str = QStringLiteral("DeleteChannel");
            break;
        case  NetMsg::AuthentificationSucessed:
            str = QStringLiteral("AuthentificationSucessed");
            break;
        case  NetMsg::AuthentificationFail:
            str = QStringLiteral("AuthentificationFail");
            break;
        case  NetMsg::LockChannel:
            str = QStringLiteral("LockChannel");
            break;
        case  NetMsg::BanUser:
            str = QStringLiteral("BanUser");
            break;
        case  NetMsg::AdminPassword:
            str = QStringLiteral("AdminPassword");
            break;
        case  NetMsg::AdminAuthSucessed:
            str = QStringLiteral("AdminPassword");
            break;
        case  NetMsg::AdminAuthFail:
            str = QStringLiteral("AdminPassword");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::PlayerCategory)
    {
        switch (act)
        {
        case  NetMsg::PlayerConnectionAction:
            str = QStringLiteral("PlayerConnectionAction");
            break;
        case  NetMsg::DelPlayerAction:
            str = QStringLiteral("DelPlayerAction");
            break;
        case  NetMsg::ChangePlayerNameAction:
            str = QStringLiteral("ChangePlayerNameAction");
            break;
        case  NetMsg::ChangePlayerColorAction:
            str = QStringLiteral("ChangePlayerColorAction");
            break;
        case  NetMsg::ChangePlayerAvatarAction:
            str = QStringLiteral("ChangePlayerAvatarAction");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::CharacterPlayerCategory)
    {
        switch (act)
        {
        case  NetMsg::AddPlayerCharacterAction:
            str = QStringLiteral("AddPlayerCharacterAction");
            break;
        case  NetMsg::DelPlayerCharacterAction:
            str = QStringLiteral("DelPlayerCharacterAction");
            break;
        case  NetMsg::ToggleViewPlayerCharacterAction:
            str = QStringLiteral("ToggleViewPlayerCharacterAction");
            break;
        case  NetMsg::ChangePlayerCharacterSizeAction:
            str = QStringLiteral("ChangePlayerCharacterSizeAction");
            break;
        case  NetMsg::ChangePlayerCharacterNameAction:
            str = QStringLiteral("ChangePlayerCharacterNameAction");
            break;
        case  NetMsg::ChangePlayerCharacterColorAction:
            str = QStringLiteral("ChangePlayerCharacterColorAction");
            break;
        case  NetMsg::ChangePlayerCharacterAvatarAction:
            str = QStringLiteral("ChangePlayerCharacterAvatarAction");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::NPCCategory)
    {
        switch (act)
        {
        case  NetMsg::addNpc:
            str = QStringLiteral("addNpc");
            break;
        case  NetMsg::delNpc:
            str = QStringLiteral("delNpc");
            break;

        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::CharacterCategory)
    {
        switch (act)
        {
        case  NetMsg::addCharacterList:
            str = QStringLiteral("addCharacterList");
            break;
        case  NetMsg::moveCharacter:
            str = QStringLiteral("moveCharacter");
            break;
        case  NetMsg::changeCharacterState:
            str = QStringLiteral("changeCharacterState");
            break;
        case  NetMsg::changeCharacterOrientation:
            str = QStringLiteral("changeCharacterOrientation");
            break;
        case  NetMsg::showCharecterOrientation:
            str = QStringLiteral("showCharecterOrientation");
            break;
        case  NetMsg::addCharacterSheet:
            str = QStringLiteral("addCharacterSheet");
            break;
        case  NetMsg::updateFieldCharacterSheet:
            str = QStringLiteral("updateFieldCharacterSheet");
            break;
        case  NetMsg::closeCharacterSheet:
            str = QStringLiteral("closeCharacterSheet");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::DrawCategory)
    {
        switch (act)
        {
        case  NetMsg::penPainting:
            str = QStringLiteral("penPainting");
            break;
        case  NetMsg::linePainting:
            str = QStringLiteral("linePainting");
            break;
        case  NetMsg::emptyRectanglePainting:
            str = QStringLiteral("emptyRectanglePainting");
            break;
        case  NetMsg::filledRectanglePainting:
            str = QStringLiteral("filledRectanglePainting");
            break;
        case  NetMsg::emptyEllipsePainting:
            str = QStringLiteral("emptyEllipsePainting");
            break;
        case  NetMsg::filledEllipsePainting:
            str = QStringLiteral("filledEllipsePainting");
            break;
        case  NetMsg::textPainting:
            str = QStringLiteral("textPainting");
            break;
        case  NetMsg::handPainting:
            str = QStringLiteral("handPainting");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::MapCategory)
    {
        switch (act)
        {
        case  NetMsg::AddEmptyMap:
            str = QStringLiteral("AddEmptyMap");
            break;
        case  NetMsg::LoadMap:
            str = QStringLiteral("LoadMap");
            break;
        case  NetMsg::ImportMap:
            str = QStringLiteral("ImportMap");
            break;
        case  NetMsg::CloseMap:
            str = QStringLiteral("CloseMap");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::ChatCategory)
    {
        switch (act)
        {
        case  NetMsg::ChatMessageAction:
            str = QStringLiteral("ChatMessageAction");
            break;
        case  NetMsg::DiceMessageAction:
            str = QStringLiteral("DiceMessageAction");
            break;
        case  NetMsg::EmoteMessageAction:
            str = QStringLiteral("EmoteMessageAction");
            break;
        case  NetMsg::UpdateChatAction:
            str = QStringLiteral("UpdateChatAction");
            break;
        case  NetMsg::DelChatAction:
            str = QStringLiteral("DelChatAction");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::MusicCategory)
    {
        switch (act)
        {
        case  NetMsg::StopSong:
            str = QStringLiteral("StopSong");
            break;
        case  NetMsg::PlaySong:
            str = QStringLiteral("PlaySong");
            break;
        case  NetMsg::PauseSong:
            str = QStringLiteral("PauseSong");
            break;
        case  NetMsg::NewSong:
            str = QStringLiteral("NewSong");
            break;
        case  NetMsg::ChangePositionSong:
            str = QStringLiteral("ChangePositionSong");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::SetupCategory)
    {
        switch (act)
        {
        case  NetMsg::AddFeatureAction:
            str = QStringLiteral("AddFeatureAction");
            break;

        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::SharePreferencesCategory)
    {
        switch (act)
        {
        case  NetMsg::addDiceAlias:
            str = QStringLiteral("addDiceAlias");
            break;
        case  NetMsg::moveDiceAlias:
            str = QStringLiteral("moveDiceAlias");
            break;
        case  NetMsg::removeDiceAlias:
            str = QStringLiteral("removeDiceAlias");
            break;
        case  NetMsg::addState:
            str = QStringLiteral("addState");
            break;
        case  NetMsg::moveState:
            str = QStringLiteral("moveState");
            break;
        case  NetMsg::removeState:
            str = QStringLiteral("removeState");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::VMapCategory)
    {
        switch (act)
        {
        case  NetMsg::addVmap:
            str = QStringLiteral("addVmap");
            break;
        case  NetMsg::vmapChanges:
            str = QStringLiteral("vmapChanges");
            break;
        case  NetMsg::loadVmap:
            str = QStringLiteral("loadVmap");
            break;
        case  NetMsg::closeVmap:
            str = QStringLiteral("closeVmap");
            break;
        case  NetMsg::addItem:
            str = QStringLiteral("addItem");
            break;
        case  NetMsg::DelItem:
            str = QStringLiteral("DelItem");
            break;
        case  NetMsg::MoveItem:
            str = QStringLiteral("MoveItem");
            break;
        case  NetMsg::ZValueItem:
            str = QStringLiteral("ZValueItem");
            break;
        case  NetMsg::RotationItem:
            str = QStringLiteral("RotationItem");
            break;
        case  NetMsg::RectGeometryItem:
            str = QStringLiteral("RectGeometryItem");
            break;
        case  NetMsg::DelPoint:
            str = QStringLiteral("DelPoint");
            break;
        case  NetMsg::OpacityItemChanged:
            str = QStringLiteral("OpacityItemChanged");
            break;
        case  NetMsg::LayerItemChanged:
            str = QStringLiteral("LayerItemChanged");
            break;
        case  NetMsg::GeometryItemChanged:
            str = QStringLiteral("GeometryItemChanged");
            break;
        case  NetMsg::AddPoint:
            str = QStringLiteral("AddPoint");
            break;
        case  NetMsg::GeometryViewChanged:
            str = QStringLiteral("GeometryViewChanged");
            break;
        case  NetMsg::characterStateChanged:
            str = QStringLiteral("characterStateChanged");
            break;
        case  NetMsg::SetParentItem:
            str = QStringLiteral("SetParentItem");
            break;
        case  NetMsg::MovePoint:
            str = QStringLiteral("MovePoint");
            break;
        case  NetMsg::VisionChanged:
            str = QStringLiteral("VisionChanged");
            break;
        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::MediaCategory)
    {

        switch (act)
        {
        case  NetMsg::addMedia:
            str = QStringLiteral("AddMedia");
            break;
        case  NetMsg::closeMedia:
            str = QStringLiteral("closeMedia");
            break;

        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }
    else if(cat == NetMsg::SharedNoteCategory)
    {

        switch (act)
        {
        case  NetMsg::updateText:
            str = QStringLiteral("Update Text");
            break;
        case  NetMsg::updateTextAndPermission:
            str = QStringLiteral("Update Text and permission");
            break;
        case  NetMsg::updatePermissionOneUser:
            str = QStringLiteral("Update permission on user");
            break;

        default:
            str = QStringLiteral("Unknown Action");
            break;
        }
    }

    return str;
}
