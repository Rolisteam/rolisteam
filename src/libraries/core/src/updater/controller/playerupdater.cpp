#include "updater/controller/playerupdater.h"
#include "controller/gamecontroller.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "network/connectionprofile.h"
#include "worker/messagehelper.h"
#include "worker/playermessagehelper.h"

void addPlayerToModel(PlayerModel* model, NetworkMessageReader* msg)
{
    Player* player= new Player();
    PlayerMessageHelper::readPlayer(*msg, player);
    model->addPlayer(player);
}

PlayerUpdater::PlayerUpdater(NetworkController* network, PlayerController* ctrl, QObject* parent)
    : QObject{parent}, m_ctrl(ctrl), m_networkCtrl(network)
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
        case NetMsg::ChangeCharacterPropertyAct:
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
                {
                    auto local= m_ctrl->localPlayer();
                    PlayerMessageHelper::sendOffPlayerInformations(local);

                    connect(local, &Player::avatarChanged, m_networkCtrl,
                            [this, local]()
                            {
                                auto pro= m_networkCtrl->currentProfile();
                                if(!pro)
                                    return;
                                pro->setPlayerAvatar(local->avatar());
                            });
                    connect(local, &Player::nameChanged, m_networkCtrl,
                            [this, local]()
                            {
                                auto pro= m_networkCtrl->currentProfile();
                                if(!pro)
                                    return;
                                pro->setPlayerName(local->name());
                            });
                    connect(local, &Player::colorChanged, m_networkCtrl,
                            [this, local]()
                            {
                                auto pro= m_networkCtrl->currentProfile();
                                if(!pro)
                                    return;
                                pro->setPlayerColor(local->getColor());
                            });

                    auto updateCharacters= [this, local]()
                    {
                        auto pro= m_networkCtrl->currentProfile();
                        if(!pro)
                            return;
                        pro->setPlayerColor(local->getColor());

                        auto const& characters= local->children();
                        std::vector<connection::CharacterData> data;
                        data.reserve(characters.size());

                        std::transform(
                            std::begin(characters), std::end(characters), std::back_inserter(data),
                            [](const std::unique_ptr<Character>& character)
                            {
                                QHash<QString, QVariant> params;
                                params.insert(Core::updater::key_char_property_hp, character->getHealthPointsCurrent());
                                params.insert(Core::updater::key_char_property_maxhp, character->getHealthPointsMax());
                                params.insert(Core::updater::key_char_property_minhp, character->getHealthPointsMin());
                                params.insert(Core::updater::key_char_property_dist, character->getDistancePerTurn());
                                params.insert(Core::updater::key_char_property_state_id, character->stateId());
                                params.insert(Core::updater::key_char_property_life_color, character->getLifeColor());
                                params.insert(Core::updater::key_char_property_init_cmd, character->initCommand());
                                params.insert(Core::updater::key_char_property_has_init, character->hasInitScore());
                                params.insert(Core::updater::key_char_property_init_score,
                                              character->getInitiativeScore());
                                return connection::CharacterData({character->uuid(), character->name(),
                                                                  character->getColor(), character->avatar(), params});
                            });

                        pro->setCharacters(data);
                    };

                    connect(local, &Player::characterChanged, m_networkCtrl, updateCharacters);
                    connect(local, &Player::characterCountChanged, m_networkCtrl, updateCharacters);
                }
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
        connect(p, &Character::hasInitScoreChanged, this,
                [this, p]() { sendOffChanges<bool>(p, true, Core::person::hasInitiative); });
        connect(p, &Character::initiativeChanged, this,
                [this, p]() { sendOffChanges<int>(p, true, Core::person::initiative); });
        connect(p, &Character::stateIdChanged, this,
                [this, p]() { sendOffChanges<QString>(p, true, Core::person::stateId); });
        connect(p, &Character::lifeColorChanged, this,
                [this, p]() { sendOffChanges<QColor>(p, true, Core::person::lifeColor); });
    }
}

void PlayerUpdater::playerLeft(Player* player)
{
    disconnect(player, 0, this, 0);
}
