#include "applicationcontroller.h"
#include "common/logcategory.h"
#include "controller/gamecontroller.h"
#include "controller/instantmessagingcontroller.h"

ApplicationController::ApplicationController(GameController* ctrl, QObject* parent)
    : AbstractApplicationController{parent}, m_gameCtrl(ctrl)
{
}

void ApplicationController::msgToGM(const QString& msg)
{
    auto im= m_gameCtrl->instantMessagingController();
    if(!im)
    {
        qCWarning(MessagingCat) << "Invalid Instant Messaging Controller";
        return;
    }
    im->sendMessageToGlobal(msg, m_characterId);
}

void ApplicationController::msgToAll(const QString& msg)
{
    auto im= m_gameCtrl->instantMessagingController();
    if(!im)
    {
        qCWarning(MessagingCat) << "Invalid Instant Messaging Controller";
        return;
    }
    im->sendMessageToGlobal(msg, m_characterId);
}

void ApplicationController::rollDice(const QString& cmd, bool gmOnly)
{
    auto im= m_gameCtrl->instantMessagingController();
    if(!im)
    {
        qCWarning(MessagingCat) << "Invalid Instant Messaging Controller";
        return;
    }
    im->rollDiceCommand(cmd, gmOnly, m_characterId);
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

QString ApplicationController::characterId() const
{
    return m_characterId;
}

void ApplicationController::setCharacterId(const QString& newCharacterId)
{
    if(newCharacterId == m_characterId)
        return;
    m_characterId= newCharacterId;
    emit characterIdChanged();
}
