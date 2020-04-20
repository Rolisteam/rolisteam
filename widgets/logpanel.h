#ifndef LOGPANEL_H
#define LOGPANEL_H

#include "common/controller/logcontroller.h"
#include "preferences/preferencesmanager.h"
#include <QAbstractListModel>
#include <QPointer>
#include <QWidget>

namespace Ui
{
class LogPanel;
}

class LogPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LogPanel(QWidget* parent= nullptr);
    void setController(LogController* controller);
    ~LogPanel();

public slots:
    void showMessage(QString, LogController::LogLevel level);
    void initSetting();

protected slots:
    void saveLog();

private:
    Ui::LogPanel* ui;
    PreferencesManager* m_prefManager= PreferencesManager::getInstance();
    QPointer<LogController> m_controller;
};

#endif // LOGPANEL_H
