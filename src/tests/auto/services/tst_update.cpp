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

#include "services/updatechecker.h"

class UpdateTest : public QObject
{
    Q_OBJECT

public:
    UpdateTest();

private slots:
    void init();
    void cleanupTestCase();

    void doTest();

private:
    std::unique_ptr<UpdateChecker> m_update;
};

UpdateTest::UpdateTest() {}

void UpdateTest::init()
{
    m_update.reset(new UpdateChecker("v1.0.0"));
}

void UpdateTest::cleanupTestCase() {}

void UpdateTest::doTest()
{
    QSignalSpy spy(m_update.get(), &UpdateChecker::checkFinished);
    m_update->startChecking();

    spy.wait(1000);
    QCOMPARE(spy.count(), 1);

    QVERIFY(m_update->needUpdate());
    QVERIFY(!m_update->getLatestVersion().isEmpty());
    QVERIFY(!m_update->getLatestVersionDate().isEmpty());

    m_update->startChecking();

    spy.wait(1000);
}

QTEST_MAIN(UpdateTest);

#include "tst_update.moc"
