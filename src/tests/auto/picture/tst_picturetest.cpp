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
#include <QGuiApplication>
#include <QClipboard>>
#include "controller/view_controller/imageselectorcontroller.h"
#include "worker/iohelper.h"
#include "utils/iohelper.h"
#include "test_root_path.h"

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
    auto movie = m_ctrl->movie();
    QVERIFY(!movie->isValid());

    QCOMPARE(m_ctrl->validData(), false);

    auto clip = qGuiApp->clipboard();
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
    QRect r{0,0,200,300};
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



    m_ctrl.reset(new ImageSelectorController(true,ImageSelectorController::Clipboard, ImageSelectorController::Square, QString()));
    QCOMPARE(m_ctrl->shape(), ImageSelectorController::Square);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::Clipboard);
    QVERIFY(m_ctrl->askPath());
    QVERIFY(!m_ctrl->canDrop());
    QVERIFY(!m_ctrl->canDownload());
    QVERIFY(m_ctrl->canPaste());
    m_ctrl->setImageData(utils::IOHelper::loadFile(":/img/girafe3.jpg"));
    QVERIFY(!m_ctrl->dataInShape());

    m_ctrl->setRect(r);
    auto final = IOHelper::dataToPixmap(m_ctrl->finalImageData());
    QVERIFY(!final.isNull());

    m_ctrl.reset(new ImageSelectorController(true,ImageSelectorController::DragAndDrop, ImageSelectorController::Square, QString()));
    QCOMPARE(m_ctrl->shape(), ImageSelectorController::Square);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::DragAndDrop);
    QVERIFY(m_ctrl->askPath());
    QVERIFY(m_ctrl->canDrop());
    QVERIFY(!m_ctrl->canDownload());
    QVERIFY(!m_ctrl->canPaste());


    m_ctrl.reset(new ImageSelectorController(true,ImageSelectorController::Web, ImageSelectorController::Square, QString()));
    QCOMPARE(m_ctrl->shape(), ImageSelectorController::Square);
    QCOMPARE(m_ctrl->sources(), ImageSelectorController::Web);
    QVERIFY(m_ctrl->askPath());
    QVERIFY(!m_ctrl->canDrop());
    QVERIFY(m_ctrl->canDownload());
    QVERIFY(!m_ctrl->canPaste());


    m_ctrl.reset(new ImageSelectorController(true,ImageSelectorController::DiskFile, ImageSelectorController::Square, QString()));
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


    QString dir("path/to/root");
    m_ctrl.reset(new ImageSelectorController(true,ImageSelectorController::All, static_cast<ImageSelectorController::Shape>(shape) ,dir));
    m_ctrl->setImageData(utils::IOHelper::loadFile(path));
    m_ctrl->setRect(geometry);
    QCOMPARE(m_ctrl->rectInShape(), expected);


}

void PictureTest::respectShape_data()
{
    QTest::addColumn<int>("shape");
    QTest::addColumn<QRect>("geometry");
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expected");

    QTest::addRow("any with any with img") << static_cast<int>(ImageSelectorController::AnyShape) << QRect(20,20,300,23) <<":/img/girafe3.jpg" << true;
    QTest::addRow("any with any with no img") << static_cast<int>(ImageSelectorController::AnyShape) << QRect(20,20,300,23) << QString{} << false;

    QTest::addRow("any with square with img") << static_cast<int>(ImageSelectorController::AnyShape) << QRect(20,20,300,300) <<":/img/girafe3.jpg"<< true;
    QTest::addRow("any with square with no img") << static_cast<int>(ImageSelectorController::AnyShape) << QRect(20,20,300,300) << QString{}<< false;

    QTest::addRow("square with square with img") << static_cast<int>(ImageSelectorController::Square) << QRect(20,20,300,300) <<":/img/girafe3.jpg"<< true;
    QTest::addRow("square with square with no img") << static_cast<int>(ImageSelectorController::Square) << QRect(20,20,300,300) << QString{}<< false;

    QTest::addRow("square with any with img") << static_cast<int>(ImageSelectorController::Square) << QRect(20,20,200,300) <<":/img/girafe3.jpg"<< false;
    QTest::addRow("square with any with no img") << static_cast<int>(ImageSelectorController::Square) << QRect(20,20,200,300) << QString{}<< false;
}

void PictureTest::finalImage()
{

}

void PictureTest::setImageFromClipboard()
{
    auto clip = qGuiApp->clipboard();
    clip->clear();
    QVERIFY(m_ctrl->imageData().isEmpty());
    m_ctrl->imageFromClipboard();
    QVERIFY(m_ctrl->imageData().isEmpty());

    clip->setPixmap(IOHelper::dataToPixmap(utils::IOHelper::loadFile(":/img/girafe3.jpg")));

    m_ctrl->imageFromClipboard();
    QVERIFY(!m_ctrl->imageData().isEmpty());

    m_ctrl.reset(new ImageSelectorController());
    clip->clear();

    QVERIFY(m_ctrl->imageData().isEmpty());
    m_ctrl->imageFromClipboard();
    QVERIFY(m_ctrl->imageData().isEmpty());

    clip->setImage(utils::IOHelper::dataToImage(utils::IOHelper::loadFile(":/img/girafe3.jpg")));

    m_ctrl->imageFromClipboard();
    QVERIFY(!m_ctrl->imageData().isEmpty());


    clip->clear();
    m_ctrl.reset(new ImageSelectorController());
    QVERIFY(m_ctrl->imageData().isEmpty());

    QMimeData* mimedata = new QMimeData();
    mimedata->setUrls({QUrl::fromUserInput("https://www.nucreum.com/l5r/images/ten.jpg")});

     QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::imageDataChanged);
    clip->setMimeData(mimedata);
    m_ctrl->imageFromClipboard();
    spy.wait();

    QCOMPARE(spy.count(), 1);
    QVERIFY(!m_ctrl->imageData().isEmpty());
    //clip->clear();
    //clip->setMimeData(nullptr);
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

    QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::imageDataChanged);

    m_ctrl->downloadImageFrom(QUrl::fromUserInput(path));

    spy.wait();

    QCOMPARE(spy.count(), 1);
    auto data= m_ctrl->finalImageData();

    auto pix= IOHelper::dataToPixmap(data);

    QVERIFY(!pix.isNull());
}

void PictureTest::readImageFromUrl()
{
    QSignalSpy spy(m_ctrl.get(), &ImageSelectorController::imageDataChanged);

    m_ctrl->downloadImageFrom(QUrl::fromUserInput("https://www.nucreum.com/l5r/images/ten.jpg"));

    spy.wait();

    QCOMPARE(spy.count(), 1);
    auto data= m_ctrl->finalImageData();

    auto pix= IOHelper::dataToPixmap(data);

    QVERIFY(!pix.isNull());
}
QTEST_MAIN(PictureTest);

#include "tst_picturetest.moc"
