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

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/cleveruri.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "updater/vmapupdater.h"
#include "worker/convertionhelper.h"
#include <memory>

class UpdaterVMapTest : public QObject
{
    Q_OBJECT

public:
    enum Type
    {
        REAL,
        INT,
        LAYER,
        VISIBILITY,
        GRIDPATTERN,
        PERMISSION,
        BOOL,
        QCOLOR,
        UNIT
    };
    Q_ENUM(Type)
    UpdaterVMapTest();

private slots:
    void init();
    void cleanupTestCase();
    void serializationTest();
    void serializationTest_data();

private:
    std::unique_ptr<VMapUpdater> m_updater;
};

UpdaterVMapTest::UpdaterVMapTest() {}

void UpdaterVMapTest::init()
{
    m_updater.reset(new VMapUpdater);
}

void UpdaterVMapTest::cleanupTestCase() {}

void UpdaterVMapTest::serializationTest()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, propertyValue);
    QFETCH(Type, type);
    QFETCH(bool, result);

    VectorialMapController map(new CleverURI(CleverURI::ContentType::VMAP));
    auto ok= map.setProperty(propertyName.toLocal8Bit().data(), propertyValue);
    QVERIFY(ok);

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::UpdateMediaProperty);
    msg.string16(propertyName);
    switch(type)
    {
    case REAL:
        Helper::variantToType<qreal>(propertyValue.value<qreal>(), msg);
        break;
    case INT:
        Helper::variantToType<int>(propertyValue.value<int>(), msg);
        break;
    case LAYER:
        Helper::variantToType<Core::Layer>(propertyValue.value<Core::Layer>(), msg);
        break;
    case VISIBILITY:
        Helper::variantToType<Core::VisibilityMode>(propertyValue.value<Core::VisibilityMode>(), msg);
        break;
    case GRIDPATTERN:
        Helper::variantToType<Core::GridPattern>(propertyValue.value<Core::GridPattern>(), msg);
        break;
    case PERMISSION:
        Helper::variantToType<Core::PermissionMode>(propertyValue.value<Core::PermissionMode>(), msg);
        break;
    case BOOL:
        Helper::variantToType<bool>(propertyValue.value<bool>(), msg);
        break;
    case QCOLOR:
        Helper::variantToType<QColor>(propertyValue.value<QColor>(), msg);
        break;
    case UNIT:
        Helper::variantToType<Core::ScaleUnit>(propertyValue.value<Core::ScaleUnit>(), msg);
        break;
    }

    auto data= msg.getData();

    NetworkMessageReader msgReader;
    msgReader.setData(data);

    VectorialMapController ma2p(new CleverURI(CleverURI::ContentType::VMAP));
    QVERIFY(m_updater->updateVMapProperty(&msgReader, &ma2p));
}

void UpdaterVMapTest::serializationTest_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("propertyValue");
    QTest::addColumn<Type>("type");
    QTest::addColumn<bool>("result");

    QTest::addRow("value 1") << QString("collision") << QVariant::fromValue(true) << BOOL << true;
    QTest::addRow("value 2") << QString("collision") << QVariant::fromValue(false) << BOOL << true;
    QTest::addRow("value 3") << QString("characterVision") << QVariant::fromValue(true) << BOOL << true;
    QTest::addRow("value 4") << QString("characterVision") << QVariant::fromValue(false) << BOOL << true;
    QTest::addRow("value 5") << QString("gridColor") << QVariant::fromValue(QColor(Qt::yellow)) << QCOLOR << true;
    QTest::addRow("value 6") << QString("gridScale") << QVariant::fromValue(1.5) << REAL << true;
    QTest::addRow("value 7") << QString("gridSize") << QVariant::fromValue(29) << INT << true;
    QTest::addRow("value 8") << QString("gridVisibility") << QVariant::fromValue(true) << BOOL << true;
    QTest::addRow("value 8bis") << QString("gridVisibility") << QVariant::fromValue(false) << BOOL << true;
    QTest::addRow("value 9") << QString("gridAbove") << QVariant::fromValue(false) << BOOL << true;
    QTest::addRow("value 9bis") << QString("gridAbove") << QVariant::fromValue(true) << BOOL << true;
    QTest::addRow("value 10") << QString("scaleUnit") << QVariant::fromValue(Core::KM) << UNIT << true;
    QTest::addRow("value 11") << QString("permission") << QVariant::fromValue(Core::PC_ALL) << PERMISSION << true;
    QTest::addRow("value 12") << QString("gridPattern") << QVariant::fromValue(Core::SQUARE) << GRIDPATTERN << true;
    QTest::addRow("value 13") << QString("visibility") << QVariant::fromValue(Core::FOGOFWAR) << VISIBILITY << true;
    QTest::addRow("value 14") << QString("backgroundColor") << QVariant::fromValue(QColor(Qt::yellow)) << QCOLOR
                              << true;
    QTest::addRow("value 15") << QString("layer") << QVariant::fromValue(Core::Layer::GROUND) << LAYER << true;
    QTest::addRow("value 16") << QString("zIndex") << QVariant::fromValue(18) << INT << true;
}

QTEST_MAIN(UpdaterVMapTest);

#include "tst_vmapupdatertest.moc"
