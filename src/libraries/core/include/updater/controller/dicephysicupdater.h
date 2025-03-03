#ifndef DICEPHYSICUPDATER_H
#define DICEPHYSICUPDATER_H

#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <core_global.h>

#include "dicephysics/controllers/dice3dcontroller.h"
#include "network/networkreceiver.h"

class CORE_EXPORT DicePhysicUpdater : public QObject, public NetWorkReceiver
{
    Q_OBJECT
public:
    DicePhysicUpdater(Dice3DController* ctrl, QObject* parent= nullptr);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

private:
    QPointer<Dice3DController> m_ctrl;
};

#endif // DICEPHYSICUPDATER_H
