/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   Copyright (C) 2015 by Benoit Lagarde                                  *
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

#include <colorbutton.h>

class ColorButtonTest : public QObject
{
    Q_OBJECT

public:
    ColorButtonTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void getAndSetTest();
    void colorChangedTest();
    void colorChangedTwiceTest();
    void colorChangedTwiceDifferentTest();

private:
    ColorButton* m_colorButton;
};
ColorButtonTest::ColorButtonTest()
{

}
void ColorButtonTest::initTestCase()
{
    m_colorButton =new ColorButton(nullptr,false);
}

void ColorButtonTest::getAndSetTest()
{
    QColor  test_color(12,74,181);
    m_colorButton->setColor(test_color);
    QColor result = m_colorButton->color();
    QVERIFY(result==test_color);
}
void ColorButtonTest::colorChangedTest()
{
    QColor  test_color(12,74,182);
    QSignalSpy spy(m_colorButton, SIGNAL(colorChanged(QColor)));
    m_colorButton->setColor(test_color);
    QCOMPARE(spy.count(), 1);
}
void ColorButtonTest::colorChangedTwiceTest()
{
    QColor  test_color(12,74,183);
    QSignalSpy spy(m_colorButton, SIGNAL(colorChanged(QColor)));
    m_colorButton->setColor(test_color);
    m_colorButton->setColor(test_color);
    QCOMPARE(spy.count(), 1);
}
void ColorButtonTest::colorChangedTwiceDifferentTest()
{
    QColor  test_color(12,74,184);
    QColor  test_color2(74,180,12);
    QSignalSpy spy(m_colorButton, SIGNAL(colorChanged(QColor)));
    m_colorButton->setColor(test_color);
    m_colorButton->setColor(test_color2);
    QCOMPARE(spy.count(), 2);
}
void ColorButtonTest::cleanupTestCase()
{
    delete m_colorButton;
}


QTEST_MAIN(ColorButtonTest);

#include "tst_colorButton.moc"
