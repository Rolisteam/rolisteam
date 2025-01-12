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
#include <QUrl>

#include "controller/view_controller/imagecontroller.h"
#include <memory>

class ImageControllorTest : public QObject
{
    Q_OBJECT

public:
    ImageControllorTest();

private slots:
    void init();
    void cleanupTestCase();

    void formatTest();
    void formatTest_data();

    void zoomTest();

    void proprietyTest();

    void movieTest();

private:
    std::unique_ptr<ImageController> m_ctrl;
};

ImageControllorTest::ImageControllorTest() {}

void ImageControllorTest::init()
{
    m_ctrl.reset(new ImageController());
}

void ImageControllorTest::cleanupTestCase() {}

void ImageControllorTest::proprietyTest()
{
    m_ctrl.reset(new ImageController("", "", QUrl("qrc://img/girafe.jpg"), ""));

    m_ctrl->setFitWindow(true);

    QSignalSpy spy1(m_ctrl.get(), &ImageController::fitWindowChanged);
    QSignalSpy spy2(m_ctrl.get(), &ImageController::cursorChanged);

    m_ctrl->setFitWindow(false);

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(m_ctrl->cursor(), Qt::OpenHandCursor);
    QCOMPARE(m_ctrl->fitWindow(), false);
}

void ImageControllorTest::formatTest()
{
    QFETCH(QString, path);
    QFETCH(qreal, ratioV);
    QFETCH(qreal, ratioH);
    QFETCH(bool, isMovie);

    m_ctrl.reset(new ImageController("", "", path));

    QSignalSpy spy1(m_ctrl.get(), &ImageController::pixmapChanged);
    spy1.wait(1000);

    QCOMPARE(m_ctrl->ratioH(), ratioH);

    QCOMPARE(m_ctrl->ratioV(), ratioV);

    QCOMPARE(m_ctrl->isMovie(), isMovie);
}

void ImageControllorTest::formatTest_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<qreal>("ratioV");
    QTest::addColumn<qreal>("ratioH");
    QTest::addColumn<bool>("isMovie");
    // QTest::addColumn<bool>("fitWindow");

    QTest::addRow("value 1") << "qrc://img/girafe.jpg" << 1000.0 / 750 << 0.75 << false;
    QTest::addRow("value 2") << "qrc://img/girafe3.jpg" << 3264.0 / 2448 << 2448.0 / 3264 << false;
    QTest::addRow("value 3") << "qrc://img/lion.jpg" << 1000.0 / 750 << 0.75 << false;
    QTest::addRow("value 4") << "qrc://img/lion3.jpg" << 3264.0 / 2448 << 2448.0 / 3264 << false;
    QTest::addRow("value 5") << "qrc://img/white.png" << 1126.0 / 271 << 271.0 / 1126 << false;
    QTest::addRow("value 6") << "qrc://img/control_life_bar.gif" << 1784.0 / 828 << 828.0 / 1784 << true;
}

void ImageControllorTest::zoomTest()
{
    m_ctrl.reset(new ImageController("", "", QUrl(":/img/girafe.jpg"), ""));

    m_ctrl->setZoomLevel(1.0); // set default value

    QSignalSpy spy1(m_ctrl.get(), &ImageController::zoomLevelChanged);

    m_ctrl->zoomIn();

    QCOMPARE(m_ctrl->zoomLevel(), 1.2);
    QCOMPARE(spy1.count(), 1);

    m_ctrl->zoomOut();

    QCOMPARE(m_ctrl->zoomLevel(), 1.0);
    QCOMPARE(spy1.count(), 2);

    m_ctrl->zoomIn(0.4);

    QCOMPARE(m_ctrl->zoomLevel(), 1.4);
    QCOMPARE(spy1.count(), 3);

    m_ctrl->zoomOut(0.4);

    QCOMPARE(m_ctrl->zoomLevel(), 1.0);
    QCOMPARE(spy1.count(), 4);

    m_ctrl->setZoomLevel(2.4);

    QCOMPARE(m_ctrl->zoomLevel(), 2.4);
    QCOMPARE(spy1.count(), 5);

    m_ctrl->setZoomLevel(0.5);

    QCOMPARE(m_ctrl->zoomLevel(), 0.5);
    QCOMPARE(spy1.count(), 6);
}

void ImageControllorTest::movieTest()
{
    m_ctrl.reset(new ImageController("", "", QUrl("qrc://img/control_life_bar.gif")));

    QVERIFY2(m_ctrl->isMovie(), "Not a movie!!");
    QCOMPARE(m_ctrl->status(), ImageController::Playing);

    QSignalSpy spy1(m_ctrl.get(), &ImageController::statusChanged);

    m_ctrl->play(); // play pause are the same

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(m_ctrl->status(), ImageController::Paused);

    m_ctrl->play(); // play pause are the same

    QCOMPARE(spy1.count(), 2);
    QCOMPARE(m_ctrl->status(), ImageController::Playing);

    m_ctrl->stop();

    QCOMPARE(spy1.count(), 3);
    QCOMPARE(m_ctrl->status(), ImageController::Stopped);
}

QTEST_MAIN(ImageControllorTest);

#include "tst_imagecontrollertest.moc"
