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
void CodeEditorDialog::setPlainText(const QString data)
{
    ui->m_codeEditor->setPlainText(data);
}
QString CodeEditorDialog::toPlainText()
{
    return ui->m_codeEditor->toPlainText();
}
