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

#include "mindmap/data/mindmaptypes.h"
#include "utils/countdownobject.h"
#include "utils/iohelper.h"
#include "utils/mappinghelper.h"
#include "worker/convertionhelper.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"
#include <helper.h>

class WorkerTest : public QObject
{
    Q_OBJECT

public:
    enum class Type
    {
        Integer,
        PointF,
        Real,
        Bool,
        ScaleUnit,
        PermissionMode,
        GridPattern,
        Layer,
        VisibilityMode,
        Color,
        Image,
        RectF,
        uint16,
        ByteArray,
        Font,
        VecPoint,
        ShapeVision,
        Size,
        PermissionParticipiant,
        ArrowDirection
    };
    Q_ENUM(Type)
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

    void convertionHelperTest();
    void convertionHelperTest_data();
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
            for(auto r : std::as_const(ratios))
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

    CountDownObject obj(tryCount, intervalCount, 100); // 100 ms

    QSignalSpy spy(&obj, &CountDownObject::triggered);
    QSignalSpy spy2(&obj, &CountDownObject::countDownChanged);

    QVERIFY(!obj.isRunning());

    obj.start();
    QVERIFY(obj.isRunning());

    auto c= tryCount * intervalCount;

    QTest::qWait((c + 5) * 100);

    QCOMPARE(spy2.count(), c);
    QCOMPARE(spy.count(), tryCount + 1);
    QVERIFY(!obj.isRunning());
}

void WorkerTest::convertionHelperTest()
{
    QFETCH(QVariant, data);
    QFETCH(ulong, size);
    QFETCH(Type, type);

    NetworkMessageWriter writer(NetMsg::InstantMessageCategory, NetMsg::InstantMessageAction);

    QCOMPARE(writer.currentPos(), 9);

    auto realSize= 9 + size;

    switch(type)
    {

    case Type::Integer:
        Helper::variantToType<int>(data.value<int>(), writer);
        break;
    case Type::PointF:
        Helper::variantToType<QPointF>(data.value<QPointF>(), writer);
        break;
    case Type::Real:
        Helper::variantToType<qreal>(data.value<qreal>(), writer);
        break;
    case Type::Bool:
        Helper::variantToType<bool>(data.value<bool>(), writer);
        break;
    case Type::ScaleUnit:
        Helper::variantToType<Core::ScaleUnit>(data.value<Core::ScaleUnit>(), writer);
        break;
    case Type::PermissionMode:
        Helper::variantToType<Core::PermissionMode>(data.value<Core::PermissionMode>(), writer);
        break;
    case Type::GridPattern:
        Helper::variantToType<Core::GridPattern>(data.value<Core::GridPattern>(), writer);
        break;
    case Type::Layer:
        Helper::variantToType<Core::Layer>(data.value<Core::Layer>(), writer);
        break;
    case Type::VisibilityMode:
        Helper::variantToType<Core::VisibilityMode>(data.value<Core::VisibilityMode>(), writer);
        break;
    case Type::Color:
        Helper::variantToType<QColor>(data.value<QColor>(), writer);
        break;
    case Type::Image:
        Helper::variantToType<QImage>(data.value<QImage>(), writer);
        break;
    case Type::RectF:
        Helper::variantToType<QRectF>(data.value<QRectF>(), writer);
        break;
    case Type::uint16:
        Helper::variantToType<quint16>(data.value<quint16>(), writer);
        break;
    case Type::ByteArray:
        Helper::variantToType<QByteArray>(data.value<QByteArray>(), writer);
        break;
    case Type::Font:
        Helper::variantToType<QFont>(data.value<QFont>(), writer);
        break;
    case Type::VecPoint:
        Helper::variantToType<std::vector<QPointF>>(data.value<std::vector<QPointF>>(), writer);
        break;
    case Type::ShapeVision:
        Helper::variantToType<CharacterVision::SHAPE>(data.value<CharacterVision::SHAPE>(), writer);
        break;
    case Type::Size:
        Helper::variantToType<QSize>(data.value<QSize>(), writer);
        break;
    case Type::PermissionParticipiant:
        Helper::variantToType<ParticipantModel::Permission>(data.value<ParticipantModel::Permission>(), writer);
        break;
    case Type::ArrowDirection:
        Helper::variantToType<mindmap::ArrowDirection>(data.value<mindmap::ArrowDirection>(), writer);
        break;
    }
    QCOMPARE(writer.currentPos(), realSize);
}

