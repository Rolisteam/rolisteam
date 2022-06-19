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

#include "controller/audioplayercontroller.h"
#include "model/musicmodel.h"

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
    // std::unique_ptr<PlayerWidget> m_playerWidget;
    std::unique_ptr<AudioPlayerController> m_audioController;
};

TestAudioPlayer::TestAudioPlayer() {}

void TestAudioPlayer::init()
{
    m_audioController.reset(new AudioPlayerController(0, "", nullptr));
    m_audioController->setLocalIsGm(true);
    //    m_audioPlayer.reset(new AudioPlayer());
    // m_playerWidget.reset(new PlayerWidget(0));
}

void TestAudioPlayer::addSongTest()
{
    QFETCH(QList<QUrl>, songs);
    QFETCH(int, expected);

    m_audioController->addSong(songs);

    auto model= m_audioController->model();
    QCOMPARE(model->rowCount(), expected);
}

void TestAudioPlayer::addSongTest_data()
{
    QTest::addColumn<QList<QUrl>>("songs");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QList<QUrl>() << 0;
    QTest::addRow("list2") << QList<QUrl>({QUrl("song")}) << 1;
    QTest::addRow("list3") << QList<QUrl>({QUrl("song1"), QUrl("song2")}) << 2;
    QTest::addRow("list4") << QList<QUrl>({QUrl("song1"), QUrl("song2"), QUrl("song3")}) << 3;
}

void TestAudioPlayer::playSong()
{
    QFETCH(QList<QUrl>, songs);
    QFETCH(int, expected);

    m_audioController->addSong(songs);

    auto model= m_audioController->model();
    QCOMPARE(model->rowCount(), expected);

    QSignalSpy spy(m_audioController.get(), &AudioPlayerController::stateChanged);
    QSignalSpy spy2(m_audioController.get(), &AudioPlayerController::startPlayingSong);
    m_audioController->setMedia(model->index(0));
    m_audioController->play();

    spy.wait();
    spy2.wait();
    QCOMPARE(spy.count(), expected);
    QCOMPARE(spy2.count(), expected);
}

void TestAudioPlayer::playSong_data()
{
    QTest::addColumn<QList<QUrl>>("songs");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QList<QUrl>({QUrl("qrc:/music/07.mp3")}) << 1;
}

void TestAudioPlayer::playSongInLoop()
{
    QFETCH(QList<QUrl>, songs);
    QFETCH(int, expected);

    m_audioController->setPlayingMode(AudioPlayerController::LOOP);

    m_audioController->addSong(songs);

    auto model= m_audioController->model();
    QCOMPARE(model->rowCount(), expected);

    QSignalSpy spy(m_audioController.get(), &AudioPlayerController::stateChanged);
    QSignalSpy spy2(m_audioController.get(), &AudioPlayerController::startPlayingSong);
    m_audioController->setMedia(model->index(0));
    m_audioController->play();

    spy.wait((TIME_SONG + 10) * 1000);
    spy2.wait();
    QCOMPARE(spy.count(), expected);
    QCOMPARE(spy2.count(), expected + 2);
}
void TestAudioPlayer::playSongInLoop_data()
{
    QTest::addColumn<QList<QUrl>>("songs");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QList<QUrl>({QUrl("qrc:/music/07.mp3")}) << 1;
}
QTEST_MAIN(TestAudioPlayer);

#include "tst_audioplayer.moc"
