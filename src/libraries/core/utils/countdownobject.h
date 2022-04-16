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
#include <memory>

class CountDownObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int tryCount READ tryCount CONSTANT)
    Q_PROPERTY(int interval READ interval CONSTANT)
    Q_PROPERTY(int countDown READ countDown NOTIFY countDownChanged)
public:
    explicit CountDownObject(int tryCount, int interval, QObject* parent= nullptr);

    int tryCount() const;
    int interval() const;
    int countDown() const;
public slots:
    void start();
    void stop();

signals:
    void triggered();
    void countDownChanged();

private slots:
    void setCountDown(int);

private:
    std::unique_ptr<QTimer> m_timer;
    int m_interval= 1000;
    int m_tryCount= 0;
    int m_currentTry= 0;
    int m_countDown= 0;
    bool m_running= true;
};

#endif // COUNTDOWNOBJECT_H
