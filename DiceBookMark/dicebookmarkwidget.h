#ifndef DICEBOOKMARKWIDGET_H
#define DICEBOOKMARKWIDGET_H

#include <QWidget>
#include "dicebookmarkmodel.h"

namespace Ui {
class DiceBookMarkWidget;
}

class DiceBookMarkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DiceBookMarkWidget(QWidget *parent = 0);
    ~DiceBookMarkWidget();

private:
    Ui::DiceBookMarkWidget *ui;
    DiceBookMarkModel* m_model;
};

#endif // DICEBOOKMARKWIDGET_H
