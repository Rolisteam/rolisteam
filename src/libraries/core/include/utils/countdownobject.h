/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#ifndef COUNTDOWNOBJECT_H
#define COUNTDOWNOBJECT_H

#include <QObject>
#include <QTimer>
#include <core_global.h>
#include <memory>
class  CORE_EXPORT CountDownObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning  NOTIFY runningChanged)
public:
    explicit CountDownObject(int tryCount = 5, int countDown = 10, int timeBeforeDecrease = 1000, QObject* parent= nullptr);

    bool isRunning() const;

public slots:
    void start();
    void stop();
    void pause();
    void resume();

signals:
    void triggered(int remaingTries);
    void countDownChanged(int c);
    void runningChanged();

private slots:
    void setRunning(bool);
    void init();

private:
    std::unique_ptr<QTimer> m_timer;
    int m_allDown= 0;
    int m_countDown= 10;
    int m_tryCount= 5;
    bool m_running= false;
};

#endif // COUNTDOWNOBJECT_H
