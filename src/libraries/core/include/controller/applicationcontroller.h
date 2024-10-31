#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H

#include "charactersheet/abstractapplicationcontroller.h"
#include <QObject>
#include <QPointer>
#include <core_global.h>

class GameController;

class CORE_EXPORT ApplicationController : public AbstractApplicationController
{
    Q_OBJECT
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged FINAL)
public:
    explicit ApplicationController(GameController* ctrl, QObject* parent= nullptr);
    void msgToGM(const QString& msg, const QString& characterId) override;
    void msgToAll(const QString& msg, const QString& characterId) override;
    void rollDice(const QString& cmd, const QString& characterId, bool gmOnly= false) override;
    qreal zoomLevel() const override;
    void setZoomLevel(qreal newZoomLevel) override;

    QString characterId() const override;
    void setCharacterId(const QString& newCharacterId) override;

signals:
    void zoomLevelChanged();

private:
    QPointer<GameController> m_gameCtrl;
    qreal m_zoomLevel{1.0};
};

#endif // APPLICATIONCONTROLLER_H
