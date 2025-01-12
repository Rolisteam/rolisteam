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
#include <QTest>
#include <QSignalSpy>

#include <QModelIndex>
#include <QModelIndexList>
#include <QtCore/QString>
#include <memory>

#include "controller/audiocontroller.h"
#include "controller/audioplayercontroller.h"
#include "model/musicmodel.h"
#include "rwidgets/customs/playerwidget.h"
#include "test_root_path.h"

constexpr int TIME_SONG{36};

class TestAudioPlayer : public QObject
{
    Q_OBJECT
public:
    TestAudioPlayer();

private slots:
    void init();

    void mode();

    void property_audio();

    void addSongTest();
    void addSongTest_data();

    void playSong();
    void playSong_data();

    void playSongInLoop();
    void playSongInLoop_data();

    void playerWidget();

private:
    std::unique_ptr<AudioPlayerController> m_audioController;
    std::unique_ptr<AudioController> m_audiosCtrl;
};

TestAudioPlayer::TestAudioPlayer() {}

void TestAudioPlayer::init()
{
    m_audioController.reset(new AudioPlayerController(0, "", nullptr));
    m_audioController->setLocalIsGm(true);
    m_audiosCtrl.reset(new AudioController(nullptr, nullptr));
    m_audiosCtrl->setLocalIsGM(true);
}

void TestAudioPlayer::playerWidget()
{
    PlayerWidget playerWidget(m_audioController.get());

    QMenu menu;
    playerWidget.addActionsIntoMenu(&menu);

    auto listAct= menu.actions();

    /*for(auto act: listAct)
    {
        //act->trigger();
    }*/
}

void TestAudioPlayer::mode()
{
    m_audioController->addSong(
        {QUrl("qrc:/music/07.mp3"),
         QUrl::fromUserInput(QString("file://%1/resources/%2").arg(tests::root_path, "break.mp3")),
         QUrl::fromUserInput(QString("file://%1/resources/%2").arg(tests::root_path, "quickFixes.mp3")),
         QUrl::fromUserInput(QString("file://%1/resources/%2").arg(tests::root_path, "taskFailed.mp3")),
         QUrl::fromUserInput(QString("file://%1/resources/%2").arg(tests::root_path, "taskCompleted.mp3")),
         QUrl::fromUserInput(QString("file://%1/resources/%2").arg(tests::root_path, "warning.mp3")),
         QUrl::fromUserInput(QString("file://%1/resources/%2").arg(tests::root_path, "error.mp3"))});
    auto model= m_audioController->model();
    m_audioController->setPlayingMode(AudioPlayerController::PlayingMode::NEXT);
    QSignalSpy spy(m_audioController.get(), &AudioPlayerController::stateChanged);
    m_audioController->setMedia(model->index(0, 0));
    QCOMPARE(model->indexOfCurrent(), 0);
    m_audioController->play();
    m_audioController->text();

    m_audioController->exportList(QString("file://%1/%2").arg(tests::root_path, "playlist.m3u"));

    m_audioController->mute();
    QVERIFY(m_audioController->muted());

    m_audioController->setVolume(20);
    QCOMPARE(m_audioController->volume(), 20);

    m_audioController->error();

    m_audioController->setVisible(true);
    m_audioController->setVisible(false);
    QVERIFY(!m_audioController->visible());
    m_audioController->setVisible(true);
    QVERIFY(m_audioController->visible());

    m_audioController->setTime(m_audioController->time() + 10);
    m_audioController->id();
    QCOMPARE(m_audioController->mode(), AudioPlayerController::PlayingMode::NEXT);
    spy.wait(TIME_SONG * 400);

    auto arg= spy.takeLast();

    QCOMPARE(arg[0].toInt(), AudioPlayerController::PlayingState);
    QCOMPARE(m_audioController->state(), AudioPlayerController::PlayingState);

    spy.clear();
    m_audioController->next();
    QVERIFY(model->indexOfCurrent() == 1);
    m_audioController->setPlayingMode(AudioPlayerController::PlayingMode::LOOP);

    spy.wait(TIME_SONG);
    spy.wait(TIME_SONG);

    arg= spy.takeLast();

    QCOMPARE(arg[0].toInt(), AudioPlayerController::PlayingState);

    m_audioController->next();
    QVERIFY(model->indexOfCurrent() == 1);

    spy.clear();
    m_audioController->pause();
    spy.wait(TIME_SONG);

    arg= spy.takeLast();

    QCOMPARE(arg[0].toInt(), AudioPlayerController::PausedState);

    m_audioController->setPlayingMode(AudioPlayerController::PlayingMode::UNIQUE);
    m_audioController->play();

    spy.wait(TIME_SONG);
    m_audioController->next();
    spy.wait(TIME_SONG);

    QCOMPARE(m_audioController->state(), AudioPlayerController::StoppedState);

    m_audioController->setPlayingMode(AudioPlayerController::PlayingMode::SHUFFLE);
    m_audioController->next();

    spy.wait(TIME_SONG * 10);
    m_audioController->next();
    spy.wait(TIME_SONG * 10);
    m_audioController->next();

    m_audioController->removeSong({model->index(0, 0)});

    m_audioController->nwNewSong("taskCompleted.mp3", 0);
    m_audioController->setLocalIsGm(false);
    m_audioController->nwNewSong("taskCompleted.mp3", 0);

    m_audioController->clear();
    m_audioController->next();

    m_audioController->loadPlayList(QString("file://%1/%2").arg(tests::root_path, "playlist.m3u"));
}

void TestAudioPlayer::property_audio()
{
    QVERIFY(m_audiosCtrl->localIsGM());
    QVERIFY(m_audiosCtrl->firstController() != nullptr);
    QVERIFY(m_audiosCtrl->secondController() != nullptr);
    QVERIFY(m_audiosCtrl->thirdController() != nullptr);

    m_audiosCtrl->setLocalIsGM(true);
    m_audiosCtrl->setLocalIsGM(false);

    QVERIFY(!m_audiosCtrl->firstController()->localIsGm());
    QVERIFY(!m_audiosCtrl->secondController()->localIsGm());
    QVERIFY(!m_audiosCtrl->thirdController()->localIsGm());
}

void TestAudioPlayer::addSongTest()
{
    QFETCH(QList<QUrl>, songs);
    QFETCH(int, expected);

    QVERIFY(m_audioController->localIsGm());

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
    QCOMPARE(spy2.count(), expected);
}
void TestAudioPlayer::playSongInLoop_data()
{
    QTest::addColumn<QList<QUrl>>("songs");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QList<QUrl>({QUrl("qrc:/music/07.mp3")}) << 1;
}
QTEST_MAIN(TestAudioPlayer);

#include "tst_audioplayer.moc"
