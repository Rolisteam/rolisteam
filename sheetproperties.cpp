#include "sheetproperties.h"
#include "ui_sheetproperties.h"

SheetProperties::SheetProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SheetProperties)
{
    ui->setupUi(this);
}

SheetProperties::~SheetProperties()
{
    delete ui;
}
