#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QWidget>

namespace Ui {
class LogPanel;
}

class LogPanel : public QWidget
{
    Q_OBJECT

public:
    enum Level {
        Error,
        Debug,
        Warning,
        Info
    };
    explicit LogPanel(QWidget* parent = 0);
    ~LogPanel();

public slots:
    void appendMessage(QString msg, Level lvl);
protected slots:
    void saveLog();
private:
    Ui::LogPanel *ui;
    Level m_currentLevel = LogPanel::Error;
};

#endif // LOGPANEL_H
