/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   renaud@rolisteam.org                  *
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
#include "data/rolisteammimedata.h"
#include "media/mediatype.h"
#include <helper.h>
#include <QSignalSpy>

class RolisteamMimeDataTest : public QObject
{
    Q_OBJECT
public:
    RolisteamMimeDataTest();

private Q_SLOTS:
    void testGetSet();

private:
    RolisteamMimeData m_data;
};

RolisteamMimeDataTest::RolisteamMimeDataTest()
{

}

void RolisteamMimeDataTest::testGetSet()
{
    QStringList list{Core::mimedata::MIME_KEY_PERSON_DATA,
                     Core::mimedata::MIME_KEY_DICE_ALIAS_DATA,
                     Core::mimedata::MIME_KEY_MEDIA_UUID,
                     Core::mimedata::MIME_KEY_NPC_ID};

    for(auto s : list)
        m_data.hasFormat(s);

    auto uuid = Helper::randomString();
    QSignalSpy spy(&m_data, &RolisteamMimeData::mediaUuidChanged);
    m_data.setMediaUuid(uuid);
    m_data.setMediaUuid(uuid);
    QCOMPARE(m_data.mediaUuid(), uuid);
    QCOMPARE(spy.count(), 1);

    uuid = Helper::randomString();
    QSignalSpy spy2(&m_data, &RolisteamMimeData::npcUuidChanged);
    m_data.setNpcUuid(uuid);
    m_data.setNpcUuid(uuid);
    QCOMPARE(m_data.npcUuid(), uuid);
    QCOMPARE(spy2.count(), 1);



    auto aName = Helper::randomString();
    auto aCmd = Helper::randomString();
    QSignalSpy spy3(&m_data, &RolisteamMimeData::aliasChanged);
    m_data.setAlias(aName, aCmd, true);

    QCOMPARE(m_data.alias().command(), aCmd);
    QCOMPARE(spy2.count(), 1);
}

QTEST_MAIN(RolisteamMimeDataTest);

#include "tst_rolisteammimedata.moc"
