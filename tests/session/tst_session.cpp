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
#include <QtTest/QtTest>

#include "data/cleveruri.h"
#include <QMouseEvent>
#include <character.h>
#include <helper.h>
#include <sessionitemmodel.h>
#include <sessionmanager.h>

class SessionTest : public QObject
{
    Q_OBJECT

public:
    SessionTest();

private slots:
    void init();
    void testModel();
    void testManager();
    void cleanupTestCase();
    void saveAndLoadTest();
    void saveAndLoadTest_data();

private:
    std::unique_ptr<SessionManager> m_sessionManager;
    SessionItemModel* m_model;
};

Q_DECLARE_METATYPE(std::vector<CleverURI::ContentType>);

SessionTest::SessionTest()
{
    qRegisterMetaType<ResourcesNode*>("ResourcesNode*");
}

void SessionTest::init()
{
    m_sessionManager.reset(new SessionManager(nullptr));
    m_model= m_sessionManager->getModel();
}

void SessionTest::testModel()
{
    m_sessionManager->show();
    QApplication::processEvents();

    CleverURI* uri1= new CleverURI("uri1", "/path/uri1", CleverURI::CHARACTERSHEET);
    CleverURI* uri2= new CleverURI("uri2", "/path/uri2", CleverURI::VMAP);
    Character* character= new Character("character1", Qt::red, true);
    Chapter* chapter= new Chapter();
    chapter->setName("chapter1");

    auto sessionview= m_sessionManager->getView();

    QModelIndex index;
    m_model->addResource(uri1, index);
    m_model->addResource(uri2, index);
    m_model->addResource(character, index);
    m_model->addResource(chapter, index);

    QVERIFY2(m_model->rowCount(index) == 4, "NOT the excepted number of data");

    QSignalSpy layout(m_model, &SessionItemModel::layoutChanged);
    QPoint local(10, 10);
    QTest::mouseClick(m_sessionManager.get(), Qt::LeftButton, Qt::KeyboardModifiers(), local);

    for(int i= 0; i < 55; ++i)
    {
        QPoint moveMouse(10, 10 + i);
        QTest::mouseMove(m_sessionManager.get(), moveMouse);
    }

    QTest::mouseMove(m_sessionManager.get(), QPoint(10, 65));
    QTest::mouseRelease(m_sessionManager.get(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(10, 65));

    qDebug() << m_model->rowCount(index) << chapter << character << uri2 << uri1;

    qDebug() << "rowCount:" << m_model->rowCount(index);
    QVERIFY2(m_model->rowCount(index) == 4, "NOT the excepted number of data");

    // QVERIFY2(layout.count() == 1, "Drag and drop not working");

    m_model->clearData();
}

void SessionTest::testManager()
{
    QSignalSpy spy(m_sessionManager.get(), &SessionManager::sessionChanged);
    CleverURI* resources= new CleverURI();
    m_sessionManager->addRessource(resources);
    QVERIFY2(spy.count() == 1, "no signal about change in session");

    QSignalSpy spyOpen(m_sessionManager.get(), &SessionManager::openResource);
    auto index= m_model->index(0, 0);
    m_sessionManager->openResources(index);
    QVERIFY2(spyOpen.count() == 1, "no signal about change in session");
}

void SessionTest::saveAndLoadTest()
{
    QFETCH(std::vector<CleverURI::ContentType>, list);

    auto imageUri= new CleverURI("Girafe", ":/assets/img/girafe.jpg", CleverURI::PICTURE);
    auto characterSheet= new CleverURI("bitume", ":/assets/charactersheet/bitume_fixed.rcs", CleverURI::CHARACTERSHEET);
    auto vmap= new CleverURI("vmap", ":/assets/vmap/test.vmap", CleverURI::VMAP);
    auto text= new CleverURI("text", ":/assets/notes/test.odt", CleverURI::TEXT);
    auto sharednote= new CleverURI("sharednote", ":/assets/sharednotes/test.rsn", CleverURI::SHAREDNOTE);
    auto playlist= new CleverURI("playlist", ":/assets/list/list.m3u", CleverURI::SONGLIST);
    auto webview= new CleverURI("webview", "http://www.rolisteam.org", CleverURI::WEBVIEW);

    std::vector<CleverURI*> data;
    for(auto id : list)
    {
        CleverURI* uri= nullptr;
        switch(id)
        {
        case CleverURI::PICTURE:
            uri= imageUri;
            break;
        case CleverURI::CHARACTERSHEET:
            uri= characterSheet;
            break;
        case CleverURI::TEXT:
            uri= text;
            break;
        case CleverURI::VMAP:
            uri= vmap;
            break;
        case CleverURI::SHAREDNOTE:
            uri= sharednote;
            break;
        case CleverURI::SONGLIST:
            uri= playlist;
            break;
        case CleverURI::WEBVIEW:
            uri= webview;
            break;
        }
        data.push_back(uri);
    }

    QSignalSpy spy(m_sessionManager.get(), &SessionManager::sessionChanged);
    for(auto uri : data)
    {
        m_sessionManager->addRessource(uri);
    }
    QCOMPARE(spy.count(), list.size());

    QByteArray array;
    QDataStream in(&array, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_7);

    m_sessionManager->saveSession(in);

    std::unique_ptr<SessionManager> sessionManager(new SessionManager(nullptr));
    QDataStream out(&array, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_7);
    sessionManager->loadSession(out);

    QCOMPARE(
        sessionManager->getModel()->rowCount(QModelIndex()), m_sessionManager->getModel()->rowCount(QModelIndex()));
    QByteArray array2;
    QDataStream in2(&array2, QIODevice::WriteOnly);
    in2.setVersion(QDataStream::Qt_5_7);
    sessionManager->saveSession(in2);

    QCOMPARE(array.size(), array2.size());
}

void SessionTest::saveAndLoadTest_data()
{
    QTest::addColumn<std::vector<CleverURI::ContentType>>("list");

    std::vector<CleverURI::ContentType> data({CleverURI::VMAP, CleverURI::SHAREDNOTE, CleverURI::CHARACTERSHEET,
        CleverURI::TEXT, CleverURI::PICTURE, CleverURI::SONGLIST, CleverURI::WEBVIEW}); //

    // auto list = new std::vector<CleverURI*>();
    std::vector<CleverURI::ContentType> list;

    int index= 0;
    for(int i= 0; i < data.size(); ++i)
    {
        std::size_t comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                list.push_back(*it);
            }
            auto title= QStringLiteral("save%1").arg(++index);
            QTest::addRow(title.toStdString().c_str()) << list;
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}

void SessionTest::cleanupTestCase() {}
QTEST_MAIN(SessionTest);

#include "tst_session.moc"
