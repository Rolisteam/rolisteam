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

#include "widgets/circledisplayer.h"
#include "widgets/colorselector.h"
#include "widgets/diameterselector.h"
#include "widgets/filedirchooser.h"
#include "widgets/realslider.h"

class WidgetsTest : public QObject
{
    Q_OBJECT

public:
    WidgetsTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void realSliderTest();
    void fileDirChooserTest();
    void diameterSelectorTest();

private:
    std::unique_ptr<RealSlider> m_realSlider;
    std::unique_ptr<FileDirChooser> m_fileDirChooser;
    std::unique_ptr<DiameterSelector> m_diameter;
};
WidgetsTest::WidgetsTest() {}
void WidgetsTest::initTestCase()
{
    m_realSlider.reset(new RealSlider());
    m_fileDirChooser.reset(new FileDirChooser());
    m_diameter.reset(new DiameterSelector());
}

void WidgetsTest::cleanupTestCase() {}

void WidgetsTest::realSliderTest()
{
    qreal step= 0.01;
    m_realSlider->setStep(step);
    QCOMPARE(m_realSlider->step(), step);

    qreal end= 100.;
    m_realSlider->setEnd(end);
    QCOMPARE(m_realSlider->end(), end);

    qreal start= 0.;
    m_realSlider->setStart(start);
    QCOMPARE(m_realSlider->start(), start);

    m_realSlider->setRealValue(50.);

    QSignalSpy spy(m_realSlider.get(), &RealSlider::valueChanged);

    m_realSlider->triggerAction(QAbstractSlider::SliderSingleStepAdd);
    /*QCOMPARE(spy.count(), 1);

    auto args = spy.takeFirst();
    QVERIFY(qFuzzyCompare(args.at(0).toReal(),50.01));

    m_realSlider->triggerAction(QAbstractSlider::SliderSingleStepSub);
    QCOMPARE(spy.count(), 2);

    args = spy.takeFirst();
    QVERIFY(qFuzzyCompare(args.at(0).toReal(),50.0));*/
}

void WidgetsTest::fileDirChooserTest()
{
    QString path("/home/path/place");
    m_fileDirChooser->setPath(path);
    QCOMPARE(m_fileDirChooser->path(), path);

    QString filter("*.png");
    m_fileDirChooser->setFilter(filter);
    QCOMPARE(m_fileDirChooser->getFilter(), filter);
}

void WidgetsTest::diameterSelectorTest()
{
    auto val= 50;
    m_diameter->setDiameter(val);
    QCOMPARE(m_diameter->getCurrentValue(), val);
}

QTEST_MAIN(WidgetsTest);

#include "tst_widgets.moc"
