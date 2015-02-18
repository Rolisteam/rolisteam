/***************************************************************************
    *   Copyright (C) 2011 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    *   This program is distributed in the hope that it will be useful,       *
    *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
    *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
    *   GNU General Public License for more details.                          *
    *                                                                         *
    *   You should have received a copy of the GNU General Public License     *
    *   along with this program; if not, write to the                         *
    *   Free Software Foundation, Inc.,                                       *
    *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
    ***************************************************************************/

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
