#include "updater/vmapitem/sightupdater.h"

#include "controller/item_controllers/sightcontroller.h"
#include "network/networkmessagereader.h"

#include <QDataStream>

SightUpdater::SightUpdater(QObject *parent)
    : VMapItemControllerUpdater{parent}
{

}

void SightUpdater::addItemController(vmap::VisualItemController *ctrl)
{
    if(nullptr == ctrl)
        return;

    auto sightCtrl= dynamic_cast<vmap::SightController*>(ctrl);

    if(nullptr == sightCtrl)
        return;

    VMapItemControllerUpdater::addItemController(sightCtrl);

    //connect(sightCtrl, &vmap::SightController::characterSightChanged, this,
    //        [this, sightCtrl]() { sendOffVMapChanges<bool>(sightCtrl, QStringLiteral("characterSight")); });
    connect(sightCtrl, &vmap::SightController::fowPathChanged, this,
            [this, sightCtrl]() { sendOffVMapChanges<QPainterPath>(sightCtrl, QStringLiteral("fowPath")); });
    connect(sightCtrl, &vmap::SightController::characterCountChanged, this,
            [this, sightCtrl]() { sendOffVMapChanges<int>(sightCtrl, QStringLiteral("characterCount")); });

    /*if(!ctrl->remote())
        connect(sightCtrl, &vmap::SightController::initializedChanged, this,
                [sightCtrl]() { MessageHelper::sendOffLine(sightCtrl, sightCtrl->mapUuid()); });*/
}

bool SightUpdater::updateItemProperty(NetworkMessageReader *msg, vmap::VisualItemController *ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    // TODO implement save/restore
    auto datapos= msg->pos();

    if(VMapItemControllerUpdater::updateItemProperty(msg, ctrl))
        return true;

    msg->resetToPos(datapos);

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;

    if(property == QStringLiteral("characterSight"))
    {
        var= static_cast<bool>(msg->uint8());
    }
    else if(property == QStringLiteral("fowPath"))
    {
        auto data= msg->byteArray32();
        {
            QDataStream read(&data, QIODevice::ReadOnly);
            QPainterPath path;
            read >> path;
            var = QVariant::fromValue(path);
        }
    }
    else if(property == QStringLiteral("characterCount"))
    {
        var= QVariant::fromValue(msg->int64());
    }
    else
    {
        return false;
    }

    m_updatingFromNetwork= true;
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}
