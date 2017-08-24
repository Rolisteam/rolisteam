#include "columndefinitiondialog.h"
#include "ui_columndefinitiondialog.h"

ColumnDefinitionDialog::ColumnDefinitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnDefinitionDialog)
{
    ui->setupUi(this);
}

ColumnDefinitionDialog::~ColumnDefinitionDialog()
{
    delete ui;
}
