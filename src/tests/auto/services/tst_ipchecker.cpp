/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#include <QTest>
#include <QSignalSpy>

#include <QImage>
#include <QPainter>
#include <QRect>
#include <QString>
#include <memory>

#include "services/ipchecker.h"

class IpCheckerTest : public QObject
{
    Q_OBJECT

public:
    IpCheckerTest();

private slots:
    void init();
    void cleanupTestCase();

    void doTest();

private:
    std::unique_ptr<IpChecker> m_ipCheck;
};

IpCheckerTest::IpCheckerTest() {}

void IpCheckerTest::init()
{
    m_ipCheck.reset(new IpChecker);
}

void IpCheckerTest::cleanupTestCase() {}

void IpCheckerTest::doTest()
{
    QSignalSpy spy(m_ipCheck.get(), &IpChecker::finishedCheck);
    m_ipCheck->startCheck();

    spy.wait(1000);

    QCOMPARE(spy.count(), 1);

    QVERIFY(!m_ipCheck->ipAddress().isEmpty());
}

QTEST_MAIN(IpCheckerTest);

#include "tst_ipchecker.moc"
