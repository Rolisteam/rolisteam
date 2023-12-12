#include "updater/controller/playerupdater.h"
#include "controller/gamecontroller.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "worker/messagehelper.h"
#include "worker/playermessagehelper.h"

void addPlayerToModel(PlayerModel* model, NetworkMessageReader* msg)
{
    Player* player= new Player();
    PlayerMessageHelper::readPlayer(*msg, player);
    model->addPlayer(player);
}

PlayerUpdater::PlayerUpdater(PlayerController* ctrl, QObject* parent) : QObject{parent}, m_ctrl(ctrl)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::UserCategory, this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::PlayerCharacterCategory, this);
    auto model= m_ctrl->model();

    connect(model, &PlayerModel::playerJoin, this, &PlayerUpdater::updateNewPlayer);
    connect(model, &PlayerModel::playerLeft, this, &PlayerUpdater::playerLeft);
}

NetWorkReceiver::SendType PlayerUpdater::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::AllExceptSender;
    auto model= m_ctrl->model();
    if(msg->category() == NetMsg::UserCategory)
    {
        switch(msg->action())
        {
        case NetMsg::PlayerConnectionAction:
            addPlayerToModel(model, msg);
            break;
        case NetMsg::DelPlayerAction:
            model->removePlayer(model->playerById(MessageHelper::readPlayerId(*msg)));
            break;
        default:
            break;
        }
    }
    else if(msg->category() == NetMsg::PlayerCharacterCategory)
    {
        switch(msg->action())
        {

        case NetMsg::ChangePlayerPropertyAct:
            MessageHelper::updatePerson(*msg, model);
            break;
        default:
            break;
        }
    }
    return type;
}

void PlayerUpdater::setGameController(GameController* gameCtrl)
{
    connect(gameCtrl, &GameController::connectedChanged, this,
            [this](bool b)
            {
                if(b)
                    PlayerMessageHelper::sendOffPlayerInformations(m_ctrl->localPlayer());
                else
                    m_ctrl->clear();
            });
}

void PlayerUpdater::updateNewPlayer(Player* player)
{
    if(!m_ctrl->localIsGm() && player != m_ctrl->localPlayer())
        return;

    connect(player, &Player::avatarChanged, this,
            [this, player]() { sendOffChanges<QByteArray>(player, false, Core::person::avatar); });
    connect(player, &Player::nameChanged, this,
            [this, player]() { sendOffChanges<QString>(player, false, Core::person::name); });
    connect(player, &Player::colorChanged, this,
            [this, player]() { sendOffChanges<QColor>(player, false, Core::person::color); });

    auto const& characters= player->children();

    for(auto const& character : characters)
    {
        auto p= character.get();
        connect(p, &Character::avatarChanged, this,
                [this, p]() { sendOffChanges<QByteArray>(p, true, Core::person::avatar); });
        connect(p, &Character::nameChanged, this,
                [this, p]() { sendOffChanges<QString>(p, true, Core::person::name); });
        connect(p, &Character::colorChanged, this,
                [this, p]() { sendOffChanges<QColor>(p, true, Core::person::color); });
        connect(p, &Character::currentHealthPointsChanged, this,
                [this, p]() { sendOffChanges<int>(p, true, Core::person::healthPoints); });
        connect(p, &Character::npcChanged, this, [this, p]() { sendOffChanges<bool>(p, true, Core::person::isNpc); });
        connect(p, &Character::maxHPChanged, this, [this, p]() { sendOffChanges<int>(p, true, Core::person::maxHP); });
        connect(p, &Character::minHPChanged, this, [this, p]() { sendOffChanges<int>(p, true, Core::person::minHP); });
        connect(p, &Character::distancePerTurnChanged, this,
                [this, p]() { sendOffChanges<int>(p, true, Core::person::distancePerTurn); });

        connect(p, &Character::initCommandChanged, this,
                [this, p]() { sendOffChanges<QString>(p, true, Core::person::initCommand); });
        connect(p, &Character::initiativeChanged, this,
                [this, p]() { sendOffChanges<int>(p, true, Core::person::initiative); });
        connect(p, &Character::stateIdChanged, this,
                [this, p]() { sendOffChanges<QString>(p, true, Core::person::stateId); });
    }
}

void PlayerUpdater::playerLeft(Player* player)
{
    disconnect(player, 0, this, 0);
}
