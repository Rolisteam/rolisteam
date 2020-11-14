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

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>

#include "controller/editorcontroller.h"

class TestEditorController : public QObject
{
    Q_OBJECT

public:
    TestEditorController();

private slots:
    void initTestCase();
    void getAndSetTest();
    void commandsTest();
    void wrongCommandsTest();
    void cleanupTestCase();

private:
    EditorController* m_ctrl;
};

TestEditorController::TestEditorController() {}

void TestEditorController::initTestCase()
{
    m_ctrl= new EditorController();
}

void TestEditorController::getAndSetTest() {}

void TestEditorController::commandsTest()
{
}
void TestEditorController::addPageTest()
{
    Q_FETCH(int, numberOfPage)
    Q_FETCH(QVector<int>, idpages)

    for(int i=0; i< numberOfPage; ++i)
    {
        m_ctrl->addPage();
    }

    // check id of each page

}

void TestEditorController::addPageTest_data()
{
    Q
}
void TestEditorController::cleanupTestCase()
{
}

QTEST_MAIN(TestEditorController);

#include "tst_editorcontroller.moc"
