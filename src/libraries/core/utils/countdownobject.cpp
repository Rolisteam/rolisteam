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
#include "countdownobject.h"

constexpr int oneSecond= 1000;
CountDownObject::CountDownObject(int tryCount, int interval, QObject* parent)
    : QObject(parent), m_timer(new QTimer), m_interval(interval), m_tryCount(tryCount)
{
    m_timer->setInterval(oneSecond);
    connect(m_timer.get(), &QTimer::timeout, this, [this]() { setCountDown(countDown() - oneSecond); });
}

int CountDownObject::tryCount() const
{
    return m_tryCount;
}

int CountDownObject::interval() const
{
    return m_interval;
}

int CountDownObject::countDown() const
{
    return m_countDown;
}

void CountDownObject::setCountDown(int dc)
{
    if(m_countDown == dc)
        return;
    m_countDown= dc;
    emit countDownChanged();
    if(m_countDown == 0)
        m_timer->stop();
}

void CountDownObject::start()
{
    if(m_currentTry == m_tryCount || !m_running)
    {
        m_running= true;
        return;
    }

    emit triggered();
    ++m_currentTry;
    setCountDown(m_interval);
    QTimer::singleShot(m_interval, this, &CountDownObject::start);
}

void CountDownObject::stop()
{
    m_timer->stop();
    m_running= false;
}
