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

#include <QModelIndex>
#include <QModelIndexList>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <memory>

#include "core/model/musicmodel.h"

class TestMusicModel : public QObject
{
    Q_OBJECT
public:
    TestMusicModel();

private slots:
    void init();

    void addTest();
    void addTest_data();

    void insertTest();
    void removeTest();

private:
    std::unique_ptr<MusicModel> m_model;
};

TestMusicModel::TestMusicModel() {}

void TestMusicModel::init()
{
    m_model.reset(new MusicModel());
}

void TestMusicModel::addTest()
{
    QFETCH(QList<QUrl>, list);
    QFETCH(int, expected);

    QVERIFY(m_model->rowCount() == 0);

    m_model->addSong(list);

    QCOMPARE(m_model->rowCount(), expected);
}

void TestMusicModel::addTest_data()
{
    QTest::addColumn<QList<QUrl>>("list");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QList<QUrl>() << 0;
    QTest::addRow("list2") << QList<QUrl>({QUrl("song.mp3")}) << 1;
    QTest::addRow("list3") << QList<QUrl>({QUrl("song.mp3"), QUrl("song1.ogg")}) << 2;
    QTest::addRow("list3") << QList<QUrl>({QUrl("song.mp3"), QUrl("song1.ogg"), QUrl("song4.wav")}) << 3;
}

void TestMusicModel::removeTest()
{
    QList<QUrl> list;
    list << QUrl("song1.mp3") << QUrl("song2.ogg");

    m_model->addSong(list);

    QCOMPARE(m_model->rowCount(), 2);

    m_model->removeAll();

    QCOMPARE(m_model->rowCount(), 0);

    m_model->addSong(list);

    QCOMPARE(m_model->rowCount(), 2);

    QModelIndexList indexList;
    auto index= m_model->index(0, 0);
    indexList << index;
    m_model->removeSong(indexList);

    QCOMPARE(m_model->rowCount(), 1);

    m_model->removeSong(indexList);

    QCOMPARE(m_model->rowCount(), 0);
    indexList.clear();

    m_model->addSong(list);

    QCOMPARE(m_model->rowCount(), 2);

    index= m_model->index(0, 0);
    auto index1= m_model->index(1, 0);
    indexList << index << index1;
    m_model->removeSong(indexList);
    QCOMPARE(m_model->rowCount(), 0);
}

void TestMusicModel::insertTest()
{
    QList<QUrl> list;
    list << QUrl("/home/file/song1.mp3") << QUrl("/home/file/song2.ogg");
    m_model->addSong(list);

    QCOMPARE(m_model->rowCount(), 2);
    QString song("/home/file/songInserted.mp3");
    m_model->insertSong(1, song);

    QCOMPARE(m_model->rowCount(), 3);

    auto index= m_model->index(1, 0);
    QCOMPARE(index.data().toString(), "songInserted.mp3");
}

QTEST_MAIN(TestMusicModel);

#include "tst_musicmodel.moc"
