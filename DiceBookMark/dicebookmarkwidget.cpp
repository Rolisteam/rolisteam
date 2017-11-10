#include "dicebookmarkwidget.h"
#include "ui_dicebookmarkwidget.h"

DiceBookMarkWidget::DiceBookMarkWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DiceBookMarkWidget)
{
    ui->setupUi(this);
}

DiceBookMarkWidget::~DiceBookMarkWidget()
{
    delete ui;
}
