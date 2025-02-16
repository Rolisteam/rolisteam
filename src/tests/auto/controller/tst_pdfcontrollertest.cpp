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
#include <QTest>

#include <QClipboard>
#include <QSignalSpy>
#include <QUrl>

#include "controller/view_controller/pdfcontroller.h"
#include "test_root_path.h"
#include "utils/iohelper.h"
#include <memory>

class PdfControllorTest : public QObject
{
    Q_OBJECT

public:
    PdfControllorTest();

private slots:
    void init();
    void cleanupTestCase();

    void create();
    void create_data();

    void zoomTest();
    void zoomTest_data();

    void shareAsPdf();
    void copyImage();

private:
    std::unique_ptr<PdfController> m_ctrl;
};

PdfControllorTest::PdfControllorTest() {}

void PdfControllorTest::init()
{
    m_ctrl.reset(new PdfController());
}

void PdfControllorTest::cleanupTestCase() {}

void PdfControllorTest::create()
{
    QFETCH(QString, path);
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, expected);

    QString id("idtest");

    m_ctrl.reset(new PdfController(id));
    m_ctrl->setData(data);
    m_ctrl->setStaticData(QUrl::fromUserInput(path));

    QCOMPARE(m_ctrl->data(), expected);

    m_ctrl->setData(m_ctrl->data());
    m_ctrl->buffer();
}

void PdfControllorTest::create_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("expected");

    QString path("%1/resources/menu_restaurant.pdf");
    path= path.arg(tests::root_path);

    auto data= utils::IOHelper::loadFile(path);

    QTest::addRow("with path") << path << QByteArray() << data;
    QTest::addRow("with data") << QString() << data << data;
    QTest::addRow("with both") << path << data << data;
}
void PdfControllorTest::zoomTest()
{
    QFETCH(QStringList, actions);
    QFETCH(qreal, expected);
    QFETCH(int, expectedCount);

    QSignalSpy spy(m_ctrl.get(), &PdfController::zoomFactorChanged);

    for(const auto& action : actions)
    {
        if(action == "+")
        {
            m_ctrl->zoomIn();
        }
        else if(action == "-")
        {
            m_ctrl->zoomOut();
        }
        else
        {
            m_ctrl->setZoomFactor(action.toDouble());
        }
    }

    spy.wait(20);
    QCOMPARE(spy.count(), expectedCount);
    QCOMPARE(m_ctrl->zoomFactor(), expected);
}
void PdfControllorTest::zoomTest_data()
{
    QTest::addColumn<QStringList>("actions");
    QTest::addColumn<qreal>("expected");
    QTest::addColumn<int>("expectedCount");

    QTest::addRow("zoomin") << QStringList{"+", "+", "+", "+", "+"} << (1.1) << 5;
    QTest::addRow("zoomout") << QStringList{"-", "-", "-", "-", "-"} << (0.9) << 5;
    QTest::addRow("zommInAndOut") << QStringList{"-", "-", "+", "+", "1.5", "1.5"} << (1.5) << 5;
}

void PdfControllorTest::shareAsPdf()
{
    QSignalSpy spy(m_ctrl.get(), &PdfController::sharingChanged);

    m_ctrl->setSharing(true);

    QCOMPARE(spy.count(), 0);

    QString path("%1/resources/menu_restaurant.pdf");
    path= path.arg(tests::root_path);
    m_ctrl->setData(utils::IOHelper::loadFile(path));

    m_ctrl->setSharing(false);

    QCOMPARE(spy.count(), 1);
}
void PdfControllorTest::copyImage()
{
    m_ctrl->copyImage(QString("%1/resources/img/predateur.jpg").arg(tests::root_path));

    auto clip= QApplication::clipboard();
    QVERIFY(!clip->pixmap().isNull());
}

QTEST_MAIN(PdfControllorTest);

#include "tst_pdfcontrollertest.moc"
