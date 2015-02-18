/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#include "connectionwizzard.h"
#include "ui_connectionwizzard.h"

#include <QListWidget>
#include <QDebug>

#include "preferencesmanager.h"
#include "connection.h"

ConnectionWizzard::ConnectionWizzard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWizzard)
{
    ui->setupUi(this);
    m_options = PreferencesManager::getInstance();



    QVariant tmp2;
    tmp2.setValue(ConnectionList());
    QVariant tmp = m_options->value("network/connectionsList",tmp2);
    m_connectionList = tmp.value< ConnectionList >();

    connect(ui->m_addButton,SIGNAL(clicked()),this,SLOT(addNewConnection()));
    connect(ui->m_deleteButton,SIGNAL(clicked()),this,SLOT(removeConnection()));
    connect(ui->m_connectionListWidget,SIGNAL(currentRowChanged(int)),this,SLOT(selectionChanged()));
    connect(ui->m_name,SIGNAL(editingFinished()),this,SLOT(editionFinished()));
    connect(ui->buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(onApply(QAbstractButton*)));

    updateList();
}

ConnectionWizzard::~ConnectionWizzard()
{
    delete ui;
}

void ConnectionWizzard::addNewConnection()
{
    m_currentConnection= new Connection();
    m_currentConnection->setName(tr("Unknown"));


    m_currentRow = ui->m_connectionListWidget->count();
    m_connectionList.insert(m_currentRow,*m_currentConnection);
    ui->m_connectionListWidget->insertItem(m_currentRow,m_currentConnection->getName());
    ui->m_connectionListWidget->setCurrentRow(m_currentRow);
    m_currentRow = ui->m_connectionListWidget->currentRow();


    //modification of GUI
    ui->m_name->setText(m_currentConnection->getName());
    ui->m_name->setFocus();
    ui->m_port->setValue(m_options->value("network/defaultPort",6660).toInt());
    m_currentConnection->setPort(m_options->value("network/defaultPort",6660).toInt());

}
void ConnectionWizzard::removeConnection()
{
    m_connectionList.removeAt(ui->m_connectionListWidget->currentRow());
    delete ui->m_connectionListWidget->currentItem();
    ui->m_name->setText("");
    ui->m_hostname->setText("");
    ui->m_port->setValue(m_options->value("network/defaultPort",6660).toInt());

    selectionChanged();
}
void ConnectionWizzard::selectionChanged()
{
    m_currentRow = ui->m_connectionListWidget->currentRow();
    m_currentConnection = &m_connectionList[m_currentRow];
    ui->m_name->setText(m_currentConnection->getName());
    ui->m_hostname->setText(m_currentConnection->getAddress());
    ui->m_port->setValue(m_currentConnection->getPort());
}
void ConnectionWizzard::editionFinished()
{
    //m_currentConnection->setName(ui->m_name->text());
    ui->m_connectionListWidget->currentItem()->setText(ui->m_name->text());
}
void ConnectionWizzard::onApply(QAbstractButton* tmpbutton)
{
    if(ui->buttonBox->buttonRole(tmpbutton)==QDialogButtonBox::ApplyRole)
    {
        m_currentConnection->setName(ui->m_name->text());
        m_currentConnection->setAddress(ui->m_hostname->text());
        m_currentConnection->setPort(ui->m_port->value());
        m_options->registerValue("network/defaultPort",ui->m_port->value(),false);
        QVariant tmp2;
        tmp2.setValue(m_connectionList);
        qDebug() << "ecriture=" << m_connectionList.size();
        m_options->registerValue("network/connectionsList",tmp2,true);
    }
}
void ConnectionWizzard::updateList()
{
    foreach(Connection tmp, m_connectionList)
    {
         ui->m_connectionListWidget->insertItem(ui->m_connectionListWidget->count(),tmp.getName());
    }
}

void ConnectionWizzard::changeEvent(QEvent *e)
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
