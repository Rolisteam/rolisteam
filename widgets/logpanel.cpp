#include "logpanel.h"
#include "ui_logpanel.h"

LogPanel::LogPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogPanel)
{
    ui->setupUi(this);
}

LogPanel::~LogPanel()
{
    delete ui;
}
