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
#ifndef NOTIFICATIONZONE_H
#define NOTIFICATIONZONE_H

#include <QDockWidget>
#include "rwidgets_global.h"

class LogController;
namespace Ui
{
class NotificationZone;
}

class  RWIDGET_EXPORT  NotificationZone : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationZone(LogController* logCtrl, QWidget* parent= nullptr);
    ~NotificationZone();
    void initSetting();

public slots:
    void receiveData(quint64 readDate, quint64 size);

signals:
    void downloadVisibleChanged();

private:
    Ui::NotificationZone* ui;
};

#endif // NOTIFICATIONZONE_H
