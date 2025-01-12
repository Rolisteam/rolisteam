/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   renaud@rolisteam.org                   *
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
#include <QObject>

#include <data/diceshortcut.h>
#include <helper.h>

class DiceShortCutTest : public QObject
{
    Q_OBJECT

public:
    DiceShortCutTest();

private Q_SLOTS:
    void setAndGetTest();

private:
    DiceShortCut* m_diceSc;
};

DiceShortCutTest::DiceShortCutTest()
{

}

void DiceShortCutTest::setAndGetTest()
{
    m_diceSc = new DiceShortCut();
    m_diceSc->setAlias(true);
    QVERIFY(m_diceSc->alias());

    m_diceSc->setAlias(false);
    QVERIFY(!m_diceSc->alias());

    auto cmd = Helper::randomString();

    m_diceSc->setCommand(cmd);
    QCOMPARE(m_diceSc->command(), cmd);

    auto text = Helper::randomString();

    m_diceSc->setText(text);
    QCOMPARE(m_diceSc->text(), text);

    delete m_diceSc;
}

QTEST_MAIN(DiceShortCutTest);

#include "tst_diceshortcut.moc"
