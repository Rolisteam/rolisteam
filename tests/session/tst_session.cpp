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

#include <sessionmanager.h>
#include <sessionitemmodel.h>
#include <character.h>
#include <QMouseEvent>

class SessionTest : public QObject
{
    Q_OBJECT

public:
    SessionTest();

private slots:
    void initTestCase();
    void testModel();
    void testManager();
    void cleanupTestCase();

private:
    SessionManager* m_sessionManager;
    SessionItemModel* m_model;
};

SessionTest::SessionTest()
{

}

void SessionTest::initTestCase()
{
    m_sessionManager = new SessionManager(nullptr);
    m_model = m_sessionManager->getModel();
}

void SessionTest::testModel()
{
    m_sessionManager->show();
    QApplication::processEvents();

    CleverURI* uri1 = new CleverURI("uri1","/path/uri1",CleverURI::CHARACTERSHEET);
    CleverURI* uri2  = new CleverURI("uri2","/path/uri2",CleverURI::VMAP);
    Character* character = new Character("character1",Qt::red,true);
    Chapter* chapter = new Chapter();
    chapter->setName("chapter1");

    auto sessionview = m_sessionManager->getView();

    QModelIndex index;
    m_model->addResource(uri1,index);
    m_model->addResource(uri2,index);
    m_model->addResource(character,index);
    m_model->addResource(chapter,index);

    QVERIFY2(m_model->rowCount(index) == 4, "NOT the excepted number of data");

    QSignalSpy layout(m_model, &SessionItemModel::layoutChanged);
    QPoint local(10,10);
    QMouseEvent* event = new QMouseEvent(QEvent::MouseButtonPress,local,Qt::LeftButton,nullptr,nullptr);
    QApplication::postEvent(m_sessionManager, event);
    QApplication::processEvents();

    for(int i = 0; i < 55; ++i)
    {
        QPoint moveMouse(10,10+i);
        QMouseEvent* eventMove = new QMouseEvent(QEvent::MouseMove,moveMouse,Qt::LeftButton,Qt::LeftButton,nullptr);
        QApplication::postEvent(m_sessionManager, eventMove);
        QApplication::processEvents();
    }

    QPoint moveMouse(10,65);
    QMouseEvent* eventMove = new QMouseEvent(QEvent::MouseMove,moveMouse,Qt::LeftButton,Qt::LeftButton,nullptr);
    QApplication::postEvent(m_sessionManager, eventMove);
    QApplication::processEvents();

    QPoint moveRelease(10,65);
    auto index2 = sessionview->indexAt(moveRelease);
    QMouseEvent* eventRelease = new QMouseEvent(QEvent::MouseButtonRelease,moveRelease,Qt::LeftButton,nullptr,nullptr);
    QApplication::postEvent(m_sessionManager, eventRelease);
    QApplication::processEvents();

    qDebug() << m_model->rowCount(index) << chapter << character << uri2 << uri1 ;

    qDebug() << "rowCount:" <<m_model->rowCount(index);
    QVERIFY2(m_model->rowCount(index) == 4, "NOT the excepted number of data");

    //QVERIFY2(layout.count() == 1, "Drag and drop not working");

    m_model->clearData();
}

void SessionTest::testManager()
{
    QSignalSpy spy(m_sessionManager, &SessionManager::sessionChanged);
    CleverURI* resources = new CleverURI();
    m_sessionManager->addRessource(resources);
    QVERIFY2(spy.count() == 1,"no signal about change in session");

    QSignalSpy spyOpen(m_sessionManager, &SessionManager::openResource);
    auto index = m_model->index(0,0);
    m_sessionManager->openResources(index);
    QVERIFY2(spyOpen.count() == 1,"no signal about change in session");
}

void SessionTest::cleanupTestCase()
{
    delete m_sessionManager;
}
QTEST_MAIN(SessionTest);

#include "tst_session.moc"
