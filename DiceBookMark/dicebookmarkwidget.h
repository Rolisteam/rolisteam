#ifndef DICEBOOKMARKWIDGET_H
#define DICEBOOKMARKWIDGET_H

#include <QDockWidget>

namespace Ui {
class DiceBookMarkWidget;
}

class DiceBookMarkWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DiceBookMarkWidget(QWidget *parent = 0);
    ~DiceBookMarkWidget();

private:
    Ui::DiceBookMarkWidget *ui;
};

#endif // DICEBOOKMARKWIDGET_H
