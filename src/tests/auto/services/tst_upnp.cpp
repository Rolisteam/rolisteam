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

#include "network/upnp/upnpnat.h"

class UpnpTest : public QObject
{
    Q_OBJECT

public:
    UpnpTest();

private slots:
    void init();
    void cleanupTestCase();

    void doTest();

private:
    std::unique_ptr<UpnpNat> m_upnat;
};

UpnpTest::UpnpTest() {}

void UpnpTest::init()
{
    m_upnat.reset(new UpnpNat);
}

void UpnpTest::cleanupTestCase() {}

void UpnpTest::doTest()
{
    m_upnat->setSubnet(QHostAddress("192.168.1.2"));
    m_upnat->setMask(24);
    m_upnat->setSubnet(QHostAddress("192.168.1.2"));
    m_upnat->setMask(24);
    m_upnat->setSubnet(QHostAddress());
    m_upnat->setMask(24);
    m_upnat->init(5, 10);
    m_upnat->init(5, 10);

    QSignalSpy discoveryEnd(m_upnat.get(), &UpnpNat::discoveryEnd);
    QSignalSpy mappingSpy(m_upnat.get(), &UpnpNat::portMappingEnd);

    connect(m_upnat.get(), &UpnpNat::discoveryEnd, this,
            [this](bool b)
            {
                if(b)
                {
                    m_upnat->addPortMapping("upnpRolisteam", m_upnat->localIp(), 6664, 6664, "TCP");
                }
            });

    connect(m_upnat.get(), &UpnpNat::statusChanged, this,
            [this]()
            {
                if(m_upnat->status() == UpnpNat::NAT_STAT::NAT_ADD)
                {
                    qDebug() << "It worked!";
                }
            });

    connect(m_upnat.get(), &UpnpNat::lastErrorChanged, this,
            [this]() { qDebug() << " Error:" << m_upnat->lastError(); });

    m_upnat->discovery();

    discoveryEnd.wait(10000);
    if(discoveryEnd.count() == 0)
        return;
    QCOMPARE(discoveryEnd.count(), 1);
    QCOMPARE(m_upnat->lastError(), QString());

    mappingSpy.wait(1000);
    QCOMPARE(mappingSpy.count(), 1);
    QCOMPARE(m_upnat->status(), UpnpNat::NAT_STAT::NAT_ADD);

    m_upnat->setLastError("Error test");
    QCOMPARE(m_upnat->status(), UpnpNat::NAT_STAT::NAT_ERROR);
    m_upnat->setLastError("Error test");
    m_upnat->setLastError("Error test2");

    m_upnat->discovery();
    discoveryEnd.wait(10000);
}

QTEST_MAIN(UpnpTest);

#include "tst_upnp.moc"
