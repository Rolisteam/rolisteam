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
#include "utils/countdownobject.h"

CountDownObject::CountDownObject(int tryCount,int countDown, int timeBeforeDecrease, QObject* parent)
    : QObject(parent), m_timer(new QTimer), m_countDown(countDown), m_tryCount(tryCount)
{
    m_timer->setInterval(timeBeforeDecrease);
    init();
    connect(m_timer.get(), &QTimer::timeout, this, [this, countDown]() {
        --m_allDown;

        auto rest = (m_allDown % countDown);
        emit countDownChanged(rest);
        if(rest == 0)
        {
            emit triggered(m_allDown / countDown);
        }
        if(m_allDown<=0)
        {
            stop();
        }
    });
}

bool CountDownObject::isRunning() const
{
    return m_running;
}

void CountDownObject::setRunning(bool b)
{
    if(b == m_running)
        return;
    m_running = b;
    emit runningChanged();
}

void CountDownObject::start()
{
    setRunning(true);
    m_timer->start();
}
void CountDownObject::pause()
{
    setRunning(false);
    m_timer->stop();
}
void CountDownObject::resume()
{
    start();
}

void CountDownObject::stop()
{
    setRunning(false);
    m_timer->stop();
    init();
}

void CountDownObject::init()
{
    m_allDown = m_countDown * m_tryCount;
}
