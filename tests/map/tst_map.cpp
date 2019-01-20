/***************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                                *
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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <memory>

#include "map.h"
#include "mapframe.h"

#include "network/networkmessage.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

class TestMap : public QObject
{
    Q_OBJECT
public:
    TestMap();

private slots:
    void init();

    void getAndSetTest();

    void networkSaveAndLoadTest();
    void fileSaveAndLoadTest();

private:
    std::unique_ptr<MapFrame> m_mapFrame;
    Map* m_map;
};

TestMap::TestMap() {}

void TestMap::init()
{
    m_mapFrame.reset(new MapFrame());
}
void TestMap::networkSaveAndLoadTest()
{
    QImage img(800, 600, QImage::Format_ARGB32_Premultiplied);
    m_map= new Map("id", "idcarte", &img, false);

    m_mapFrame->setMap(m_map);

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::addMedia);
    m_mapFrame->fill(msg);

    auto const array= msg.getData();

    NetworkMessageReader msg2;
    msg2.setData(array);
    msg2.resetToData();

    MapFrame frame2;
    frame2.setCleverUriType(CleverURI::MAP);
    frame2.readMessage(msg2);

    auto map= m_mapFrame->getMap();
    auto map2= frame2.getMap();

    QCOMPARE(map->getMapId(), map2->getMapId());
    QCOMPARE(m_mapFrame->getMediaId(), frame2.getMediaId());
    QCOMPARE(m_mapFrame->getUriName(), frame2.getUriName());
}

void TestMap::getAndSetTest() {}

void TestMap::fileSaveAndLoadTest()
{
    QImage img(800, 600, QImage::Format_ARGB32_Premultiplied);
    m_map= new Map("id", "idcarte", &img, false);

    m_mapFrame->setMap(m_map);

    auto title= QStringLiteral("title");
    m_mapFrame->setUriName(title);

    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    m_map->saveMap(out, title);

    MapFrame frame2;

    QDataStream in(&array, QIODevice::ReadOnly);
    frame2.readMapAndNpc(in, false);

    // auto map = m_mapFrame->getMap();

    QCOMPARE(frame2.windowTitle(), m_mapFrame->windowTitle());
}

QTEST_MAIN(TestMap);

#include "tst_map.moc"
