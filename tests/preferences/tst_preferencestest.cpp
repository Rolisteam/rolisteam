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
#include <QtTest/QtTest>

#include <preferencesmanager.h>

class PreferencesTest : public QObject
{
    Q_OBJECT

public:
    PreferencesTest();

private slots:
    void testPreferenceRegisterValue();
    void initTestCase();
    void testNotOverridePreferenceValue();
    void testOverridePreferenceValue();
    void cleanupTestCase();

private:
    PreferencesManager* m_preferences;
};

PreferencesTest::PreferencesTest()
{

}


void PreferencesTest::testPreferenceRegisterValue()
{
    m_preferences->registerValue("key",300);

    QVERIFY(m_preferences->value("key",400)==300);
}
void PreferencesTest::testNotOverridePreferenceValue()
{
    m_preferences->registerValue("key1",300,false);
    QVERIFY(m_preferences->value("key1",400)==300);
    m_preferences->registerValue("key1",100,false);
    QVERIFY(m_preferences->value("key1",400)==300);
}
void PreferencesTest::testOverridePreferenceValue()
{
    m_preferences->registerValue("key2",300);
    QVERIFY(m_preferences->value("key2",400)==300);
    m_preferences->registerValue("key2",100);
    QVERIFY(m_preferences->value("key2",400)==100);
}
void PreferencesTest::initTestCase()
{
    m_preferences = PreferencesManager::getInstance();
}

void PreferencesTest::cleanupTestCase()
{

}


QTEST_MAIN(PreferencesTest);

#include "tst_preferencestest.moc"
