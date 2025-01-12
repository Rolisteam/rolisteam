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
#include <QTest>
#include <QSignalSpy>

#include <memory.h>
#include "common_qml/theme.h"
#include "test_root_path.h"
#include <helper.h>

class ThemeTest : public QObject
{
    Q_OBJECT
public:
    ThemeTest();


private slots:
    void init();
    void getAndSet();

private:
    std::unique_ptr<customization::Theme> m_theme;
};



ThemeTest::ThemeTest()
{

}

void ThemeTest::init()
{
    customization::Theme::setPath(QString("%1/../../resources/stylesheet/qml/theme.ini").arg(tests::root_path));
    m_theme.reset(new customization::Theme);
}
void ThemeTest::getAndSet()
{
    auto styleSheet = m_theme->styleSheet("InstantMessaging");

    auto key = Helper::randomString();
    auto value = Helper::randomString();
    styleSheet->insertOrUpdate(key, QVariant::fromValue(value));

    QCOMPARE(styleSheet->value(key), value);

    QSignalSpy spy(m_theme.get(), &customization::Theme::nightModeChanged);

    m_theme->setNightMode(true);
    m_theme->setNightMode(true);
    m_theme->setNightMode(false);

    spy.wait();
    QCOMPARE(spy.count(), 2);
}

QTEST_MAIN(ThemeTest)

#include "tst_theme.moc"
