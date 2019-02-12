#ifndef LOGPANEL_H
#define LOGPANEL_H

#include "common/controller/logcontroller.h"
#include <QAbstractListModel>
#include <QWidget>
#include <preferencesmanager.h>

namespace Ui
{
    class LogPanel;
}

class LogPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LogPanel(LogController* controller, QWidget* parent= 0);
    ~LogPanel();

public slots:
    void showMessage(QString, LogController::LogLevel level);
    void initSetting();

protected slots:
    void saveLog();

private:
    Ui::LogPanel* ui;
    PreferencesManager* m_prefManager= PreferencesManager::getInstance();
    LogController* m_controller= nullptr;
};

#endif // LOGPANEL_H
