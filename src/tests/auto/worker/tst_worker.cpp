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
#include <QtTest/QtTest>

#include <QImage>
#include <QPainter>
#include <QRect>
#include <QString>
#include <memory>

#include "utils/countdownobject.h"
#include "utils/iohelper.h"
#include "utils/mappinghelper.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"
#include <helper.h>

class WorkerTest : public QObject
{
    Q_OBJECT

public:
    WorkerTest();

private slots:
    void init();
    void cleanupTestCase();
    void helperUtilsTest();
    void helperUtilsTest_data();

    void imageIsSquare();
    void imageIsSquare_data();

    void mappingHelperTest();
    void iohelperTest();

    void countDownTest();
};

WorkerTest::WorkerTest() {}

void WorkerTest::init() {}

void WorkerTest::cleanupTestCase() {}

void WorkerTest::helperUtilsTest()
{
    QFETCH(QRect, rect);
    QFETCH(qreal, ratio);

    auto newRect= helper::utils::computerBiggerRectInside(rect, ratio);

    QVERIFY(newRect.width() <= rect.width());
    QVERIFY(newRect.height() <= rect.height());

    QVERIFY(qFuzzyCompare(static_cast<qreal>(newRect.width()) / newRect.height(), ratio));
}

void WorkerTest::helperUtilsTest_data()
{
    QTest::addColumn<QRect>("rect");
    QTest::addColumn<qreal>("ratio");

    QVector<qreal> ratios({1., 16. / 9., 4. / 3.});
    long long int idx= 0;

    constexpr int k_width= 100;  // 1920
    constexpr int k_height= 100; // 1080

    // QTest::addRow("%s", QString("cmd %1").arg(idx++).toStdString().c_str()) << QRect(0, 0, 16, 10) << ratios[2];

    for(int i= 10; i <= k_width; ++i)
    {
        for(int j= 10; j <= k_height; ++j)
        {
            for(auto r : qAsConst(ratios))
            {
                QTest::addRow("%s", QString("cmd %1").arg(idx++).toStdString().c_str()) << QRect(0, 0, i, j) << r;
            }
        }
    }
}

void WorkerTest::imageIsSquare()
{
    QFETCH(QByteArray, image);
    QFETCH(bool, expected);
    QCOMPARE(helper::utils::isSquareImage(image), expected);
}
void WorkerTest::imageIsSquare_data()
{
    QTest::addColumn<QByteArray>("image");
    QTest::addColumn<bool>("expected");

    QTest::addRow("cmd1") << IOHelper::imageToData(IOHelper::readImageFromFile(":/img/arbre_500.jpg")) << false;
    QTest::addRow("cmd2") << IOHelper::imageToData(IOHelper::readImageFromFile(":/img/arbre_square_500.jpg")) << true;
}

void WorkerTest::iohelperTest()
{
    QTemporaryDir dir;

    if(dir.isValid())
    {
        utils::IOHelper::makeDir(dir.path());
        utils::IOHelper::copyFile(":/img/arbre_500.jpg", dir.path());
        utils::IOHelper::removeFile(QString("%1/arbre_500.jpg").arg(dir.path()));
        auto src= utils::IOHelper::copyFile(":/img/arbre_500.jpg", dir.path());
        utils::IOHelper::readPixmapFromURL(QString("%1/arbre_500.jpg").arg(dir.path()));
        utils::IOHelper::shortNameFromPath(QString("%1/arbre_500.jpg").arg(dir.path()));
        auto data= utils::IOHelper::readTextFile(":/sharednotes/scenario.md");
        QVERIFY(!data.isEmpty());
        /*QVERIFY(!utils::IOHelper::moveFile(QStringLiteral("%1/%2").arg(QDir::rootPath(), Helper::randomString()),
                                           QStringLiteral("%1/%2").arg(QDir::rootPath(), Helper::randomString())));*/

        QVERIFY(utils::IOHelper::moveFile(QStringLiteral("%1/arbre_500.jpg").arg(dir.path()),
                                          QStringLiteral("%1/image_test.jpg").arg(dir.path())));

        QVERIFY(!utils::IOHelper::writeFile(QString("%1/%2").arg(QDir::rootPath(), Helper::randomString()),
                                            Helper::imageData()));
    }
}

void WorkerTest::mappingHelperTest()
{
    utils::MappingHelper helper;

    auto res= helper.mapSizeTo(QSizeF(100., 100.), QSizeF(400., 200.));
    QCOMPARE(res, QSizeF(100., 50.));

    auto max= helper.maxRect(QRectF(0., 0., 500., 500.), QRectF(1000., 400., 10., 10.));
    QCOMPARE(max, QRectF(0, 0, 1010., 500.));

    helper.mapRectInto(QRectF(0., 0., 500., 500.), QRectF(0., 0., 500., 500.), QRectF(0., 0., 500., 500.));
}

void WorkerTest::countDownTest()
{
    auto tryCount= Helper::generate<int>(1, 5);
    auto intervalCount= Helper::generate<int>(1, 10);

    CountDownObject obj(tryCount, intervalCount, 100);//100 ms

    QSignalSpy spy(&obj, &CountDownObject::triggered);
    QSignalSpy spy2(&obj, &CountDownObject::countDownChanged);

    QVERIFY(!obj.isRunning());

    obj.start();
    QVERIFY(obj.isRunning());

    auto c = tryCount * intervalCount;

    QTest::qWait((c+5)*100);

    QCOMPARE(spy2.count(), c);
    QCOMPARE(spy.count(), tryCount);
    QVERIFY(!obj.isRunning());
}

QTEST_MAIN(WorkerTest);

#include "tst_worker.moc"
