#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QWidget>
#include "logcontroller.h"

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
    void showMessage(QString);

signals:
    void logLevelChanged(LogController::LogLevel i);
protected slots:
    void saveLog();
private:
    Ui::LogPanel *ui;
};

#endif // LOGPANEL_H
