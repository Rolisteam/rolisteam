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
#include "controller/view_controller/imageselectorcontroller.h"
#include "test_root_path.h"
#include "utils/iohelper.h"
#include "worker/iohelper.h"
#include <QClipboard>>
#include <QGuiApplication>
#include <QtTest/QtTest>
#include <helper.h>
#include <memory>

class PictureTest : public QObject
{
    Q_OBJECT

public:
    PictureTest();

private slots:
    void init();
    void create();
    void finalImage();
    void readImageFromFile();
    void readImageFromUrl();
    void respectShape();
    void respectShape_data();
    void setImageFromClipboard();

    void readAllImageSize();
    void readAllImageSize_data();

private:
    std::unique_ptr<ImageSelectorController> m_ctrl;
};

PictureTest::PictureTest() {}

void PictureTest::init()
{
    m_ctrl.reset(new ImageSelectorController());
}
void PictureTest::create()
{
    QVERIFY(m_ctrl->address().isEmpty());
    QVERIFY(m_ctrl->title().isEmpty());
    QCOMPARE(m_ctrl->currentDir(), QDir::homePath());
    QVERIFY(!m_ctrl->dataInShape());

    QCOMPARE(m_ctrl->shape(), ImageSelectorController::AnyShape);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::All);
    QVERIFY(!m_ctrl->askPath());
    QVERIFY(m_ctrl->canDrop());
    QVERIFY(m_ctrl->canDownload());
    QVERIFY(m_ctrl->canPaste());
    auto movie= m_ctrl->movie();
    QVERIFY(!movie->isValid());

    QCOMPARE(m_ctrl->validData(), false);

    auto clip= qGuiApp->clipboard();
    clip->clear();
    QVERIFY(!m_ctrl->hasContentToPaste());
    clip->setPixmap(IOHelper::dataToPixmap(utils::IOHelper::loadFile(":/img/girafe3.jpg")));
    QVERIFY(m_ctrl->hasContentToPaste());

    QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::addressChanged);
    m_ctrl->setAddress(tests::root_path);

    spy.wait(10);
    QCOMPARE(spy.count(), 1);
    m_ctrl->setAddress(tests::root_path);
    spy.wait(10);
    QCOMPARE(spy.count(), 1);

    QSignalSpy spyRect(m_ctrl.get(), &ImageSelectorController::rectChanged);
    QRect r{0, 0, 200, 300};
    m_ctrl->setRect(r);
    spyRect.wait(10);
    QCOMPARE(spyRect.count(), 1);
    QCOMPARE(m_ctrl->rect(), r);
    m_ctrl->setRect(r);
    spyRect.wait(10);
    QCOMPARE(spyRect.count(), 1);

    QSignalSpy spytitle(m_ctrl.get(), &ImageSelectorController::titleChanged);
    QString title{"foobar"};
    m_ctrl->setTitle(title);
    spytitle.wait(10);
    QCOMPARE(spytitle.count(), 1);
    QCOMPARE(m_ctrl->title(), title);
    m_ctrl->setTitle(title);
    spytitle.wait(10);
    QCOMPARE(spytitle.count(), 1);

    m_ctrl.reset(new ImageSelectorController(true, ImageSelectorController::Clipboard, ImageSelectorController::Square,
                                             QString()));
    QCOMPARE(m_ctrl->shape(), ImageSelectorController::Square);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::Clipboard);
    QVERIFY(m_ctrl->askPath());
    QVERIFY(!m_ctrl->canDrop());
    QVERIFY(!m_ctrl->canDownload());
    QVERIFY(m_ctrl->canPaste());
    QSignalSpy waitPixmapChanged(m_ctrl.get(), &ImageSelectorController::pixmapChanged);

    m_ctrl->setImageData(utils::IOHelper::loadFile(":/img/girafe3.jpg"));
    QVERIFY(!m_ctrl->dataInShape());

    waitPixmapChanged.wait();
    QCOMPARE(waitPixmapChanged.count(), 1);

    m_ctrl->setVisualSize({300, 300});

    waitPixmapChanged.wait();
    QCOMPARE(waitPixmapChanged.count(), 2);
    m_ctrl->setRect(r);
    auto final= IOHelper::dataToPixmap(m_ctrl->finalImageData());
    QVERIFY(!final.isNull());

    m_ctrl.reset(new ImageSelectorController(true, ImageSelectorController::DragAndDrop,
                                             ImageSelectorController::Square, QString()));
    QCOMPARE(m_ctrl->shape(), ImageSelectorController::Square);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::DragAndDrop);
    QVERIFY(m_ctrl->askPath());
    QVERIFY(m_ctrl->canDrop());
    QVERIFY(!m_ctrl->canDownload());
    QVERIFY(!m_ctrl->canPaste());

    m_ctrl.reset(
        new ImageSelectorController(true, ImageSelectorController::Web, ImageSelectorController::Square, QString()));
    QCOMPARE(m_ctrl->shape(), ImageSelectorController::Square);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::Web);
    QVERIFY(m_ctrl->askPath());
    QVERIFY(!m_ctrl->canDrop());
    QVERIFY(m_ctrl->canDownload());
    QVERIFY(!m_ctrl->canPaste());

    m_ctrl.reset(new ImageSelectorController(true, ImageSelectorController::DiskFile, ImageSelectorController::Square,
                                             QString()));
    QCOMPARE(m_ctrl->shape(), ImageSelectorController::Square);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::DiskFile);
    QVERIFY(m_ctrl->askPath());
    QVERIFY(!m_ctrl->canDrop());
    QVERIFY(!m_ctrl->canDownload());
    QVERIFY(!m_ctrl->canPaste());
}

