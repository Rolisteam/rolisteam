#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QAbstractListModel>
#include <QPointer>
#include <QWidget>
#include <common/logcontroller.h>
#include <common_widgets/common_widgets_global.h>

namespace Ui
{
class LogPanel;
}
class COMMON_WIDGET_EXPORT LogPanel : public QWidget
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
    QPointer<LogController> m_controller;
};

#endif // LOGPANEL_H
