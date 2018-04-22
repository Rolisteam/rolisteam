#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QWidget>
 #include <QAbstractListModel>
#include "common/controller/logcontroller.h"
#include <preferencesmanager.h>

namespace Ui {
class LogPanel;
}

class LogPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LogPanel(QWidget* parent = 0);
    ~LogPanel();

public slots:
    void showMessage(QString,LogController::LogLevel level);
    void initSetting();

signals:
    void logLevelChanged(LogController::LogLevel i);
protected slots:
    void saveLog();
private:
    Ui::LogPanel *ui;
    PreferencesManager* m_prefManager = PreferencesManager::getInstance();
    LogController::LogLevel m_currentLogLevel;
};

#endif // LOGPANEL_H
