#include "aboutrcse.h"
#include "ui_aboutrcse.h"

AboutRcse::AboutRcse(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutRcse)
{
    ui->setupUi(this);
}

AboutRcse::~AboutRcse()
{
    delete ui;
}
