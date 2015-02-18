#include "serverdialog.h"
#include "ui_serverdialog.h"
#include "preferencesmanager.h"

ServerDialog::ServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerDialog)
{
    ui->setupUi(this);
    m_options = PreferencesManager::getInstance();

    readPreferences();
}

ServerDialog::~ServerDialog()
{
    delete ui;
}

void ServerDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void ServerDialog::readPreferences()
{
    setPort(m_options->value("network/server/port",6660).toInt());
    enablePassword(m_options->value("network/server/haspassword",false).toBool());
    setPassword(m_options->value("network/server/password","").toString());
}
void ServerDialog::writePrefences()
{
    m_options->registerValue("network/server/port",getPort());
    m_options->registerValue("network/server/haspassword",hasPassword());
    m_options->registerValue("network/server/password",getPassword());


}
QString ServerDialog::getPassword()
{
    return ui->m_passwordEditline->text();
}

bool ServerDialog::hasPassword()
{
    return ui->m_passwordrequired->isChecked();
}
int ServerDialog::getPort()
{
    return ui->m_portSpin->value();
}
void ServerDialog::setPassword(QString p)
{
    ui->m_passwordEditline->setText(p);
}
void ServerDialog::enablePassword(bool p)
{
    ui->m_passwordrequired->setChecked(p);
}
void ServerDialog::setPort(int p)
{
    ui->m_portSpin->setValue(p);
}
