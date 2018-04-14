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

}

void SessionTest::testManager()
{
    QSignalSpy spy(m_sessionManager, &SessionManager::sessionChanged);
    CleverURI resources;
    m_sessionManager->addRessource(&resources);
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
