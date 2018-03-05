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
    explicit LogPanel(QWidget* parent = 0);
    ~LogPanel();

public slots:
protected slots:
    void saveLog();
private:
    Ui::LogPanel *ui;
};

#endif // LOGPANEL_H
