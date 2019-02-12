/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
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
#include <QObject>
#include <QtTest/QtTest>
#include <rolisteamtheme.h>

class RolisteamThemeTest : public QObject
{
    Q_OBJECT

public:
    RolisteamThemeTest();

private slots:
    void testPreferenceRegisterValue();
    void initTestCase();
    void testNotOverridePreferenceValue();
    void testOverridePreferenceValue();
    void cleanupTestCase();

private:
    RolisteamTheme* m_theme;
};
RolisteamThemeTest::RolisteamThemeTest() {}

void RolisteamThemeTest::testPreferenceRegisterValue() {}
void RolisteamThemeTest::testNotOverridePreferenceValue() {}
void RolisteamThemeTest::testOverridePreferenceValue() {}
void RolisteamThemeTest::initTestCase() {}

void RolisteamThemeTest::cleanupTestCase() {}

QTEST_MAIN(RolisteamThemeTest);

#include "tst_rolisteamtheme.moc"
