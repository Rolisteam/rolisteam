/***************************************************************************
 *   Copyright (C) 2022 by Renaud Guezennec                                *
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
#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>
#include <QRect>
#include <QString>
#include <memory>

#include "helper.h"
#include "services/tipchecker.h"

class TipCheckerTest : public QObject
{
    Q_OBJECT

public:
    TipCheckerTest();

private slots:
    void init();
    void cleanupTestCase();

    void doTest();

private:
    std::unique_ptr<TipChecker> m_tip;
};

TipCheckerTest::TipCheckerTest() {}

void TipCheckerTest::init()
{
    m_tip.reset(new TipChecker);
}

void TipCheckerTest::cleanupTestCase() {}

void TipCheckerTest::doTest()
{
    QSignalSpy spy(m_tip.get(), &TipChecker::checkFinished);
    m_tip->startChecking();

    spy.wait(1000);
    QCOMPARE(spy.count(), 1);

    QVERIFY(!m_tip->getArticleContent().isEmpty());
    QVERIFY(!m_tip->getArticleTitle().isEmpty());
    QVERIFY(m_tip->getUrl().isEmpty());
    QVERIFY(m_tip->hasArticle());

    auto id= Helper::generate<int>(0, 50000);
    m_tip->setId(id);
    QCOMPARE(m_tip->getId(), id);
}

QTEST_MAIN(TipCheckerTest);

#include "tst_tipchecker.moc"
