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

#include "core/worker/utilshelper.h"

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
};

WorkerTest::WorkerTest() {}

void WorkerTest::init() {}

void WorkerTest::cleanupTestCase() {}

void WorkerTest::helperUtilsTest()
{
    QFETCH(QRect, rect);
    QFETCH(qreal, ratio);
    static int i= 0;

    auto newRect= helper::utils::computerBiggerRectInside(rect, ratio);

    QVERIFY(newRect.width() <= rect.width());
    QVERIFY(newRect.height() <= rect.height());

    // if(!qFuzzyCompare(static_cast<qreal>(newRect.width()) / newRect.height(), ratio))
    {
        // qDebug() << rect << ratio << newRect << (newRect.width() / newRect.height());

        QImage image(rect.width(), rect.height(), QImage::Format_Mono);
        image.fill(1);

        QPainter painter(&image);
        newRect.setX(rect.width() / 2 - newRect.width() / 2);
        newRect.setY(rect.height() / 2 - newRect.height() / 2);
        auto pen= painter.pen();
        pen.setColor(Qt::black);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawRect(newRect);

        painter.end();

        image.save(QString("fileName_%1.jpg").arg(i++, 5, 10, QChar('0')), "jpg");
    }

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

QTEST_MAIN(WorkerTest);

#include "tst_worker.moc"
