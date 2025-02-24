#ifndef DICEBOOKMARKWIDGET_H
#define DICEBOOKMARKWIDGET_H

#include "data/diceshortcut.h"
#include "dicebookmarkmodel.h"
#include "rwidgets_global.h"
#include <QDialog>
#include <QSettings>
#include <QWidget>
namespace Ui
{
class DiceBookMarkWidget;
}

class RWIDGET_EXPORT DiceBookMarkWidget : public QDialog
{
    Q_OBJECT

public:
    explicit DiceBookMarkWidget(std::vector<DiceShortCut>& data, QWidget* parent= 0);
    ~DiceBookMarkWidget();

    virtual void writeSettings();
    virtual void readSettings();

private:
    Ui::DiceBookMarkWidget* ui;
    DiceBookMarkModel* m_model;
};

#endif // DICEBOOKMARKWIDGET_H
