#include "updater/controller/dicephysicupdater.h"

#include "network/networkmessagewriter.h"
#include "network/receiveevent.h"

DicePhysicUpdater::DicePhysicUpdater(Dice3DController* ctrl, QObject* parent) : QObject(parent), m_ctrl(ctrl)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::Dice3DCategory, this);

    auto model= m_ctrl->model();
    connect(model, &DiceModel::diceRollChanged, this,
            [this](const QByteArray& data)
            {
                if(!m_ctrl->sharedOnline())
                    return;
                NetworkMessageWriter msg(NetMsg::Dice3DCategory, NetMsg::Roll3DAct);
                msg.byteArray32(data);
                msg.sendToServer();
            });
}

NetWorkReceiver::SendType DicePhysicUpdater::processMessage(NetworkMessageReader* msg)
{
    if(checkAction(msg, NetMsg::Dice3DCategory, NetMsg::Roll3DAct))
    {
        auto model= m_ctrl->model();
        model->setDiceData(msg->byteArray32());
    }
    return NetWorkReceiver::SendType::NONE;
}
