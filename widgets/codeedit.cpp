#include "codeedit.h"
#include "ui_codeedit.h"

CodeEdit::CodeEdit(Field* m_field,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeEdit)
{
    Q_UNUSED(m_field);
    ui->setupUi(this);
}

CodeEdit::~CodeEdit()
{
    delete ui;
}
