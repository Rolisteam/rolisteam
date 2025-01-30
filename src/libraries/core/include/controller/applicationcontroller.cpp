#include "applicationcontroller.h"
#include "common/logcategory.h"
#include "controller/gamecontroller.h"
#include "controller/instantmessagingcontroller.h"

ApplicationController::ApplicationController(GameController* ctrl, QObject* parent)
    : AbstractApplicationController{parent}, m_gameCtrl(ctrl)
{
}

void ApplicationController::msgToGM(const QString& msg, const QString& characterId)
{
    auto im= m_gameCtrl->instantMessagingController();
    if(!im)
    {
        qCWarning(MessagingCat) << "Invalid Instant Messaging Controller";
        return;
    }
    im->sendMessageToGlobal(msg, characterId);
}

void ApplicationController::msgToAll(const QString& msg, const QString& characterId)
{
    auto im= m_gameCtrl->instantMessagingController();
    if(!im)
    {
        qCWarning(MessagingCat) << "Invalid Instant Messaging Controller";
        return;
    }
    im->sendMessageToGlobal(msg, characterId);
}

void ApplicationController::rollDice(const QString& cmd, const QString& characterId, bool gmOnly)
{
    auto im= m_gameCtrl->instantMessagingController();
    if(!im)
    {
        qCWarning(MessagingCat) << "Invalid Instant Messaging Controller";
        return;
    }
    im->rollDiceCommand(cmd, gmOnly, characterId);
}

qreal ApplicationController::zoomLevel() const
{
    return m_zoomLevel;
}

void ApplicationController::setZoomLevel(qreal newZoomLevel)
{
    if(qFuzzyCompare(newZoomLevel, m_zoomLevel))
        return;
    m_zoomLevel= newZoomLevel;
    emit zoomLevelChanged();
}
