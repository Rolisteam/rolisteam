/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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

#include <QModelIndex>
#include <QModelIndexList>
#include <QtCore/QString>
#include <memory>

#include "audio/audioPlayer.h"
#include "audio/playerwidget.h"

#define TIME_SONG 36

class TestAudioPlayer : public QObject
{
    Q_OBJECT
public:
    TestAudioPlayer();

private slots:
    void init();

    void addSongTest();
    void addSongTest_data();

    void playSong();
    void playSong_data();

    void playSongInLoop();
    void playSongInLoop_data();

private:
    // std::unique_ptr<AudioPlayer> m_audioPlayer;
    std::unique_ptr<PlayerWidget> m_playerWidget;
};

TestAudioPlayer::TestAudioPlayer() {}

void TestAudioPlayer::init()
{
    //    m_audioPlayer.reset(new AudioPlayer());
    m_playerWidget.reset(new PlayerWidget(0));
}

void TestAudioPlayer::addSongTest()
{
    QFETCH(QStringList, songs);
    QFETCH(int, expected);

    for(auto song : songs)
    {
        m_playerWidget->addSongIntoModel(song);
    }
    auto model= m_playerWidget->model();
    QCOMPARE(model->rowCount(), expected);
}

void TestAudioPlayer::addSongTest_data()
{
    QTest::addColumn<QStringList>("songs");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QStringList() << 0;
    QTest::addRow("list2") << QStringList({"song"}) << 1;
    QTest::addRow("list3") << QStringList({"song1", "song2"}) << 2;
    QTest::addRow("list4") << QStringList({"song1", "song2", "song3"}) << 3;
}

void TestAudioPlayer::playSong()
{
    QFETCH(QStringList, songs);
    QFETCH(int, expected);

    for(auto song : songs)
    {
        m_playerWidget->addSongIntoModel(song);
    }
    auto model= m_playerWidget->model();
    QCOMPARE(model->rowCount(), expected);

    QSignalSpy spy(m_playerWidget.get(), &PlayerWidget::playerIsPlaying);
    QSignalSpy spy2(m_playerWidget.get(), &PlayerWidget::newSongPlayed);
    m_playerWidget->startMedia(model->getMediaByModelIndex(model->index(0)));

    spy.wait();
    spy2.wait();
    QCOMPARE(spy.count(), expected);
    QCOMPARE(spy2.count(), expected);
}

void TestAudioPlayer::playSong_data()
{
    QTest::addColumn<QStringList>("songs");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QStringList({"qrc:/music/07.mp3"}) << 1;
}

void TestAudioPlayer::playSongInLoop()
{
    QFETCH(QStringList, songs);
    QFETCH(int, expected);

    m_playerWidget->setPlayingMode(PlayerWidget::LOOP);

    for(auto song : songs)
    {
        m_playerWidget->addSongIntoModel(song);
    }
    auto model= m_playerWidget->model();
    QCOMPARE(model->rowCount(), expected);

    QSignalSpy spy(m_playerWidget.get(), &PlayerWidget::playerIsPlaying);
    QSignalSpy spy2(m_playerWidget.get(), &PlayerWidget::newSongPlayed);
    m_playerWidget->startMedia(model->getMediaByModelIndex(model->index(0)));

    spy.wait(TIME_SONG * 1000);
    spy2.wait();
    QCOMPARE(spy.count(), expected + 1);
    QCOMPARE(spy2.count(), expected);
}
void TestAudioPlayer::playSongInLoop_data()
{
    QTest::addColumn<QStringList>("songs");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QStringList({"qrc:/music/07.mp3"}) << 1;
}
QTEST_MAIN(TestAudioPlayer);

#include "tst_audioplayer.moc"
