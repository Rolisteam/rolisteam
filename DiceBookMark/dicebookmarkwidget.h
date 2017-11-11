#ifndef DICEBOOKMARKWIDGET_H
#define DICEBOOKMARKWIDGET_H

#include <QWidget>
#include <QSettings>
#include "dicebookmarkmodel.h"
#include "widgets/gmtoolbox/gamemastertool.h"

namespace Ui {
class DiceBookMarkWidget;
}

class DiceBookMarkWidget : public GameMasterTool
{
    Q_OBJECT

public:
    explicit DiceBookMarkWidget(QWidget *parent = 0);
    ~DiceBookMarkWidget();

    virtual void writeSettings(QSettings& settings) override;
    virtual void readSettings(QSettings& settings) override;


private:
    Ui::DiceBookMarkWidget *ui;
    DiceBookMarkModel* m_model;
};

#endif // DICEBOOKMARKWIDGET_H
