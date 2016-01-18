#include "convertor.h"
#include "ui_convertor.h"

Convertor::Convertor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Convertor)
{
    ui->setupUi(this);
}

Convertor::~Convertor()
{
    delete ui;
}
