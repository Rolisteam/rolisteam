/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "notificationzone.h"
#include "ui_notificationzone.h"

#include "common/logcontroller.h"

NotificationZone::NotificationZone(LogController* logCtrl, QWidget* parent)
    : QDockWidget(parent), ui(new Ui::NotificationZone)
{
    ui->setupUi(this);
    ui->m_logPanel->setController(logCtrl);
}

NotificationZone::~NotificationZone()
{
    delete ui;
}

void NotificationZone::receiveData(quint64 readDate, quint64 size)
{
    if(size == 0)
        ui->m_progessBar->setVisible(false);
    else if(readDate != size)
    {
        ui->m_progessBar->setVisible(true);
        int i= static_cast<int>((size - readDate) * 100 / size);
        ui->m_progessBar->setValue(i);
    }
}
void NotificationZone::initSetting()
{
    ui->m_logPanel->initSetting();
}
