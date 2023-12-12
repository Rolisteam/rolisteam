#ifndef PLAYERUPDATER_H
#define PLAYERUPDATER_H

#include <QObject>
#include <QPointer>

#include "network/networkreceiver.h"
#include "network/networkmessagewriter.h"
#include "worker/convertionhelper.h"
#include "data/person.h"
#include <core_global.h>

class PlayerController;
class GameController;
class Player;
class Person;
class Character;
class CORE_EXPORT PlayerUpdater : public QObject, public NetWorkReceiver
{
    Q_OBJECT
public:
    explicit PlayerUpdater(PlayerController* ctrl, QObject *parent = nullptr);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);
    void setGameController(GameController*);



public slots:
    void updateNewPlayer(Player* player);
    void playerLeft(Player* player);


private:
    template <typename T>
    void sendOffChanges(Person* person, bool isCharacter, const QString& property);


private:
    QPointer<PlayerController> m_ctrl;
    bool m_updating{false};
};

template <typename T>
void PlayerUpdater::sendOffChanges(Person* person, bool isCharacter,const QString& property)
{
    if(nullptr == person || m_updating)
        return;

    NetworkMessageWriter msg(NetMsg::PlayerCharacterCategory, isCharacter ? NetMsg::ChangeCharacterPropertyAct : NetMsg::ChangePlayerPropertyAct);
    msg.string8(person->uuid());
    msg.string16(property);
    auto val= person->property(property.toLocal8Bit().data());
    Helper::variantToType<T>(val.value<T>(), msg);
    msg.sendToServer();
}

#endif // PLAYERUPDATER_H