void PictureTest::respectShape()
{
    QFETCH(int, shape);
    QFETCH(QRect, geometry);
    QFETCH(QString, path);
    QFETCH(bool, expected);
    QFETCH(int, count);

    QString dir("path/to/root");
    m_ctrl.reset(new ImageSelectorController(true, ImageSelectorController::All,
                                             static_cast<ImageSelectorController::Shape>(shape), dir));
    m_ctrl->setVisualSize(QSize{547,333});
    m_ctrl->setRect(geometry);
    m_ctrl->setImageData(utils::IOHelper::loadFile(path));

    QSignalSpy waitPixmapChanged(m_ctrl.get(), &ImageSelectorController::pixmapChanged);

    waitPixmapChanged.wait();
    QCOMPARE(waitPixmapChanged.count(), count);
    QCOMPARE(m_ctrl->rectInShape(), expected);
}

void PictureTest::respectShape_data()
{
    QTest::addColumn<int>("shape");
    QTest::addColumn<QRect>("geometry");
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");
    QTest::addColumn<int>("count");

    QTest::addRow("any with any with img")
        << static_cast<int>(ImageSelectorController::AnyShape) << QRect(20, 20, 300, 23) << ":/img/girafe3.jpg" << true << 1;
    QTest::addRow("any with any with no img")
        << static_cast<int>(ImageSelectorController::AnyShape) << QRect(20, 20, 300, 23) << QString{} << false << 0;

    QTest::addRow("any with square with img") << static_cast<int>(ImageSelectorController::AnyShape)
                                              << QRect(20, 20, 300, 300) << ":/img/girafe3.jpg" << true << 1;
    QTest::addRow("any with square with no img")
        << static_cast<int>(ImageSelectorController::AnyShape) << QRect(20, 20, 300, 300) << QString{} << false << 0;

    QTest::addRow("square with square with img")
        << static_cast<int>(ImageSelectorController::Square) << QRect(20, 20, 300, 300) << ":/img/girafe3.jpg" << true  << 1;
    QTest::addRow("square with square with no img")
        << static_cast<int>(ImageSelectorController::Square) << QRect(20, 20, 300, 300) << QString{} << false  << 0;

    QTest::addRow("square with any with img")
        << static_cast<int>(ImageSelectorController::Square) << QRect(20, 20, 200, 300) << ":/img/girafe3.jpg" << false  << 1;
    QTest::addRow("square with any with no img")
        << static_cast<int>(ImageSelectorController::Square) << QRect(20, 20, 200, 300) << QString{} << false  << 0;
}

void PictureTest::finalImage() {}

void PictureTest::setImageFromClipboard()
{
    auto server= Helper::initWebServer();
    QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::pixmapChanged);

    auto clip= qGuiApp->clipboard();
    clip->clear();
    QVERIFY(m_ctrl->imageData().isEmpty());
    m_ctrl->imageFromClipboard();
    spy.wait();
    QCOMPARE(spy.count(), 0);
    QVERIFY(m_ctrl->imageData().isEmpty());

    clip->setPixmap(IOHelper::dataToPixmap(utils::IOHelper::loadFile(":/img/girafe3.jpg")));

    m_ctrl->imageFromClipboard();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QVERIFY(!m_ctrl->imageData().isEmpty());

    m_ctrl.reset(new ImageSelectorController());
    QSignalSpy spy2(m_ctrl.get(), &ImageSelectorController::pixmapChanged);
    clip->clear();

    QVERIFY(m_ctrl->imageData().isEmpty());
    m_ctrl->imageFromClipboard();
    spy2.wait();
    QCOMPARE(spy2.count(), 0);
    QVERIFY(m_ctrl->imageData().isEmpty());

    clip->setImage(utils::IOHelper::dataToImage(utils::IOHelper::loadFile(":/img/girafe3.jpg")));

    m_ctrl->imageFromClipboard();
    spy2.wait();
    QCOMPARE(spy2.count(), 1);
    QVERIFY(!m_ctrl->imageData().isEmpty());

    clip->clear();
    m_ctrl.reset(new ImageSelectorController());
    QVERIFY(m_ctrl->imageData().isEmpty());

    QMimeData* mimedata= new QMimeData();
    mimedata->setUrls({QUrl::fromUserInput("http://127.0.0.1:9090/image/Seppun_tashime.jpg")});

    clip->setMimeData(mimedata);
    m_ctrl->imageFromClipboard();

    spy2.wait();
    QCOMPARE(spy2.count(), 1);
    QVERIFY(!m_ctrl->imageData().isEmpty());
    // clip->clear();
    // clip->setMimeData(nullptr);

    delete server;
}

