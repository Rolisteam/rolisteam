#include "keygeneratordialog.h"
#include "ui_keygeneratordialog.h"

#include <QCryptographicHash>

KeyGeneratorDialog::KeyGeneratorDialog(QWidget* parent) : QDialog(parent), ui(new Ui::KeyGeneratorDialog)
{
    ui->setupUi(this);
    connect(ui->m_passwordEdit, &QLineEdit::editingFinished, this, &KeyGeneratorDialog::generateKey);
    connect(ui->pushButton, &QPushButton::pressed, this, &KeyGeneratorDialog::generateKey);
}

KeyGeneratorDialog::~KeyGeneratorDialog()
{
    delete ui;
}

void KeyGeneratorDialog::generateKey()
{
    ui->m_resultEdit->clear();
    auto pw= ui->m_passwordEdit->text();
    auto pwBase64= QString::fromStdString(
        QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha3_512).toBase64().toStdString());
    ui->m_resultEdit->setPlainText(pwBase64);
}
