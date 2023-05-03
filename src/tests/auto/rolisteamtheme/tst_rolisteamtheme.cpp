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
#include <data/rolisteamtheme.h>
#include <helper.h>
#include <QStyle>
#include <QStyleFactory>

class RolisteamThemeTest : public QObject
{
    Q_OBJECT

public:
    RolisteamThemeTest();

private slots:
    void initTestCase();
    void setAndGetTest();
    void cleanupTestCase();

private:
    RolisteamTheme* m_theme;
};
RolisteamThemeTest::RolisteamThemeTest() {}

void RolisteamThemeTest::initTestCase()
{
    m_theme = new RolisteamTheme();
}

void RolisteamThemeTest::setAndGetTest()
{
    QPalette pal(Qt::blue);

    {
        QList<QPalette::ColorGroup> group{QPalette::ColorGroup::Disabled,QPalette::ColorGroup::Active,QPalette::ColorGroup::Inactive};
        QList<QPalette::ColorRole> role{QPalette::Highlight, QPalette::HighlightedText,
                                        QPalette::Link, QPalette::LinkVisited,
                                        QPalette::NoRole};
        for(auto g : group)
        {
            for(auto r : role)
            {
                pal.setColor(g, r, pal.color(g, r));
            }
        }
    }
    m_theme->setPalette(pal);

    QList<QPalette::ColorGroup> group{QPalette::ColorGroup::Disabled,QPalette::ColorGroup::Active,QPalette::ColorGroup::Inactive};
    QList<QPalette::ColorRole> role{QPalette::WindowText, QPalette::Button, QPalette::Light, QPalette::Midlight, QPalette::Dark, QPalette::Mid,
        QPalette::Text, QPalette::BrightText, QPalette::ButtonText, QPalette::Base, QPalette::Window, QPalette::Shadow,
        QPalette::Highlight, QPalette::HighlightedText,
        QPalette::Link, QPalette::LinkVisited,
        QPalette::AlternateBase,
        QPalette::NoRole,
        QPalette::ToolTipBase, QPalette::ToolTipText,
        QPalette::PlaceholderText};
    for(auto g : group)
    {
        for(auto r : role)
        {
            QCOMPARE(m_theme->getPalette().color(g, r), pal.color(g, r));
        }
    }
    QCOMPARE(m_theme->getPalette(), pal);

    auto name = Helper::randomString();
    m_theme->setName(name);
    QCOMPARE(m_theme->getName(), name);

    auto css  = Helper::randomString();
    m_theme->setCss(css);
    QCOMPARE(m_theme->getCss(), css);


    m_theme->setRemovable(true);
    QVERIFY(m_theme->isRemovable());
    m_theme->setRemovable(false);
    QVERIFY(!m_theme->isRemovable());

    auto style = QStyleFactory::create("fusion");
    m_theme->setStyle(style);
    QCOMPARE(m_theme->getStyle()->objectName(), style->objectName());
    QCOMPARE(m_theme->getStyleName(), "fusion");

    auto imgPath = Helper::randomString();
    m_theme->setBackgroundImage(imgPath);
    QCOMPARE(m_theme->getBackgroundImage(), imgPath);

    auto bgColor = Helper::randomColor();
    m_theme->setBackgroundColor(bgColor);
    QCOMPARE(m_theme->getBackgroundColor(), bgColor);

    auto position = Helper::generate<int>(0,10);
    m_theme->setBackgroundPosition(position);
    QCOMPARE(m_theme->getBackgroundPosition(), position);

    auto diceColor = Helper::randomColor();
    m_theme->setDiceHighlightColor(diceColor);
    QCOMPARE(m_theme->getDiceHighlightColor(), diceColor);

}
void RolisteamThemeTest::cleanupTestCase()
{
    delete m_theme;
}

QTEST_MAIN(RolisteamThemeTest);

#include "tst_rolisteamtheme.moc"
