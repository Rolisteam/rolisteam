#include "applicationcontroller.h"
#include "controller/gamecontroller.h"

ApplicationController::ApplicationController(GameController* ctrl,QObject* parent) : AbstractApplicationController{parent}, m_gameCtrl(ctrl)
{

}

void ApplicationController::msgToGM(const QString &msg)
{

}

void ApplicationController::msgToAll(const QString &msg)
{

}

void ApplicationController::rollDice(const QString &cmd, bool withAlias, bool gmOnly)
{

}

qreal ApplicationController::zoomLevel() const
{

}

void ApplicationController::setZoomLevel(qreal newZoomLevel)
{

}