void PictureTest::readImageFromFile()
{
    QFile file(":/img/girafe3.jpg");
    QByteArray dataArray;
    if(file.open(QIODevice::ReadOnly))
    {
        dataArray= file.readAll();
    }
    QTemporaryFile tfile;
    QString path;
    if(tfile.open())
    {
        tfile.write(dataArray);
        path= tfile.fileName();
    }

    QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::pixmapChanged);

    m_ctrl->downloadImageFrom(QUrl::fromUserInput(path));

    spy.wait();

    QCOMPARE(spy.count(), 1);
    auto data= m_ctrl->finalImageData();

    auto pix= IOHelper::dataToPixmap(data);

    QVERIFY(!pix.isNull());
}

void PictureTest::readImageFromUrl()
{
    auto server= Helper::initWebServer();
    QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::pixmapChanged);

    m_ctrl->downloadImageFrom(QUrl::fromUserInput("http://127.0.0.1:9090/image/Seppun_tashime.jpg"));

    spy.wait();

    QCOMPARE(spy.count(), 1);
    auto data= m_ctrl->finalImageData();

    auto pix= IOHelper::dataToPixmap(data);

    QVERIFY(!pix.isNull());

    delete server;
}

void PictureTest::readAllImageSize()
{
    QFETCH(QSize, imageSize);
    QFETCH(QSize, visualsize);
    QFETCH(QRect, selectedRect);
    QFETCH(bool, expected);

    QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::pixmapChanged);

    auto img = utils::IOHelper::dataToImage(utils::IOHelper::loadFile(":/img/girafe3.jpg"));

    auto imgW = img.copy(0,0, imageSize.width(), imageSize.height());


    m_ctrl->setVisualSize(visualsize);
    m_ctrl->setRect(selectedRect);

    m_ctrl->setImageData(IOHelper::imageToData(imgW));

    spy.wait();

    QCOMPARE(spy.count(), 1);

    QCOMPARE(m_ctrl->rectInShape(), expected);
}

void PictureTest::readAllImageSize_data()
{
    QTest::addColumn<QSize>("imageSize");
    QTest::addColumn<QSize>("visualsize");
    QTest::addColumn<QRect>("selectedRect");
    QTest::addColumn<bool>("expected");

    QTest::addRow("ImageBigSelectionIn") << QSize(3264,2448) << QSize(547,333) << QRect{100,100,50,50} << true;
    QTest::addRow("ImageBigSelectionOut") << QSize(3264,2448) << QSize(547,333) << QRect{-100,-100,50,50} << false;

    //QTest::addRow("ImageLargeSelectionDefault") << QSize(600,333) << QSize(547,333) << QRect{} << true;
    QTest::addRow("ImageLargeSelectionIn") << QSize(600,333) << QSize(547,333) << QRect{100,100,50,50} << true;
    QTest::addRow("ImageLargeSelectionOut") << QSize(600,333) << QSize(547,333) << QRect{-100,-100,50,50} << false;

    //QTest::addRow("ImageHighSelectionDefault") << QSize(547,400) << QSize(547,333) << QRect{} << true;
    QTest::addRow("ImageHighSelectionIn") << QSize(547,400) << QSize(547,333) << QRect{100,100,50,50} << true;
    QTest::addRow("ImageHighSelectionOut") << QSize(547,400) << QSize(547,333) << QRect{-100,-100,50,50} << false;


    //QTest::addRow("ImageSmallSelectionDefault") << QSize(123,111) << QSize(547,333) << QRect{} << true;
    QTest::addRow("ImageSmallSelectionIn") << QSize(123,111) << QSize(547,333) << QRect{100,100,50,50} << true;
    QTest::addRow("ImageSmallSelectionOut") << QSize(123,111) << QSize(547,333) << QRect{-100,-100,50,50} << false;

}
QTEST_MAIN(PictureTest);

#include "tst_picturetest.moc"
