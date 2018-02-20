#ifndef DICEBOOKMARKWIDGET_H
#define DICEBOOKMARKWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QSettings>
#include "dicebookmarkmodel.h"
#include "widgets/gmtoolbox/gamemastertool.h"

namespace Ui {
class DiceBookMarkWidget;
}

class DiceBookMarkWidget : public QDialog
{
    Q_OBJECT

public:
    explicit DiceBookMarkWidget(std::vector<std::pair<QString,QString>>& data,QWidget *parent = 0);
    ~DiceBookMarkWidget();

    virtual void writeSettings(QSettings& settings);
    virtual void readSettings(QSettings& settings);


private:
    Ui::DiceBookMarkWidget *ui;
    DiceBookMarkModel* m_model;
};

#endif // DICEBOOKMARKWIDGET_H
