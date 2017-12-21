#include "codeeditordialog.h"
#include "ui_codeeditordialog.h"

CodeEditorDialog::CodeEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeEditorDialog)
{
    ui->setupUi(this);
}

CodeEditorDialog::~CodeEditorDialog()
{
    delete ui;
}
