#include "shortcuteditordialog.h"
#include "ui_shortcuteditordialog.h"

ShortCutEditorDialog::ShortCutEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortCutEditorDialog)
{
    ui->setupUi(this);
}

ShortCutEditorDialog::~ShortCutEditorDialog()
{
    delete ui;
}