void WorkerTest::convertionHelperTest_data()
{
    QTest::addColumn<QVariant>("data");
    QTest::addColumn<ulong>("size");
    QTest::addColumn<Type>("type");

    QTest::addRow("cmd1") << QVariant::fromValue(true) << sizeof(bool) << Type::Bool;
    QTest::addRow("cmd2") << QVariant::fromValue(static_cast<qint64>(23)) << sizeof(qint64) << Type::Integer;
    QTest::addRow("cmd3") << QVariant::fromValue(23.0) << sizeof(qreal) << Type::Real;
    QTest::addRow("cmd4") << QVariant::fromValue(Core::ScaleUnit::FEET) << sizeof(quint8) << Type::ScaleUnit;
    QTest::addRow("cmd5") << QVariant::fromValue(Core::PermissionMode::GM_ONLY) << sizeof(quint8)
                          << Type::PermissionMode;
    QTest::addRow("cmd6") << QVariant::fromValue(Core::GridPattern::HEXAGON) << sizeof(Core::GridPattern)
                          << Type::GridPattern;
    QTest::addRow("cmd7") << QVariant::fromValue(Core::Layer::GRIDLAYER) << sizeof(Core::Layer) << Type::Layer;
    QTest::addRow("cmd8") << QVariant::fromValue(Core::VisibilityMode::FOGOFWAR) << sizeof(quint8)
                          << Type::VisibilityMode;
    QTest::addRow("cmd9") << QVariant::fromValue(QColor(Qt::blue)) << sizeof(unsigned int) << Type::Color;
    QImage img= QImage::fromData(Helper::imageData());

    QByteArray data2;
    QDataStream in(&data2, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_7);
    in << img;

    QTest::addRow("cmd10") << QVariant::fromValue(img) << static_cast<ulong>(sizeof(quint32) + data2.size())
                           << Type::Image;
    QTest::addRow("cmd11") << QVariant::fromValue(QPointF{10.0, 100.0}) << sizeof(qreal) + sizeof(qreal)
                           << Type::PointF;
    QTest::addRow("cmd12") << QVariant::fromValue(QRectF{10.0, 100.0, 100., 100.}) << 4 * sizeof(qreal) << Type::RectF;

    QTest::addRow("cmd13") << QVariant::fromValue(static_cast<quint16>(250)) << sizeof(quint16) << Type::uint16;
    auto data= Helper::randomData();
    QTest::addRow("cmd14") << QVariant::fromValue(data) << static_cast<ulong>(sizeof(quint32) + data.size())
                           << Type::ByteArray;
    QFont font;
    QTest::addRow("cmd15") << QVariant::fromValue(font)
                           << static_cast<ulong>(sizeof(quint16)
                                                 + static_cast<quint64>(font.toString().size())
                                                       * static_cast<quint64>(sizeof(QChar)))
                           << Type::Font;

    std::vector<QPointF> vec{{100.0, 100.0}, {500.0, 500.0}};
    QTest::addRow("cmd16") << QVariant::fromValue(vec) << static_cast<ulong>(sizeof(quint64) + sizeof(qreal) * 4)
                           << Type::VecPoint;

    QTest::addRow("cmd17") << QVariant::fromValue(CharacterVision::SHAPE::ANGLE) << sizeof(quint8) << Type::ShapeVision;
    QTest::addRow("cmd18") << QVariant::fromValue(QSize(100, 100)) << sizeof(QSize) << Type::Size;
    QTest::addRow("cmd19") << QVariant::fromValue(ParticipantModel::Permission::hidden) << sizeof(quint8)
                           << Type::PermissionParticipiant;
    QTest::addRow("cmd20") << QVariant::fromValue(mindmap::ArrowDirection::EndToStart)
                           << sizeof(mindmap::ArrowDirection) << Type::ArrowDirection;
}

QTEST_MAIN(WorkerTest);

#include "tst_worker.moc"
