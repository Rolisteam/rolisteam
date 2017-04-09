/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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

#include "connectionretrydialog.h"
#include "ui_connectionretrydialog.h"

#define DEFAULT_TIMEOUT 11
#define DEFAULT_INTERVAL 1000

ConnectionRetryDialog::ConnectionRetryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionRetryDialog)
{
    ui->setupUi(this);
    m_timer=new QTimer();
    m_counter = DEFAULT_TIMEOUT;
    m_timeoutValue = DEFAULT_INTERVAL;
    m_timer->setInterval(m_timeoutValue);
    m_msg =tr("Connection has failed! Connection Retry in %1s.");
    ui->m_label->setText(m_msg.arg(m_counter));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(decreaseCounter()));
    connect(this,SIGNAL(rejected()),m_timer,SLOT(stop()));
}

ConnectionRetryDialog::~ConnectionRetryDialog()
{
    delete ui;
    delete m_timer;
}
void ConnectionRetryDialog::resetCounter()
{
    m_counter = DEFAULT_TIMEOUT;
}

void ConnectionRetryDialog::startTimer()
{
    m_timer->start();
}
void ConnectionRetryDialog::setTimeOut(int timeout)
{
    m_timeoutValue=timeout;
    m_timer->setInterval(m_timeoutValue);
}
void  ConnectionRetryDialog::decreaseCounter()
{
    --m_counter;
    ui->m_label->setText(m_msg.arg(m_counter));
    if(0>=m_counter)
    {
        //emit tryConnection();
        accept();
        resetCounter();
    }

}
