#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include <QObject>
#include "charactersheet/abstractapplicationcontroller.h"
#include <core_global.h>
#include <QPointer>

class GameController;

class CORE_EXPORT ApplicationController : public AbstractApplicationController
{
    Q_OBJECT
public:
    explicit ApplicationController(GameController* ctrl, QObject* parent= nullptr);
    void msgToGM(const QString& msg) override;
    void msgToAll(const QString& msg) override;
    void rollDice(const QString& cmd, bool withAlias, bool gmOnly= false) override;
    qreal zoomLevel() const override;
    void setZoomLevel(qreal newZoomLevel) override;
signals:

private:
    QPointer<GameController> m_gameCtrl;
};

#endif // APPLICATIONCONTROLLER_H
