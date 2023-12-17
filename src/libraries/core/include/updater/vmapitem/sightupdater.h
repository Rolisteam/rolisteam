#ifndef SIGHTUPDATER_H
#define SIGHTUPDATER_H

#include "vmapitemcontrollerupdater.h"
#include <QObject>
#include <core_global.h>

class CORE_EXPORT SightUpdater : public VMapItemControllerUpdater
{
public:
    explicit SightUpdater(QObject* parent= nullptr);

    virtual ~SightUpdater() override= default;

    void addItemController(vmap::VisualItemController* ctrl, bool sendOff= true) override;

    bool updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl) override;
};

#endif // SIGHTUPDATER_H
